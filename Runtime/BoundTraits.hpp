#ifndef CFAST_BOUND_TRAITS_HPP
#define CFAST_BOUND_TRAITS_HPP

#include <bitset>

#include "../Analysis/Traits.hpp"

namespace cf::rt {

struct BoundTraits {
	static constexpr size_t size = 17;
	std::bitset<size> direction = { 0b11011111111111100 };

	bool ltr(priority_t op) {
		if (op > size)
			return 0;
		return direction[op];
	}
};

} // namespace cf::rt

#endif // !CFAST_BOUND_TRAITS_HPP
