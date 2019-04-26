#ifndef CFAST_PARSER_HPP
#define CFAST_PARSER_HPP

#include "SyntaxNode.hpp"
#include "Lexer.hpp"
#include "Tree.hpp"

namespace cf {

struct Parser {
	SyntaxTree& tree;
	Lexer::Iter iter;

	Parser(SyntaxTree& _tree, Lexer& lexer): tree(_tree), iter(lexer.begin()) {
		tree.push_child(Token::Container); // root
	}

	struct Error {
		std::string message;

		Error(std::string msg): message(std::move(msg)) {}
		Error() = default;

		static Error ok() {
			return Error();
		}

		bool is_ok() const {
			return message.empty();
		}
	};

	void skip_spaces() {
		for (; iter->type != Token::End && iter->type == Token::Space; ++iter) {
			tree.push_child(*iter);
			tree[tree.last().parent].working_area.begin() += 1;
		}
	}

	void skip_spaces_and_lines() {
		for (; iter->type != Token::End && (iter->type == Token::Space || iter->type == Token::Line); ++iter) {
			tree.push_child(*iter);
			tree[tree.last().parent].working_area.begin() += 1;
		}
	}

	void parseQuote() {
		tree.last() = Token(Token::Quote);
		Source opening = tree.push_child(*iter); // opening
		TreePtr body = tree | tree.push_child(Token::String); // body

		tree.select(body); // === tree.down(), tree.next()
		tree.last().begin() = iter->end();
		for (++iter; iter->type != Token::End &&
			(iter->type != Token::Quote || *iter != opening); ++iter)
			if (iter->type == Token::SingleOperator && iter->begin().chr() == '\\')
				++iter;
		if (iter->type != Token::Quote)
			throw Error("quote is not closed");

		tree.last().end() = iter->begin();

		tree.up();
		tree.push_child_and_select(Token::Quote); // closure
		tree.last() = *iter;
		tree.up();
		tree.up();

		++iter;
	}
	
	void parseBody() {
		while (true) {
			tree.push_child_and_select(Token::Space); // reserve
			skip_spaces_and_lines();

			switch (iter->type) {
			case Token::End:
				tree.last() = Token(Token::End);
				throw Error::ok();
			case Token::Operator:
				// TODO
				break;
			case Token::SingleOperator:
				// TODO
				break;
			case Token::String:
				parseString();
				break;
			case Token::Quote:
				parseQuote();
				break;
			case Token::OpenBrace:
				parseBraces();
				break;
			case Token::CloseBrace:
				tree.move_up();
				return;
			case Token::Line: case Token::Space:
			case Token::Container: default:
				throw Error("unexpected token type: "s + Token::type2str(iter->type));
			case Token::Error:
				throw Error(std::string(iter->begin().ptr(), iter->end().ptr()));
			}
		}
	}

	Error parse(void(Parser::*f)() = &Parser::parseBody) {
		try {
			(this->*f)();
		}
		catch (const Error& e) {
			return e;
		}
		return Error::ok();
	}

	void parseString() {
		tree.last() = *iter;
		++iter;
		tree.up();
	}

	void parseBraces() {
		tree.last() = Token(Token::OpenBrace);
		Source opening = tree.push_child(*iter); // opening

		++iter;
		tree.push_child_and_select(Token::OpenBrace);
		parseBody();
		//tree.up();
		if (iter->type != Token::CloseBrace) // add open/close check
			throw Error("braces is not closed");

		tree.last() = *iter;
		++iter;
		tree.up();
		tree.up();
	}
};

} // namespace cf

#endif // !CFAST_PARSER_HPP
