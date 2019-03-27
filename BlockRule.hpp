#ifndef CFAST_BLOCK_RULE_HPP
#define CFAST_BLOCK_RULE_HPP

#include "Rule.hpp"

template<
	class Start,
	class End,
	class Separator,
	class Item
>
struct BlockRule {
	Start start;
	End end;
	Separator separator;
	Item item;

	BlockRule(
		Start&& _start,
		End&& _end,
		Separator&& _separator,
		Item&& _item
	) noexcept :
		start(rule_move(_start)),
		end(rule_move(_end)),
		separator(rule_move(_separator)),
		item(rule_move(_item)) {}

	ErrorProcessor& operator()(Parser& p) {
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

template<
	class Start,
	class End,
	class Separator,
	class Item
>
inline auto block(
	Start&& start,
	End&& end,
	Separator&& separator,
	Item&& item
) {
	return BlockRule<Start, End, Separator, Item>(
		rule_move(start),
		rule_move(end),
		rule_move(separator),
		rule_move(item)
	);
}

template<class Item>
inline auto block(
	const char(&start_sep_end)[4],  // example: "{,}"
	Item&& item
) {
	return BlockRule<
		decltype(rule_cast('{')),
		decltype(rule_cast(',')),
		decltype(rule_cast('}')),
		Item
	>(
		rule_cast(start_sep_end[0]),
		rule_cast(start_sep_end[2]),
		rule_cast(start_sep_end[1]),
		rule_move(item)
	);
}



#endif // !CFAST_BLOCK_RULE_HPP
