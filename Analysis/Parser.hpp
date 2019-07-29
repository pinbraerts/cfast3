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
    using TokenType = typename Token::Type;
    using Type = typename Syntax::Type;
    using Traits = R;
    using View = typename Traits::View;
    using Priority = typename Syntax::Priority;

public:
    Lexer& _lexer;
    Walker _walker;
    Traits _traits;
    
    pointer _spaces = pointer{};
    bool eat_lines = true;
    Token _current = Token{};
    View _current_view = View{};
    Priority _current_priority = Priority{};
    std::string _current_error = std::string{};
    
    TokenType type() const {
        return _current.type();
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
    
    void PushCurrentAndSpaces() {
        _walker.EmplaceChildAndSelect(_current, _current_priority);
        PushSpaces();
        _walker.GoUp();
    }
    
    void BubblePriority() {
        _current_priority = _traits.GetPriority(_current_view);
            
        // bubble up until lower or equal priority
        while(_current_priority > _walker.current().item.priority && _walker.TryGoUp())
            ;
    }

public:
    Parser(
        Lexer& lexer,
        Tree& tree,
        Traits traits = Traits{}
    ) : _lexer(lexer),
        _walker(tree),
        _traits(traits) { }
    
    void err(std::string msg) noexcept {
        _current_error = msg;
    }

    void EatSpaces() noexcept {
        _spaces = _walker.tree().CreateNode(Type::ContainerSpace);
        _walker.Select(_spaces);
        for (Next(); type() != TokenType::End &&
            (
                type() == TokenType::Space ||
                (eat_lines && type() == TokenType::Line)
            ); Next()) {
            PushCurrent();
        }
        if(_walker.current().children.empty()) {
            _walker.tree().DeleteNode(_spaces);
            _spaces = pointer();
        }
        _walker.GoUp();
        eat_lines = true; // TODO false;
    }
    
    void ParseString() noexcept {
        // TODO add difference between Number literals and String, some features
        PushCurrentAndSpaces();
    }
    
    void ParseOperator() noexcept {
        if (_current_priority != _walker.current().item.priority && !_walker.current().children.empty()) {    
            auto& ch = _walker.current().children;
            auto i = ch.end() - 1, b = ch.begin();

            while (b < i && (_walker.get(*i).item.priority <= _current_priority)) --i;
            if (_walker.get(*i).item.priority > _current_priority)
                ++i;
            
            auto moved = MoveItems(ch, i, ch.end());
            _walker.EmplaceChildAndSelect(Type::ContainerOperator, _current_priority);
            _walker.current().children = std::move(moved);
        }
                
        PushCurrentAndSpaces();
        
        eat_lines = true;
    }
    
    void ParseClosure() noexcept {
        View v = _lexer.view(_walker.get(_walker.current().children[0]).item);
        if(!_traits.IsClosure(v, _current_view))
            return err(std::string(v) + " does not match " + std::string(_current_view));
        
        PushCurrentAndSpaces(); // closure
        
        _walker.current().item.priority = _traits.min_priority; // after closure container should have priority of value
        _walker.GoUp();
    }
    
    std::string ParseBody() noexcept {
        while (_current_error.empty()) {
            EatSpaces();
            BubblePriority();

            switch (type()) {
            case TokenType::End:
                _walker.GoToRoot();
                PushSpaces();
                return std::string();
            case TokenType::Operator:
                ParseOperator();
                break;
            case TokenType::OpenBrace:
                ParseOpening();
                break;
            case TokenType::CloseBrace:
                ParseClosure();
                break;
            case TokenType::String:
                ParseString();
                break;
            case TokenType::Quote:
                ParseQuote();
                break;
            case TokenType::Line: // TODO
            default:
                err(std::string("unexpected token type ") + std::string(ToString(type())));
            }
        }
        return _current_error;
    }
    
    void ParseOpening() noexcept {
        _walker.EmplaceChildAndSelect(
            Type::ContainerBrace,
            _traits.max_priority
        );
        _walker.EmplaceChildAndSelect(
            _current,
            _traits.max_priority
        ); // opening should have max priority in order not to be captured
        _walker.GoUp();
    }
    
    void ParseQuote() noexcept {
        _walker.EmplaceChildAndSelect(Type::ContainerQuote);
        std::string_view opening = _current_view;
        PushCurrentAndSpaces();

        // TODO add string literal features
        _walker.EmplaceChildAndSelect(Type::Quote);
        _walker.current().item.begin(_current.end());
        for (Next(); type() != TokenType::End &&
            (type() != TokenType::Quote || _current_view != opening); Next())
            if (_traits.IsEscape(_current_view)) {
                Next();
            }
        if (type() != TokenType::Quote)
            return err("quote is not closed");

        _walker.current().item.end(_current.begin()); // fill literal
        _walker.GoUp();

        PushCurrent(); // closure

        _walker.GoUp();
    }
    
    std::string Parse() noexcept {
        _walker.EmplaceChildAndSelect(); // root
        return ParseBody();
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
        s << ToString(x.type()) << ' ' << x.priority;
        if(!w.empty())
            s << ' ' << '\'' << w << '\'';
    };
    TreePrinter<decltype(t), decltype(print)>(t, std::cout, print).print();
}

#endif // !CFAST_PARSER_HPP