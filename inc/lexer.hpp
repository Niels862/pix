#ifndef PIX_LEXER_HPP
#define PIX_LEXER_HPP

#include "token.hpp"
#include "textposition.hpp"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(std::string const &fname);

    std::vector<Token> lex();
private:
    static std::string read_file(std::string const &fname);

    std::string const &m_fname;

    std::string m_text;

    std::vector<Token> m_tokens;

    TextPosition m_base_pos;

    TextPosition m_curr_pos;
};

#endif
