#ifndef PIX_LEXER_HPP
#define PIX_LEXER_HPP

#include "token.hpp"
#include "text-position.hpp"
#include <string>
#include <vector>
#include <stdexcept>

class LexerError : public std::exception {
public:
    LexerError(TextPosition const &pos, std::string const &msg);

    char const *what() const noexcept override {
        return m_msg.c_str();
    }
private:
    std::string m_msg;
};

class Lexer {
public:
    Lexer(std::string const &fname);

    std::vector<Token> lex();
private:
    static std::string read_file(std::string const &fname);

    bool at_eof() const;

    bool is_id_start() const;

    bool is_id_continue() const;

    bool is_digit() const;

    bool is_whitespace() const;

    bool is_comment() const;

    bool is_linebreak() const;

    TokenKind separator_kind() const;

    void lex_identifier();

    void lex_integer();

    void set_base();

    void advance();

    char curr() const;

    std::string lexeme() const;

    std::string const &m_fname;

    std::string m_text;

    std::vector<Token> m_tokens;

    std::size_t m_base_offset;

    std::size_t m_curr_offset;

    TextPosition m_base_pos;

    TextPosition m_curr_pos;
};

#endif
