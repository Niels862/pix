#ifndef PIX_TOKEN_HPP
#define PIX_TOKEN_HPP

#include "text-position.hpp"
#include <string>
#include <iostream>

enum class TokenKind {
    None,

    Identifier,
    Integer,
    
    Function,
    Return,
    True,
    False,
    Null,

    Arrow,

    ParenLeft,
    ParenRight,
    BraceLeft,
    BraceRight,
    BracketLeft,
    BracketRight,
    Comma,
    Semicolon,
    Colon,

    Synthetic,
    
    EndOfFile
};

std::string const &to_string(TokenKind kind);

TokenKind from_string(std::string const &str);

std::ostream &operator <<(std::ostream &stream, TokenKind kind);

class Token {
public:
    Token(TextPosition const &pos = TextPosition(), 
          TokenKind kind = TokenKind::None, 
          std::string const &&lexeme = "");

    static Token const &None();

    friend std::ostream &operator <<(std::ostream &stream, Token const &token);

    operator bool() const;

    TextPosition const &pos() const { return m_pos; }

    TokenKind kind() const { return m_kind; }

    std::string const &lexeme() const 
            { return m_lexeme.empty() ? to_string(m_kind) : m_lexeme; }
private:
    TextPosition m_pos;

    TokenKind m_kind;
    
    std::string const m_lexeme;
};

#endif
