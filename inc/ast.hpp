#ifndef PIX_AST_HPP
#define PIX_AST_HPP

#include "token.hpp"
#include "type.hpp"
#include "json.hpp"
#include <vector>
#include <memory>

enum class NodeKind {
    None,
    ExpressionStatement,
    Call,
    Variable,
    Integer
};

std::string const &to_string(NodeKind kind);

std::ostream &operator <<(std::ostream &stream, NodeKind kind);

class Node {
public:
    Node();

    virtual ~Node() {}

    virtual Type::unowned_ptr check_type() = 0;

    virtual JSON::ptr to_json() const = 0;

    virtual NodeKind kind() const = 0;

    using ptr = std::unique_ptr<Node>;
};

class Statement : public Node {
public:
    Statement();

    JSON::ptr to_json() const override;

    using ptr = std::unique_ptr<Statement>;

private:
    virtual void add_json_attributes(JSONObject &object) const = 0;
};

class Expression : public Node {
public:
    Expression();

    JSON::ptr to_json() const override;

    using ptr = std::unique_ptr<Expression>;

protected:
    Type::unowned_ptr m_type;

private:
    virtual void add_json_attributes(JSONObject &object) const = 0;
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Expression::ptr expr);

    virtual Type::unowned_ptr check_type();

    virtual NodeKind kind() const { return NodeKind::ExpressionStatement; }

protected:
    Expression::ptr m_expr;

private:
    void add_json_attributes(JSONObject &object) const;
};

class Call : public Expression {
public:
    Call(Expression::ptr func, std::vector<Expression::ptr> args);

    virtual Type::unowned_ptr check_type();

    virtual NodeKind kind() const { return NodeKind::Call; }

private:
    void add_json_attributes(JSONObject &object) const;

    Expression::ptr m_func;

    std::vector<Expression::ptr> m_args;
};

class Variable : public Expression {
public:
    Variable(Token const &ident);

    virtual Type::unowned_ptr check_type();

    virtual NodeKind kind() const { return NodeKind::Variable; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_ident;
};

class Integer : public Expression {
public:
    Integer(Token const &literal);

    virtual Type::unowned_ptr check_type();

    virtual NodeKind kind() const { return NodeKind::Integer; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_literal;
};

#endif
