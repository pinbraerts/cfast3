#ifndef CFAST_BOUND_TRAITS_HPP
#define CFAST_BOUND_TRAITS_HPP

#include <bitset>

#include "../Analysis/Traits.hpp"

namespace cf::rt {

struct BoundTraits {
	static constexpr size_t size = 17;
	std::bitset<size> direction = { 0b11011111111111100 };

	enum dir {
		ltr = 0b10000000,
		rtl = 0b01000000,
	};

	enum num {
		multiplie = 0,
		binary = 1,
		unary = 2
	};

	std::vector<byte> flags {
		multiplie,
		binary | ltr,
		unary | binary | ltr,
		unary | ltr,
		unary | binary | ltr,
		unary | binary | ltr,
		binary | ltr,
		binary | ltr,
		binary | ltr,
		binary | ltr,
		unary | binary | ltr,
		binary | ltr,
		binary | ltr,
		binary | ltr,
		binary | ltr,
		multiplie,
		binary | rtl,
		multiplie
	};

	bool Is(const SyntaxNode& source, byte flag) {
		if (source.priority >= flags.size()) return false;
		else if (flag == multiplie) return flags[source.priority] == multiplie;
		else return flags[source.priority] & flag;
	}
};

} // namespace cf::rt

#endif // !CFAST_BOUND_TRAITS_HPP
