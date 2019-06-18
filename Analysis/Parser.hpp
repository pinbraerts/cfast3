#ifndef CFAST_PARSER_HPP
#define CFAST_PARSER_HPP

#include "SyntaxNode.hpp"
#include "Lexer.hpp"
#include "Tree.hpp"
#include "ParserTraits.hpp"

namespace cf {
	
struct Parser {
	SyntaxTree& tree;
	Lexer::Iter iter;
	ParserTraits traits;
	bool eat_lines = false;

	Parser(SyntaxTree& _tree, Lexer& lexer): tree(_tree), iter(lexer.begin()) {
		tree.PushChild(Token::Container); // root
	}

	struct Error {
		std::string message;

		Error(std::string msg) noexcept: message(std::move(msg)) {}
		Error() = default;

		static Error ok() noexcept {
			return Error();
		}

		bool is_ok() const noexcept {
			return message.empty();
		}
	};

	void EatSpaces() {
		for (; iter->type != Token::End &&
			(
				iter->type == Token::Space ||
				(eat_lines && iter->type == Token::Line)
			); ++iter) {
			tree.PushChild(*iter);
		}
		eat_lines = false;
	}

	void ParseQuote() {
		tree.last() = Token(Token::Quote);
		const SyntaxNode& opening = tree.PushChild(*iter); // opening

		// TODO add string literal features
		tree.PushChildAndSelect(Token::String); // fill string literal
			tree.last().begin() = iter->end();
			for (++iter; iter->type != Token::End &&
				(iter->type != Token::Quote || *iter != opening); ++iter)
				if (iter->type == Token::Operator && iter->begin().chr() == '\\')
					++iter;
			if (iter->type != Token::Quote)
				throw Error("quote is not closed");

			tree.last().end() = iter->begin();
		tree.GoUp();

		tree.PushChild(*iter); // closure

		tree.GoUp();
		++iter;
	}

	void ParseBody() {
		while (true) {
			eat_lines = true;
			tree.PushChildAndSelect(Token::Space);
			EatSpaces();

			switch (iter->type) {
			case Token::End:
				tree.last() = Token::End;
				while (tree.last().parent != 0)
					tree.MoveUp();
				throw Error::ok();
			case Token::Operator:
			{
				TreePtr container = tree.last().parent;
				tree.last() = *iter;
				priority_t p = traits.GetPriority(*iter);
				tree.last().priority = p;

				if (p != tree[container].priority) {
					// bubble up to lower priority
					for (; p > tree[container].priority && container != 0; container = tree[container].parent)
						tree.MoveUp();

					if (p != tree[container].priority) {
						auto& ch = tree[container].children;
						auto i = ch.end();
						do --i;
						while (ch.begin() < i && (tree[*i].priority <= p));
						if (tree[*i].priority > p)
							++i;

						tree.Select(tree.InsertAndCapture(i, Token::Container, p));
					}
					else tree.GoUp();
				}
				else tree.GoUp();
				++iter;
			}
			break;
			case Token::CloseBrace:
				return;
			case Token::String:
				ParseString();
				break;
			case Token::Quote:
				ParseQuote();
				break;
			case Token::OpenBrace:
				ParseBraces();
				break;
			case Token::Space:
			case Token::Container: default:
				throw Error("unexpected token type: "s + Token::TypeToString(iter->type));
			case Token::Error:
				throw Error(std::string(iter->begin().ptr(), iter->end().ptr()));
			}
		}
	}

	Error Parse(void(Parser::*f)() = &Parser::ParseBody) {
		if (f == nullptr)
			return Error("Cannot call null parser");
		try {
			(this->*f)();
		}
		catch (const Error& e) {
			return e;
		}
		catch (const std::exception& e) {
			return Error(e.what());
		}
		return Error::ok();
	}

	void ParseString() {
		tree.last() = *iter;
		++iter;
		tree.GoUp();
	}
	void ParseBraces() {
		tree.last() = Token(Token::Container);
		tree.last().priority = 100;
		const TreePtr container = tree.LastPosition();
		tree.PushChild(*iter, 100); // opening

		++iter;
		ParseBody();

		if (iter->type != Token::CloseBrace) // TODO add open/close check
			throw Error("braces is not closed");

		tree.last() = *iter;
		// return to where it started
		while (tree.last().parent != container && tree.last().parent != 0)
			tree.MoveUp();
		++iter;
		tree.GoUp();
		tree.last().priority = 0;
		tree.GoUp();
	}
};

} // namespace cf

#endif // !CFAST_PARSER_HPP