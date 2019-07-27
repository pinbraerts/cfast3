#ifndef CFAST_SYNTAX_TRAITS_HPP
#define CFAST_SYNTAX_TRAITS_HPP

#include <map>
#include <set>
#include <string_view>

#include "TokenTraits.hpp"

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

constexpr std::string_view ToString(SyntaxType t) {
    switch(t) {
    case SyntaxType::End: return "EOF";
    case SyntaxType::Space: return "Space";
    case SyntaxType::Line: return "Line";
    case SyntaxType::Operator: return "Operator";
    case SyntaxType::String: return "String";
    case SyntaxType::Quote: return "Quote";
    case SyntaxType::OpenBrace: return "OpenBrace";
    case SyntaxType::CloseBrace: return "CloseBrace";
    case SyntaxType::ContainerSpace: return "Container Space";
    case SyntaxType::ContainerString: return "Container String";
    case SyntaxType::ContainerOperator: return "Container Operator";
    case SyntaxType::ContainerQuote: return "Container Quote";
    case SyntaxType::ContainerBrace: return "Container Brace";
    default: return "Error!";
    }
}

template<class T>
class SyntaxTraits {
public:
    using char_type = typename T::char_type;
    using Index = typename T::Index;
    using Type = SyntaxType;
    using TokenType = typename T::Type;
    using Token = typename T::Token;
    using Priority = float;
    using View = typename T::View;
    
private:
    // https://en.cppreference.com/w/cpp/language/operator_precedence
    std::map<View, Priority> priority_map {
        { "::", 1 },
        { ".", 2 }, { "->", 2 }, { "--", 2 }, { "++", 2 },
        { "!", 3 }, { "~", 3 },
        { "*", 4 }, { "/", 4 }, { "%", 4 },
        { "+", 5 }, { "-", 5 },
        { "<<", 6 }, { ">>", 6 },
        { "<=>", 7 },
        { "<", 8 }, { "<=", 8 }, { ">", 8 }, { ">=", 8 },
        { "==", 9 }, { "!=", 9 },
        { "&", 10 },
        { "^", 11 },
        { "|", 12 },
        { "&&", 13 },
        { "||", 14 },
        { ",", 15 },
        { "=", 16 }, { "+=", 16 }, { "-=", 16 },
        { "*=", 16 }, { "/=", 16 }, { "%=", 16 },
        { ">>=", 16 }, { "<<=", 16 },
        { "&=", 16 }, { "|=", 16 }, { "^=", 16 },
        { ";", 17 },
        { "(", 0 }, { ")", 18 },
        { "[", 0 }, { "]", 18 },
        { "{", 0 }, { "}", 18 }
    };

    std::set<View> assignment_set {
        "=", "+=", "-=", "*=", "/=", "%=", ">>=", "<<=", "&=", "^=", "|="
    };

    std::set<View> unary_set {
        "++", "--", "!", "~"
    };

public:
    Priority GetPriority(View src) const {
        if (auto iter = priority_map.find(src); iter != priority_map.end())
            return iter->second;
        else return 0;
    }

    bool IsAssignment(View src) const {
        return assignment_set.find(src) != assignment_set.end();
    }

    bool IsUnary(View src) const {
        return unary_set.find(src) != unary_set.end();
    }
};

#endif // !CFAST_SYNTAX_TRAITS_HPP