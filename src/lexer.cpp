#include "lexer.hpp"
#include "error.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

LexerError::LexerError(TextPosition const &pos, std::string const &msg) {
    std::stringstream ss;
    ss << pos << ": " << msg;
    m_msg = ss.str();
}

Lexer::Lexer(std::string const &fname) 
        : m_fname{fname}, m_text{}, m_tokens{}, 
          m_base_offset{}, m_curr_offset{}, 
          m_base_pos{m_fname}, m_curr_pos{m_fname} {}

std::vector<Token> Lexer::lex() {
    m_text = Lexer::read_file(m_fname);
    m_curr_offset = 0;
    m_tokens.clear();

    TokenKind kind;
    while (m_curr_offset < m_text.length()) {
        set_base();
        
        if (is_id_start()) {
            lex_identifier();
        } else if (is_digit()) {
            lex_integer();
        } else if (is_operator()) {
            lex_operator();
        } else if ((kind = separator_kind()) != TokenKind::None) {
            m_tokens.emplace_back(m_base_pos, kind);
            advance();
        } else if (is_whitespace()) {
            do {
                advance();
            } while (is_whitespace());
        } else if (is_comment()) {
            do {
                advance();
            } while (!is_linebreak() && !at_eof());
        } else {
            std::stringstream ss;
            ss << "Unrecognized character: " << std::string(1, curr());
            throw LexerError(m_curr_pos, ss.str());
        }
    }

    set_base();
    m_tokens.emplace_back(m_base_pos, TokenKind::EndOfFile);

    return m_tokens;
}

std::string Lexer::read_file(std::string const &fname) {
    std::ifstream file(fname);

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

bool Lexer::at_eof() const {
    if (m_curr_offset > m_text.length()) {
        throw FatalError("at_eof(): unexpected offset");
    }

    return m_curr_offset == m_text.length();
}

bool Lexer::is_id_start() const {
    return std::isalpha(curr()) || curr() == '_';
}

bool Lexer::is_id_continue() const {
    return std::isalnum(curr()) || curr() == '_';
}

bool Lexer::is_digit() const {
    return std::isdigit(curr());
}

bool Lexer::is_operator() const {
    char c = curr();

    return c == '+' || c == '-' 
        || c == '*' || c == '/'
        || c == '%' || c == '^'
        || c == '<' || c == '>'
        || c == '=' || c == '!'
        || c == '&' || c == '|'
        || c == '?' || c == '~'
        || c == '.';
}

bool Lexer::is_whitespace() const {
    static std::string const whitespace = " \n\r\t ";
    return whitespace.find(curr()) != std::string::npos; 
}

bool Lexer::is_comment() const {
    return curr() == '#';
}

bool Lexer::is_linebreak() const {
    return curr() == '\n';
}

TokenKind Lexer::separator_kind() const {
    return from_string(std::string(1, curr()));
}

void Lexer::lex_identifier() {
    do {
        advance();
    } while (is_id_continue());

    TokenKind keyword = from_string(lexeme());
    if (keyword == TokenKind::None) {
        m_tokens.emplace_back(m_base_pos, TokenKind::Identifier, lexeme());
    } else {
        m_tokens.emplace_back(m_base_pos, keyword);
    }
}

void Lexer::lex_integer() {
    do {
        advance();
    } while (is_digit());

    m_tokens.emplace_back(m_base_pos, TokenKind::Integer, lexeme());
}

void Lexer::lex_operator() {
    do {
        advance();
    } while (is_operator());

    TokenKind op = from_string(lexeme());
    if (op != TokenKind::None) {
        m_tokens.emplace_back(m_base_pos, op);
    } else {
        std::stringstream ss;
        ss << "Unrecognized operator: `" << lexeme() << "`";
        throw LexerError(m_curr_pos, ss.str());
    }
}

void Lexer::set_base() {
    m_base_offset = m_curr_offset;
    m_base_pos.copy_from(m_curr_pos);
}

void Lexer::advance() {
    if (at_eof()) {          
        throw FatalError("advance() called at end of file");
    }

    char c = curr();
    m_curr_offset++;
    m_curr_pos.advance(c);
}

char Lexer::curr() const {
    return m_text[m_curr_offset];
}

std::string Lexer::lexeme() const {
    return m_text.substr(m_base_offset, m_curr_offset - m_base_offset);
}
