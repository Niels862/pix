#include "parser.hpp"
#include <sstream>

ParserError::ParserError(TextPosition const &pos, std::string const &msg) {
    std::stringstream ss;
    ss << pos << ": " << msg;
    m_msg = ss.str();
}

Parser::Parser(std::vector<Token> tokens)
        : m_tokens{tokens}, m_curr_idx{} {}

Node::ptr Parser::parse() {
    Node::ptr node = parse_statement();

    expect(TokenKind::EndOfFile);

    return node;
}

void Parser::advance() {
    if (curr().kind() != TokenKind::EndOfFile) {
        m_curr_idx++;
    }
}

Token const &Parser::matches(TokenKind kind) const {
    return curr().kind() == kind ? curr() : Token::None();
}

Token const &Parser::accept(TokenKind kind) {    
    Token const &token = curr();
    if (token.kind() == kind) {
        advance();
        return token;
    }

    return Token::None();
}

Token const &Parser::expect(TokenKind kind) {    
    Token const &token = curr();
    if (token.kind() == kind) {
        advance();
        return token;
    }

    std::stringstream ss;
    ss << "Expected " << kind << ", but got " << token.kind();
    throw ParserError(token.pos(), ss.str());
}

Token const &Parser::curr() const {
    return m_tokens[m_curr_idx];
}

Token const &Parser::peek(std::size_t offset) const {
    for (std::size_t i = 0; i < offset; i++) {
        if (m_tokens[m_curr_idx + i].kind() == TokenKind::EndOfFile) {
            return m_tokens[m_curr_idx + i];
        }
    }

    return m_tokens[m_curr_idx + offset];
}

Statement::ptr Parser::parse_statement() {
    Expression::ptr expr = parse_expression();
    Statement::ptr stmt = 
            std::make_unique<ExpressionStatement>(std::move(expr));
    expect(TokenKind::Semicolon);
    return stmt;
}

Expression::ptr Parser::parse_expression() {
    return parse_value();
}

Expression::ptr Parser::parse_value() {
    Expression::ptr expr = parse_atom();

    return expr;
}

Expression::ptr Parser::parse_atom() {
    Token const &token = curr();

    if (accept(TokenKind::Integer)) {
        return std::make_unique<Integer>(token);
    }

    if (accept(TokenKind::Identifier)) {
        if (accept(TokenKind::ParenLeft)) {
            std::vector<Expression::ptr> args;
            Expression::ptr arg = parse_expression();
            args.push_back(std::move(arg));
            expect(TokenKind::ParenRight);

            return std::make_unique<Call>(token, std::move(args));
        }

        return std::make_unique<Variable>(token);
    }

    std::stringstream ss;
    ss << "Expected value, but got " << token.kind();
    throw ParserError(token.pos(), ss.str());
}