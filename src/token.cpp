#include "token.hpp"
#include "error.hpp"
#include "utils.hpp"
#include <unordered_map>
#include <sstream>

std::unordered_map<TokenKind, std::string> const tokenkind_to_string_map = {
    { TokenKind::None, "<none>" },
    { TokenKind::Identifier, "identifier" },
    { TokenKind::Integer, "integer" },
    { TokenKind::Function, "function" },
    { TokenKind::Return, "return" },
    { TokenKind::True, "True" },
    { TokenKind::False, "False" },
    { TokenKind::Null, "Null" },
    { TokenKind::Arrow, "->" },
    { TokenKind::ParenLeft, "(" },
    { TokenKind::ParenRight, ")" },
    { TokenKind::BraceLeft, "{" },
    { TokenKind::BraceRight, "}" },
    { TokenKind::BracketLeft, "[" },
    { TokenKind::BraceRight, "]" },
    { TokenKind::Comma, "," },
    { TokenKind::Semicolon, ";" },
    { TokenKind::Colon, ":" },
    { TokenKind::Synthetic, "<synthetic>" },
    { TokenKind::EndOfFile, "end of file" }
};

std::unordered_map<std::string, TokenKind> const string_to_tokenkind_map 
        = inverse(tokenkind_to_string_map);

std::string const &to_string(TokenKind kind) {
    auto const &it = tokenkind_to_string_map.find(kind);

    if (it == tokenkind_to_string_map.end()) {
        std::stringstream ss;
        ss << "Unmapped tokenkind: " << static_cast<int>(kind);
        throw FatalError(ss.str());
    }

    return it->second;
}

TokenKind from_string(std::string const &str) {
    auto const &it = string_to_tokenkind_map.find(str);

    if (it == string_to_tokenkind_map.end()) {
        return TokenKind::None;
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

Token const &Token::None() {
    static Token const none = Token();
    return none;
}

Token::operator bool() const {
    return m_kind != TokenKind::None;
}

std::ostream &operator <<(std::ostream &stream, Token const &token) {
    if (token.m_lexeme.empty()) {
        stream << token.m_pos << ": " << token.m_kind;
    } else {
        stream << token.m_pos << ": " << token.m_kind << ": " << token.m_lexeme;
    }
    return stream;
}
