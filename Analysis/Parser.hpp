#ifndef CFAST_PARSER_HPP
#define CFAST_PARSER_HPP

#include "Lexer.hpp"
#include "../Utils/ScopedNode.hpp"
#include "Syntax.hpp"
#include "SyntaxTraits.hpp"

namespace cfast {

template<class L,
    class R = SyntaxTraits<L>,
    class N = Syntax<R>,
    class T = Tree<N>>
class Parser {
public:
    using Lexer      = L;
    using char_type  = typename Lexer::char_type;

    using Syntax     = N;
    using Traits     = R;
    using token_type = typename Syntax::token_type;
    using TokenType  = typename token_type::Type;
    using Type       = typename Syntax::Type;
    using Priority   = typename Syntax::Priority;

    using Tree       = T;
    using Walker     = ScopedNode<Tree>;
    using pointer    = typename Walker::pointer;

public:
    Lexer& _lexer;
    Walker _walker;
    Traits _traits;
    
    pointer _spaces = pointer{};
    bool eat_lines = true;
    token_type _current { };
    string_view<char_type> _current_view { };
    Priority _current_priority { };
    std::basic_string<char_type> _current_error { };
    
    TokenType type() const {
        return _current.type;
    }
    
    void Next() {
        _current = _lexer.Next();
        _current_view = _lexer.buffer().span(_current);
    }
    
    void PushSpaces() {
        if(_spaces != pointer()) {
            _walker.Push(_spaces);
            _spaces = pointer();
        }
    }
    
    void PushCurrent() {
        _walker.CreatePush(_current);
    }
    
    void PushCurrentAndSpaces() {
        _walker.CreatePushSelect(_current, _current_priority);
        PushSpaces();
        _walker.GoUp();
    }
    
    void BubblePriority() {
        _current_priority = _traits.GetPriority(_current_view);
            
        // bubble up until lower or equal priority
        while(_current_priority > _walker->item.priority && _walker.TryGoUp())
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
        _spaces = _walker.CreateSelect(Type::ContainerSpace);
        for (Next(); type() != TokenType::End &&
            (
                type() == TokenType::Space ||
                (eat_lines && type() == TokenType::Line)
            ); Next()) {
            PushCurrent();
        }
        if(_walker->children.empty()) {
            try {
                _walker.tree().DeleteNode(_spaces);
            }
            catch (const std::runtime_error& e) {
                _current_error = e.what();
                return;
            }
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
        if (_current_priority != _walker->item.priority && !_walker->children.empty()) {    
            auto& ch = _walker->children;
            auto i = ch.end() - 1, b = ch.begin();

            while (b < i && (_walker.get(*i)->item.priority <= _current_priority)) --i;
            if (_walker.get(*i)->item.priority > _current_priority)
                ++i;
            
            auto moved = MoveItems(ch, i, ch.end());
            _walker.CreatePushSelect(Type::ContainerOperator, _current_priority);
            _walker->children = std::move(moved);
        }
        
        PushCurrentAndSpaces();
        
        eat_lines = true;
    }
    
    void ParseClosure() noexcept {
        string_view<char_type> v = _lexer.buffer().span(_walker[0]->item);
        if(!_traits.IsClosure(v, _current_view))
            return err(std::basic_string<char_type>(v) + " does not match " + std::basic_string<char_type>(_current_view));
        
        PushCurrentAndSpaces(); // closure
        
        _walker->item.priority = _traits.min_priority; // after closure container should have priority of value
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
                err(std::basic_string<char_type>("unexpected token type ") + ToString(type()));
            }
        }
        return _current_error;
    }
    
    void ParseOpening() noexcept {
        _walker.CreatePushSelect(Type::ContainerBrace, _traits.max_priority);
        _walker.CreatePush(_current, _traits.max_priority); // opening should have max priority in order not to be captured
    }
    
    void ParseQuote() noexcept {
        _walker.CreatePushSelect(Type::ContainerQuote);
        string_view<char_type> opening = _current_view;
        PushCurrentAndSpaces();

        // TODO add string literal features
        _walker.CreatePushSelect(Type::Quote);
        _walker->item.begin(_current.end());
        for (Next(); type() != TokenType::End &&
            (type() != TokenType::Quote || _current_view != opening); Next())
            if (_traits.IsEscape(_current_view)) {
                Next();
            }
        if (type() != TokenType::Quote)
            return err("quote is not closed");

        _walker->item.end(_current.begin()); // fill literal
        _walker.GoUp();

        PushCurrent(); // closure

        _walker.GoUp();
    }
    
    std::string Parse() noexcept {
        _walker.CreateSelect(); // root
        return ParseBody();
    }
};

} // namespace cfast

#endif // !CFAST_PARSER_HPP