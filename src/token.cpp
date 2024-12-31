#include "token.hpp"
#include <unordered_map>
#include <stdexcept>
#include <sstream>

std::string const &to_string(TokenKind kind) {
    static std::unordered_map<TokenKind, std::string> const map = {
        { TokenKind::EndOfFile, "end of file" }
    };

    auto const &it = map.find(kind);
    if (it == map.end()) {
        std::stringstream ss;
        ss << "Unmapped tokenkind: " << static_cast<int>(kind);
        throw std::runtime_error(ss.str());
    }

    return it->second;
}

std::ostream &operator <<(std::ostream &stream, TokenKind kind) {
    stream << to_string(kind);
    return stream;
}

Token::Token(TextPosition const &pos, TokenKind kind, 
             std::string const &&lexeme)
        : m_pos{pos}, m_kind{kind}, m_lexeme{std::move(lexeme)} {}

std::ostream &operator <<(std::ostream &stream, Token const &token) {
    stream << token.m_pos << ": " << token.m_kind;
    return stream;
}
