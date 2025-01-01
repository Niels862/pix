#ifndef PIX_AST_HPP
#define PIX_AST_HPP

#include "ast-forward.hpp"
#include "visitor.hpp"
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

    virtual ~Node() = default;

    virtual Node &accept(AstVisitor &visitor) = 0;

    virtual Type::unowned_ptr check_type() = 0;

    virtual JSON::ptr to_json() const = 0;

    virtual NodeKind kind() const = 0;

    using ptr = std::unique_ptr<Node>;

    Type::unowned_ptr &type() { return m_type; }

    void set_type(Type::unowned_ptr type) { m_type = type; }

protected:
    Type::unowned_ptr m_type;
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

private:
    virtual void add_json_attributes(JSONObject &object) const = 0;
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Expression::ptr expr);

    Node &accept(AstVisitor &visitor) { return visitor.visit(*this); } 

    virtual Type::unowned_ptr check_type();

    virtual NodeKind kind() const { return NodeKind::ExpressionStatement; }

    Expression &expr() { return *m_expr; }
protected:
    Expression::ptr m_expr;

private:
    void add_json_attributes(JSONObject &object) const;
};

class Call : public Expression {
public:
    Call(Expression::ptr func, std::vector<Expression::ptr> args);

    Node &accept(AstVisitor &visitor) { return visitor.visit(*this); } 

    virtual Type::unowned_ptr check_type();

    virtual NodeKind kind() const { return NodeKind::Call; }

    Expression::ptr &func() { return m_func; }

    std::vector<Expression::ptr> &args() { return m_args; } 
private:
    void add_json_attributes(JSONObject &object) const;

    Expression::ptr m_func;

    std::vector<Expression::ptr> m_args;
};

class Variable : public Expression {
public:
    Variable(Token const &ident);

    Node &accept(AstVisitor &visitor) { return visitor.visit(*this); } 

    virtual Type::unowned_ptr check_type();

    virtual NodeKind kind() const { return NodeKind::Variable; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_ident;
};

class Integer : public Expression {
public:
    Integer(Token const &literal);

    Node &accept(AstVisitor &visitor) { return visitor.visit(*this); } 

    virtual Type::unowned_ptr check_type();

    virtual NodeKind kind() const { return NodeKind::Integer; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_literal;
};

#endif
