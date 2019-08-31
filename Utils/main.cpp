#include "Buffer.hpp"
#include "ScopedNode.hpp"

void TestBuffer() {
    auto b = Buffer<char>::FromFile("Buffer.hpp");
    std::cout << b.data() << std::endl << std::endl;
    for (size_t i : b.lines())
        std::cout << i << ' ';
    std::cout << std::endl << std::endl;

    struct X {
        size_t a, b;

        size_t begin() {
            return a;
        }
        size_t end() {
            return b;
        }
    };

    auto str = b.span(X{ 1027, 1033 });
    auto desc = b.GetDescription(1027);
    std::cout << str << " at " << desc.line << " line at " << desc.position << " position" << std::endl;
}

void TestTree() {
    Tree<int> t;
    ScopedNode<decltype(t)> w(t), h(w);

    auto my_log = [&](std::string msg) {
        std::cout << std::setfill('=') << std::setw(10) << ' ' << msg << ' ' << std::setw(10) << ' ' << std::endl;
        for (auto& node : h) {
            std::cout << std::setfill(' ') << std::setw(node.depth()) << node << std::endl;
        }
        std::cout << std::endl;
    };

    w.CreateSelect(0); h = w;
    w.CreatePushSelect(1);
    w.CreatePush(2);
    w.CreatePush(3);
    w.GoUp();

    w.CreatePushSelect(4);
    w.CreatePush(5);
    w.CreatePushSelect(6);
    w.CreatePush(7);
    w.GoUp();

    my_log("Initialized tree");

    auto p = t.CreateNode(8);
    auto* n = t.get(p);
    auto& c = w->children;
    n->children = MoveItems(c, c.begin(), c.end());

    w.Push(p);

    my_log("Captured item");
}

int main() {
    TestBuffer();
    TestTree();
    return 0;
}
