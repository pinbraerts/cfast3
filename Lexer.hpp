#include "Operator.hpp"

struct Lexer {
    mutable bool new_line = false;

    void next(std::istream& stream, Operator& o) const {
        o.value.clear();
        o = prec('x');

        if (new_line) {
            size_t i;
            for (i = 0; cfast_isspace((char)stream.peek()); ++i) stream.get();

            if (i != 0) {
                o.value = std::string(i, ' ');
                o.precedence = prec('\n').precedence;
                return;
            }
        } else while (cfast_isspace((char)stream.peek()))
            stream.get();

        op_t c = (op_t)stream.peek();

        if (c == '#') {
            std::getline(stream, o.value);
            new_line = true;
            return;
        }
        else if (c == '$') {
            stream.get();
            o.value = "$";
            new_line = false;
            return;
        }

        if (prec(c).type == Expr::Value) {
            while (prec(c).type == Expr::Value && !cfast_isspace((char)c)) {
                o.value += (char)stream.get();
                c = (op_t)stream.peek();
            }
            new_line = false;
        }
        else if (c == '\n') {
            while (stream.peek() == '\n') stream.get();
            o = Operator(c);
            new_line = true;
        }
        else {
            while (prec(c).type != Expr::Value) {
                Operator temp(c);
                if ((char)c == EOF) {
                    stream.setstate(std::ios::eofbit);
                    break;
                }
                o = std::move(temp);
                stream.get();
                c = (c << 8) | (char)stream.peek();
            }
            new_line = false;
        }
    }
};

struct local_lexer : Lexer, std::locale::facet {
    static std::locale::id id;

    using Lexer::Lexer;
};
std::locale::id local_lexer::id;

std::istream& operator>>(std::istream& stream, Operator& o) {
    const Lexer& lex = std::use_facet<local_lexer>(stream.getloc());
    lex.next(stream, o);
    return stream;
}
