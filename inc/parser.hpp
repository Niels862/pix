#ifndef PIX_PARSER_HPP
#define PIX_PARSER_HPP

#include "token.hpp"
#include "ast.hpp"
#include <vector>
#include <stdexcept>

class ParserError : public std::exception {
public:
    ParserError(TextPosition const &pos, std::string const &msg);

    char const *what() const noexcept override {
        return m_msg.c_str();
    }
private:
    std::string m_msg;
};

class Parser {
public:
    Parser(std::vector<Token> tokens);

    Program::ptr parse();
private:
    void advance();

    Token const &matches(TokenKind kind) const;

    Token const &accept(TokenKind kind);

    Token const &expect(TokenKind kind);

    Token const &curr() const;

    Token const &peek(std::size_t offset = 1) const;

    Statement::ptr parse_statement();

    FunctionDeclaration::ptr parse_function_declaration();

    std::vector<VariableDeclaration::ptr> parse_function_parameters();

    VariableDeclaration::ptr parse_variable_declaration();

    TypeAnnotation::ptr parse_type_annotation();

    Statement::ptr parse_scoped_body();

    std::vector<Statement::ptr> parse_body();

    Statement::ptr parse_expression_statement();

    Statement::ptr parse_return_statement();

    Statement::ptr parse_if_else_statement();

    Statement::ptr parse_while_statement();

    Expression::ptr parse_expression();

    Expression::ptr parse_value();

    Expression::ptr parse_atom();

    std::vector<Expression::ptr> parse_call_args();

    std::vector<Token> m_tokens;

    std::size_t m_curr_idx;
};

#endif
