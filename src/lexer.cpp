#include "lexer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Lexer::Lexer(std::string const &fname) 
        : m_fname{fname}, m_text{}, m_tokens{}, 
          m_base_pos{m_fname}, m_curr_pos{m_fname} {}

std::vector<Token> Lexer::lex() {
    m_text = Lexer::read_file(m_fname);
    m_tokens.clear();

    m_tokens.emplace_back(m_base_pos, TokenKind::EndOfFile);

    return m_tokens;
}

std::string Lexer::read_file(std::string const &fname) {
    std::ifstream file(fname);

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}
