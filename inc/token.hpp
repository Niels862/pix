#ifndef PIX_TOKEN_HPP
#define PIX_TOKEN_HPP

#include "textposition.hpp"
#include <string>
#include <iostream>

enum class TokenKind {
    None,
    Identifier,
    EndOfFile
};

std::string const &to_string(TokenKind kind);

std::ostream &operator <<(std::ostream &stream, TokenKind kind);

class Token {
public:
    Token(TextPosition const &pos = TextPosition(), 
          TokenKind kind = TokenKind::None, 
          std::string const &&lexeme = "");

    friend std::ostream &operator <<(std::ostream &stream, Token const &token);

    TextPosition const &pos() const { return m_pos; }

    TokenKind kind() { return m_kind; }

    std::string const &lexeme() { return m_lexeme; }
private:
    TextPosition m_pos;

    TokenKind m_kind;
    
    std::string const m_lexeme;
};

#endif
