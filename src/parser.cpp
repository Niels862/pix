#include "parser.hpp"
#include <sstream>

ParserError::ParserError(TextPosition const &pos, std::string const &msg) {
    std::stringstream ss;
    ss << pos << ": " << msg;
    m_msg = ss.str();
}

Parser::Parser(std::vector<Token> tokens)
        : m_tokens{tokens}, m_curr_idx{} {}

Program::ptr Parser::parse() {
    std::vector<Statement::ptr> stmts;

    while (!accept(TokenKind::EndOfFile)) {
        Statement::ptr stmt = parse_statement();
        stmts.push_back(std::move(stmt));
    }

    Program::ptr program = std::make_unique<Program>(std::move(stmts));
    return program;
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
    if (matches(TokenKind::Function)) {
        return parse_function_declaration();
    }

    return parse_expression_statement();
}

Statement::ptr Parser::parse_function_declaration() {
    expect(TokenKind::Function);

    Token func = expect(TokenKind::Identifier);

    // temp
    expect(TokenKind::ParenLeft);
    expect(TokenKind::ParenRight);

    std::vector<Statement::ptr> stmts = parse_body();

    return std::make_unique<FunctionDeclaration>(func, std::move(stmts));
}

std::vector<Statement::ptr> Parser::parse_body() {
    expect(TokenKind::BraceLeft);

    std::vector<Statement::ptr> stmts;

    while (!accept(TokenKind::BraceRight)) {
        stmts.push_back(parse_statement());

        if (matches(TokenKind::EndOfFile)) {
            expect(TokenKind::BraceRight);
        }
    }

    return stmts;
}

Statement::ptr Parser::parse_expression_statement() {
    Statement::ptr stmt = 
            std::make_unique<ExpressionStatement>(parse_expression());

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
        if (matches(TokenKind::ParenLeft)) {
            return std::make_unique<Call>(token, parse_call_args());
        }

        return std::make_unique<Variable>(token);
    }

    std::stringstream ss;
    ss << "Expected value, but got " << token.kind();
    throw ParserError(token.pos(), ss.str());
}

std::vector<Expression::ptr> Parser::parse_call_args() {
    expect(TokenKind::ParenLeft);

    if (accept(TokenKind::ParenRight)) {
        return {};
    }

    std::vector<Expression::ptr> args;

    while (true) {
        Expression::ptr arg = parse_expression();
        args.push_back(std::move(arg));

        if (!accept(TokenKind::Comma)) {
            expect(TokenKind::ParenRight);
            return args;
        }
    }
}
