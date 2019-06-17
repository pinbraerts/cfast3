#ifndef CFAST_BOUND_TREE_HPP
#define CFAST_BOUND_TREE_HPP

#include "Object.hpp"
#include "..//Parser.hpp"

namespace cf::rt {

struct BoundNode : Node {
	Method* method;

	BoundNode(TreePtr parent, Method* _method = nullptr) : Node(parent), method(_method) {}
};

std::ostream& operator<<(std::ostream& stream, const BoundNode& node) {
	if (node.method != nullptr) {
		return stream << node.method->name;
	}
	else return stream << "empty";
}

using BoundTree = Tree<BoundNode>;

} // namespace cf::rt

#endif // !CFAST_BOUND_TREE_HPP
