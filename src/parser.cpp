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
    ss << "Expected `" << kind << "`, but got `" << token.kind() << "`";
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
    switch (curr().kind()) {
        case TokenKind::Function:
            return parse_function_declaration();

        case TokenKind::Return:
            return parse_return_statement();

        case TokenKind::If:
            return parse_if_else_statement();

        case TokenKind::While:
            return parse_while_statement();

        case TokenKind::BraceLeft:
            return parse_scoped_body();

        case TokenKind::Break:
            return parse_break_statement();

        case TokenKind::Continue:
            return parse_continue_statement();

        case TokenKind::Identifier:
            if (peek(1).kind() == TokenKind::Colon) {
                return parse_variable_declaration();
            }
            return parse_expression_statement();

        default:
            return parse_expression_statement();
    }
}

FunctionDeclaration::ptr Parser::parse_function_declaration() {
    expect(TokenKind::Function);

    Token func = expect(TokenKind::Identifier);

    std::vector<ParameterDeclaration::ptr> params = parse_function_parameters();

    TypeAnnotation::ptr ret_type_annotation;
    if (accept(TokenKind::Arrow)) {
        ret_type_annotation = parse_type_annotation();
    } else {
        Token token(curr().pos(), TokenKind::Identifier, "void");
        ret_type_annotation = std::make_unique<NamedTypeAnnotation>(token);
    }
    
    std::vector<Statement::ptr> stmts = parse_body();

    return std::make_unique<FunctionDeclaration>(func, std::move(params), 
                                                 std::move(ret_type_annotation),
                                                 std::move(stmts));
}

std::vector<ParameterDeclaration::ptr> Parser::parse_function_parameters() {
    expect(TokenKind::ParenLeft);

    if (accept(TokenKind::ParenRight)) {
        return {};
    }

    std::vector<ParameterDeclaration::ptr> params;

    while (true) {
        ParameterDeclaration::ptr param = parse_parameter_declaration();
        params.push_back(std::move(param));

        if (!accept(TokenKind::Comma)) {
            expect(TokenKind::ParenRight);
            return params;
        }
    }
}

ParameterDeclaration::ptr Parser::parse_parameter_declaration() {
    Token ident = expect(TokenKind::Identifier);

    expect(TokenKind::Colon);
    TypeAnnotation::ptr type = parse_type_annotation();
    
    return std::make_unique<ParameterDeclaration>(ident, std::move(type));
}

VariableDeclaration::ptr Parser::parse_variable_declaration() {
    Token ident = expect(TokenKind::Identifier);

    expect(TokenKind::Colon);
    TypeAnnotation::ptr annotation = parse_type_annotation();

    expect(TokenKind::Equals);
    Expression::ptr value = parse_expression();
    expect(TokenKind::Semicolon);

    return std::make_unique<VariableDeclaration>(ident, std::move(annotation), 
                                                 std::move(value));
}

TypeAnnotation::ptr Parser::parse_type_annotation() {
    Token ident = expect(TokenKind::Identifier);
    return std::make_unique<NamedTypeAnnotation>(ident);
}

Statement::ptr Parser::parse_scoped_body() {
    return std::make_unique<ScopedBlockStatement>(parse_body());
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

Statement::ptr Parser::parse_return_statement() {
    expect(TokenKind::Return);
    Expression::ptr value = parse_expression();
    expect(TokenKind::Semicolon);

    return std::make_unique<ReturnStatement>(std::move(value));
}

Statement::ptr Parser::parse_if_else_statement() {
    expect(TokenKind::If);
    
    Expression::ptr condition = parse_expression();

    Statement::ptr then_stmt = parse_scoped_body();

    Statement::ptr else_stmt;
    if (accept(TokenKind::Else)) {
        if (matches(TokenKind::If)) {
            else_stmt = parse_if_else_statement();
        } else {
            else_stmt = parse_scoped_body();
        }
    } else {
        std::vector<Statement::ptr> empty;
        else_stmt = std::make_unique<ScopedBlockStatement>(std::move(empty));
    }

    return std::make_unique<IfElseStatement>(std::move(condition), 
                                             std::move(then_stmt), 
                                             std::move(else_stmt));
}

Statement::ptr Parser::parse_while_statement() {
    expect(TokenKind::While);

    Expression::ptr condition = parse_expression();

    Statement::ptr stmt = parse_scoped_body();

    return std::make_unique<WhileStatement>(std::move(condition), 
                                            std::move(stmt));
}

Statement::ptr Parser::parse_break_statement() {
    Token token = expect(TokenKind::Break);
    expect(TokenKind::Semicolon);
    return std::make_unique<BreakStatement>(token);
}

Statement::ptr Parser::parse_continue_statement() {
    Token token = expect(TokenKind::Continue);
    expect(TokenKind::Semicolon);
    return std::make_unique<ContinueStatement>(token);
}

Expression::ptr Parser::parse_expression() {
    return parse_equality_1();
}

Expression::ptr Parser::parse_equality_1() {
    Expression::ptr left = parse_equality_2();

    Token const &token = curr();
    if (accept(TokenKind::DoubleEquals) 
            || accept(TokenKind::NotEquals)) {
        return std::make_unique<BinaryExpression>(token, std::move(left), 
                                                  parse_equality_2());
    }

    return left;
}

Expression::ptr Parser::parse_equality_2() {
    Expression::ptr left = parse_sum();

    Token const &token = curr();
    if (accept(TokenKind::LessThan) 
            || accept(TokenKind::LessEquals)
            || accept(TokenKind::GreaterThan) 
            || accept(TokenKind::GreaterEquals)) {
        return std::make_unique<BinaryExpression>(token, std::move(left), 
                                                  parse_sum());
    }

    return left;
}

Expression::ptr Parser::parse_sum() {
    Expression::ptr left = parse_term();

    Token token = curr();
    while (accept(TokenKind::Plus)
            || accept(TokenKind::Minus)) {
        left = std::make_unique<BinaryExpression>(token, std::move(left), 
                                                  parse_term());
    }

    return left;
}

Expression::ptr Parser::parse_term() {
    Expression::ptr left = parse_value();

    Token token = curr();
    while (accept(TokenKind::Times)
            || accept(TokenKind::FloorDiv)
            || accept(TokenKind::Modulo)) {
        left = std::make_unique<BinaryExpression>(token, std::move(left), 
                                                  parse_value());
    }

    return left;
}

Expression::ptr Parser::parse_value() {
    if (accept(TokenKind::ParenLeft)) {
        Expression::ptr expr = parse_expression();
        expect(TokenKind::ParenRight);
        return expr;
    }

    return parse_atom();
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

    if (accept(TokenKind::True) || accept(TokenKind::False)) {
        return std::make_unique<BooleanLiteral>(token);
    }

    std::stringstream ss;
    ss << "Expected value, but got `" << token.kind() << "`";
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
