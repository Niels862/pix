#ifndef PIX_AST_HPP
#define PIX_AST_HPP

#include "token.hpp"
#include "json.hpp"
#include <vector>
#include <memory>

class Node {
public:
    Node() {}

    virtual ~Node() {}

    virtual JSON::ptr to_json() const = 0;

    using ptr = std::unique_ptr<Node>;
private:

};

class Statement : public Node {
public:
    Statement() {}

    using ptr = std::unique_ptr<Statement>;
private:

};

class Expression : public Node {
public:
    Expression() {}

    using ptr = std::unique_ptr<Expression>;

    using ptr_vector = std::vector<Expression::ptr>;
private:

};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Expression::ptr expr);

    JSON::ptr to_json() const override;
private:
    Expression::ptr m_expr;
};

class Call : public Expression {
public:
    Call(Expression::ptr func, std::vector<Expression::ptr> args);

    JSON::ptr to_json() const override;
private:
    Expression::ptr m_func;

    std::vector<Expression::ptr> m_args;
};

class Variable : public Expression {
public:
    Variable(Token const &ident);

    JSON::ptr to_json() const override;
private:
    Token m_ident;
};

class Integer : public Expression {
public:
    Integer(Token const &literal);

    JSON::ptr to_json() const override;
private:
    Token m_literal;
};

#endif
