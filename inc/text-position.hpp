#ifndef PIX_TEXT_POSITION_HPP
#define PIX_TEXT_POSITION_HPP

#include <string>
#include <iostream>

class TextPosition {
public:
    TextPosition(std::string const &fname = "", 
                 std::size_t line = 1, std::size_t col = 1);

    void advance(char c);

    void copy_from(TextPosition const &other);

    friend std::ostream &operator <<(std::ostream &stream, 
                                     TextPosition const &pos);

    std::string const &fname() const { return m_fname; }

    std::size_t line() const { return m_line; }

    std::size_t col() const { return m_col; }
private:
    std::string const &m_fname;

    std::size_t m_line;

    std::size_t m_col;
};

#endif
