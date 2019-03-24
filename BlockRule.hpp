#ifndef CFAST_BLOCK_RULE_HPP
#define CFAST_BLOCK_RULE_HPP

#include "Parser.hpp"

struct BlockRule : Rule {
	RulePtr
		start,
		end,
		separator,
		item;

	BlockRule(
		RulePtr&& _start,
		RulePtr&& _end,
		RulePtr&& _separator,
		RulePtr&& _item
	) noexcept :
		start(std::move(_start)),
		end(std::move(_end)),
		separator(std::move(_separator)),
		item(std::move(_item)) {}

	ErrorProcessor& apply(Parser& p) override {
		p >> start;
		while (!bool(p >> end)) {
			p >> item;
			if (!bool(p >> separator)) {
				p >> end;
				break;
			}
		}
		return p;
	}
};

inline RulePtr block(
	RulePtr&& start,
	RulePtr&& end,
	RulePtr&& separator,
	RulePtr&& item
) {
	return std::make_unique<BlockRule>(
		std::move(start),
		std::move(end),
		std::move(separator),
		std::move(item)
	);
}

inline RulePtr block(
	const char(&start_sep_end)[4],  // example: "{,}"
	RulePtr&& item
) {
	return std::make_unique<BlockRule>(start_sep_end[0], start_sep_end[2], start_sep_end[1], std::move(item));
}



#endif // !CFAST_BLOCK_RULE_HPP
