#ifndef CFAST_PARSER_HPP
#define CFAST_PARSER_HPP

#include "Lexer.hpp"
#include "Tree.hpp"
#include "Syntax.hpp"
#include "SyntaxTraits.hpp"

template<class L,
    class R = SyntaxTraits<L>,
    class N = Syntax<R>,
    class T = Tree<N, PoolAllocator>>
class Parser {
public:
    using Lexer = L;
    using Tree = T;
    using Walker = TreeWalker<Tree>;
    using pointer = typename Walker::pointer;
    using Syntax = N;
    using SyntaxNode = typename Walker::Node;
    using Token = typename R::Token;
    using Type = typename Syntax::Type;
    using Traits = R;
    using View = typename Traits::View;
    using Priority = typename Syntax::Priority;

public:
    Lexer& _lexer;
    Walker _walker;
    pointer _spaces;
    Token _current;
    View _current_view;
    Traits _traits;
    bool eat_lines = true;
    
    Type type() const {
        return (Type)(size_t)_current.type();
    }
    
    void Next() {
        _current = _lexer.Next();
        _current_view = _lexer.view(_current);
    }
    
    void PushSpaces() {
        if(_spaces != pointer()) {
            _walker.PushChild(_spaces);
            _spaces = pointer();
        }
    }
    
    void PushCurrent() {
        _walker.EmplaceChild(_current);
    }
    
    void PushCurrentAndSpaces(Priority p) {
        _walker.EmplaceChildAndSelect(_current, p);
        PushSpaces();
        _walker.GoUp();
    }
    
    void PushCurrentAndSpaces() {
        _walker.EmplaceChildAndSelect(_current);
        PushSpaces();
        _walker.GoUp();
    }

public:
    Parser(
        Lexer& lexer,
        Tree& tree,
        Traits traits = Traits{}
    ) : _lexer(lexer),
        _walker(tree),
        _spaces(),
        _current(),
        _current_view(),
        _traits(traits) { }

    void EatSpaces() {
        _spaces = _walker.tree().CreateNode(Type::ContainerSpace);
        _walker.Select(_spaces);
        for (Next(); type() != Type::End &&
            (
                type() == Type::Space ||
                (eat_lines && type() == Type::Line)
            ); Next()) {
            PushCurrent();
        }
        if(_walker.current().children.empty()) {
            _walker.tree().DeleteNode(_spaces);
            _spaces = pointer();
        }
        _walker.GoUp();
        eat_lines = true;// TODO false;
    }
    
    void ParseString() {
        PushCurrentAndSpaces();
    }
    
    void ParseOperator() {
        Priority p = _traits.GetPriority(_current_view);
            
        // bubble up until lower or equal priority
        while(p > _walker.current().item.priority() && _walker.TryGoUp())
            ;
        if (p != _walker.current().item.priority() && !_walker.current().children.empty()) {
            // create node here because ch and i can be invalidated
            pointer ptr = _walker.tree().CreateNode(Type::ContainerOperator, p);
                     
            auto& ch = _walker.current().children;
            auto i = ch.end() - 1, b = ch.begin();

            while (b < i && (_walker.get(*i).item.priority() <= p)) --i;
            if (_walker.get(*i).item.priority() > p)
                ++i;

            _walker.get(ptr).children = MoveItems(ch, i, ch.end());
            _walker.PushChild(ptr);
            _walker.Select(ptr);
        }
                
        PushCurrentAndSpaces(p);
        
        eat_lines = true;
    }
    
    void ParseBody() {
        while (true) {
            EatSpaces();

            switch (type()) {
            case Type::End:
                _walker.GoToRoot();
                PushSpaces();
                //PushCurrentAndSpaces();
                throw 0;
            case Type::Operator:
                ParseOperator();
                break;
            case Type::CloseBrace:
                return;
            case Type::String:
                ParseString();
                break;
            case Type::Quote:
                ParseQuote();
                break;
            case Type::OpenBrace:
                ParseBraces();
                break;
            case Type::Line: // TODO
            default:
                throw std::runtime_error(std::string("unexpected token type ") + std::string(ToString(type())));
            }
        }
    }
    
    void ParseBraces() {
        pointer container = _walker.EmplaceChildAndSelect(Type::ContainerBrace);
        PushCurrentAndSpaces(); // opening
        
        //Next();
        ParseBody();
        
        if(type() != Type::CloseBrace)
            throw std::runtime_error("No close brace");
        
        // return to where it started
        while(_walker.current_pointer() != container && _walker.TryGoUp());
        
        if(_walker.current_pointer() != container)
            throw std::runtime_error("Cannot return back");
        
        PushCurrentAndSpaces(); // closure
        _walker.current().item.priority(0); // after closure container should have priority of value
        _walker.GoUp();
    }
    
    void ParseQuote() {
        _walker.EmplaceChildAndSelect(Type::ContainerQuote);
        std::string_view opening = _current_view;
        PushCurrentAndSpaces();

        // TODO add string literal features
        _walker.EmplaceChildAndSelect(Type::String); // fill string literal
        _walker.current().item.begin(_current.end());
        for (Next(); type() != Type::End &&
            (type() != Type::Quote || _current_view != opening); Next())
            if (type() == Type::Operator && _current_view[0] == '\\')
                Next();
        if (type() != Type::Quote)
            throw std::runtime_error("quote is not closed");

        _walker.current().item.end(_current.begin());
        _walker.GoUp();

        PushCurrent(); // closure

        _walker.GoUp();
    }
    
    std::string Parse() {
        try {
            _walker.EmplaceChildAndSelect(); // root
            ParseBody();
        }
        catch(const std::runtime_error& e) {
            return e.what();
        }
        catch(int) { }
        return std::string();
    }
};

void TestParser() {
    auto b = Buffer<char>::FromFile("Parser.hpp");
    Lexer<char> l(b);
    Parser<decltype(l)>::Tree t;
    Parser<decltype(l)> p(l, t);
    auto res = p.Parse();
    if(!res.empty()) {
        std::cerr << res << std::endl;
        return;
    }
    
    auto print = [&](std::ostream& s, const typename decltype(t)::Item& x) {
        auto w = std::string_view(b.get(x.begin()), x.size());
        s << ToString(x.type()) << ' ' << x.priority();
        if(!w.empty())
            s << ' ' << '\'' << w << '\'';
    };
    TreePrinter<decltype(t), decltype(print)>(t, std::cout, print).print();
}

#endif // !CFAST_PARSER_HPP
