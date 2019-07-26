#ifndef CFAST_LEXER_TRAITS_HPP
#define CFAST_LEXER_TRAITS_HPP

#include <set>
#include <string_view>

enum class MatchResult {
    Nothing,
    Start,
    Combination
};

enum class TokenType {
    End,
    Space,
    Line,
    Operator,
    String,
    Quote,
    OpenBrace,
    CloseBrace,
};

constexpr std::string_view ToString(TokenType t) {
    switch(t) {
    case TokenType::End: return "EOF";
    case TokenType::Space: return "Space";
    case TokenType::Line: return "Line";
    case TokenType::Operator: return "Operator";
    case TokenType::String: return "String";
    case TokenType::Quote: return "Quote";
    case TokenType::OpenBrace: return "OpenBrace";
    case TokenType::CloseBrace: return "CloseBrace";
    default: return "Error!";
    }
}

template<class B>
struct TokenTraits {
    using char_type = typename B::char_type;
    using Index = typename B::Index;
    using Type = TokenType;
    
    Type GetType(char_type chr) noexcept {
        switch (chr) {
        case ' ': case '\t':
            return Type::Space;

        case '+': case '-':
        case '*': case '/': case '%':
        case '<': case '>':
        case '&': case '|':
        case '~': case '^':
        case '!': case '=':
        case ':':
            return Type::Operator;

        case '.': case ',':
        case ';':
        case '@': case '$':
        case '#': case '?':
        case '\\':
            return Type::Operator;

        case '\'': case '"': case '`':
            return Type::Quote;

        case '(': case '{': case '[':
            return Type::OpenBrace;

        case ')': case '}': case ']':
            return Type::CloseBrace;

        case '\n': case '\r':
            return Type::Line;

        case '\0': return Type::End;
        default: return Type::String;
        }
   }

    std::set<std::string_view> possible_combinations {
        "::", "->",
        "--", "++",
        "<<", ">>",
        "<=>", "<=", ">=", "==", "!=",
        "&&", "||",
        "+=", "-=", "*=", "/=", "%=", ">>=", "<<=", "&=", "|=", "^="
    };

    MatchResult Match(Type t, std::string_view s) {
        if (t == Type::String)
            return MatchResult::Combination;
        if (t == Type::Space)
            return MatchResult::Combination;
        auto iter = possible_combinations.lower_bound(s);
        if (iter == possible_combinations.end())
            return MatchResult::Nothing;
        std::string_view s2 (*iter);
        if (s2.find_first_of(s) == 0) {
            return s2.size() == s.size() ? MatchResult::Combination : MatchResult::Start;
        }
        return MatchResult::Nothing;
    }
};

#endif // !CFAST_LEXER_TRAITS_HPP