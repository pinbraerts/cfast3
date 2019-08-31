#ifndef CFAST_SYNTAX_TRAITS_HPP
#define CFAST_SYNTAX_TRAITS_HPP

#include <map>
#include <set>

#include "TokenTraits.hpp"

namespace cfast {

enum class SyntaxType {
    End,
    Space,
    Line,
    Operator,
    String,
    Quote,
    OpenBrace,
    CloseBrace,
    ContainerSpace,
    ContainerString,
    ContainerOperator,
    ContainerQuote,
    ContainerBrace,
};

constexpr const char* ToString(SyntaxType t) {
    switch(t) {
    case SyntaxType::End:               return "EOF";
    case SyntaxType::Space:             return "Space";
    case SyntaxType::Line:              return "Line";
    case SyntaxType::Operator:          return "Operator";
    case SyntaxType::String:            return "String";
    case SyntaxType::Quote:             return "Quote";
    case SyntaxType::OpenBrace:         return "OpenBrace";
    case SyntaxType::CloseBrace:        return "CloseBrace";
    case SyntaxType::ContainerSpace:    return "Container Space";
    case SyntaxType::ContainerString:   return "Container String";
    case SyntaxType::ContainerOperator: return "Container Operator";
    case SyntaxType::ContainerQuote:    return "Container Quote";
    case SyntaxType::ContainerBrace:    return "Container Brace";
    default:                            return "Error!";
    }
}

template<class T>
class SyntaxTraits {
public:
    using char_type  = typename T::char_type;
    using Type       = SyntaxType;
    using TokenType  = typename T::Type;
    using Token_t    = typename T::Token_t;
    using Priority   = float;
    
private:
    // https://en.cppreference.com/w/cpp/language/operator_precedence
    std::map<string_view<char_type>, Priority> priority_map {
        { "::", 1.0f },
        { ".", 2.0f }, { "->", 2.0f }, { "--", 2.0f }, { "++", 2.0f },
        { "!", 3.0f }, { "~", 3.0f },
        { "*", 4.0f }, { "/", 4.0f }, { "%", 4.0f },
        { "+", 5.0f }, { "-", 5.0f },
        { "<<", 6.0f }, { ">>", 6.0f },
        { "<=>", 7.0f },
        { "<", 8.0f }, { "<=", 8.0f }, { ">", 8.0f }, { ">=", 8.0f },
        { "==", 9.0f }, { "!=", 9.0f },
        { "&", 10.0f },
        { "^", 11.0f },
        { "|", 12.0f },
        { "&&", 13.0f },
        { "||", 14.0f },
        { ",", 15.0f },
        { "=", 16.0f }, { "+=", 16.0f }, { "-=", 16.0f },
        { "*=", 16.0f }, { "/=", 16.0f }, { "%=", 16.0f },
        { ">>=", 16.0f }, { "<<=", 16.0f },
        { "&=", 16.0f }, { "|=", 16.0f }, { "^=", 16.0f },
        { ";", 17.0f },
        { "(", 0.0f }, { ")", 18.0f },
        { "[", 0.0f }, { "]", 18.0f },
        { "{", 0.0f }, { "}", 18.0f }
    };

public:
    static constexpr Priority max_priority = 18, min_priority = 0;

    Priority GetPriority(string_view<char_type> src) const {
        auto iter = priority_map.find(src);
        if (iter != priority_map.end())
            return iter->second;
        else return 0;
    }

    static constexpr bool IsEscape(string_view<char_type> v) {
        return !v.empty() && v[0] == '\\';
    }
    
    static constexpr bool IsClosure(string_view<char_type> v1, string_view<char_type> v2) {
        if(v1.empty() || v2.empty())
            return false;
        
        switch(v1[0]) {
        case '(': return v2[0] == ')';
        case '{': return v2[0] == '}';
        case '[': return v2[0] == ']';
        default: return false;
        }
    }
};

} // namespace cfast

#endif // !CFAST_SYNTAX_TRAITS_HPP