#include "textposition.hpp"
#include "error.hpp"

TextPosition::TextPosition(std::string const &fname, 
                           std::size_t line, std::size_t col)
        : m_fname{fname}, m_line{line}, m_col{col} {}

void TextPosition::advance(char c) {
    if (c == '\n') {
        m_line++;
        m_col = 1;
    } else {
        m_col++;
    }
}

void TextPosition::copy_from(TextPosition const &other) {
    if (m_fname != other.m_fname) {
        throw FatalError("copy_from() called on differing file names");
    }

    m_line = other.m_line;
    m_col = other.m_col;
}

std::ostream &operator <<(std::ostream &stream, TextPosition const &pos) {
    stream << pos.m_fname << ":" << pos.m_line << ":" << pos.m_col;
    return stream;
}
