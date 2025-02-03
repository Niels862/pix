#ifndef PIX_AST_HPP
#define PIX_AST_HPP

#include "ast-forward.hpp"
#include "visitor.hpp"
#include "token.hpp"
#include "type.hpp"
#include "symbol-table.hpp"
#include "json.hpp"
#include <vector>
#include <memory>

enum class NodeKind {
    None,
    Program,
    VariableDeclaration,
    FunctionDeclaration,
    NamedTypeAnnotation,
    ScopedBlockStatement,
    ExpressionStatement,
    ReturnStatement,
    IfElseStatement,
    WhileStatement,
    BreakStatement,
    ContinueStatement,
    UnaryExpression,
    BinaryExpression,
    Call,
    Variable,
    Integer,
    BooleanLiteral
};

std::string const &to_string(NodeKind kind);

std::ostream &operator <<(std::ostream &stream, NodeKind kind);

class Node {
public:
    Node();

    virtual ~Node() = default;

    virtual Node &accept(AstVisitor &visitor) = 0;

    virtual JSON::ptr to_json() const = 0;

    virtual NodeKind kind() const = 0;

    virtual TextPosition const &pos() const = 0;

    using ptr = std::unique_ptr<Node>;
    using unowned_ptr = Node *;

    Type::unowned_ptr &type() { return m_type; }

    void set_type(Type::unowned_ptr type) { m_type = type; }

    template <typename T>
    static JSONList::ptr to_json_list(std::vector<T> const &list);

protected:
    Type::unowned_ptr m_type;
};

class Statement : public Node {
public:
    Statement();

    JSON::ptr to_json() const override;

    using ptr = std::unique_ptr<Statement>;
    using unowned_ptr = Statement *;

private:
    virtual void add_json_attributes(JSONObject &object) const = 0;
};

class Expression : public Node {
public:
    Expression();

    JSON::ptr to_json() const override;

    using ptr = std::unique_ptr<Expression>;
    using unowned_ptr = Expression *;

private:
    virtual void add_json_attributes(JSONObject &object) const = 0;
};

class TypeAnnotation : public Node {
public:
    TypeAnnotation();

    JSON::ptr to_json() const override;

    using ptr = std::unique_ptr<TypeAnnotation>;
    using unowned_ptr = TypeAnnotation *;

private:
    virtual void add_json_attributes(JSONObject &object) const = 0;
};

class Program : public Node {
public:
    Program(std::vector<Statement::ptr> stmts);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); }

    NodeKind kind() const override { return NodeKind::Program; }

    TextPosition const &pos() const override { return m_stmts.front()->pos(); }

    JSON::ptr to_json() const override;

    std::vector<Statement::ptr> &stmts() { return m_stmts; }

    SymbolTable &symbols() { return m_symbols; }

    using ptr = std::unique_ptr<Program>;

private:
    std::vector<Statement::ptr> m_stmts;

    SymbolTable m_symbols;
};

class VariableDeclaration : public Statement {
public:
    VariableDeclaration(Token const &ident, TypeAnnotation::ptr type);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::VariableDeclaration; }

    TextPosition const &pos() const override { return m_ident.pos(); }

    Token const &ident() const { return m_ident; }

    TypeAnnotation::ptr &annotation() { return m_annotation; }

    using ptr = std::unique_ptr<VariableDeclaration>;

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_ident;

    TypeAnnotation::ptr m_annotation;
};

class FunctionDeclaration : public Statement {
public:
    FunctionDeclaration(Token const &func, 
                        std::vector<VariableDeclaration::ptr> params, 
                        TypeAnnotation::ptr ret_type_annotation,
                        std::vector<Statement::ptr> body);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::FunctionDeclaration; }

    TextPosition const &pos() const override { return m_func.pos(); }

    Token const &func() const { return m_func; }

    std::vector<VariableDeclaration::ptr> &params() { return m_params; }

    TypeAnnotation::ptr &ret_type_annotation() { return m_ret_type_annotation; }

    std::vector<Statement::ptr> &body() { return m_body; }

    SymbolTable &symbols() { return m_symbols; }

    FunctionDefinition &definition() { return *m_definition; }

    void set_definition(FunctionDefinition &definition) 
            { m_definition = &definition; }

    using ptr = std::unique_ptr<FunctionDeclaration>;
    using unowned_ptr = FunctionDeclaration *;

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_func;

    std::vector<VariableDeclaration::ptr> m_params;

    TypeAnnotation::ptr m_ret_type_annotation;

    std::vector<Statement::ptr> m_body;

    SymbolTable m_symbols;

    FunctionDefinition::unowned_ptr m_definition;
};

class NamedTypeAnnotation : public TypeAnnotation {
public:
    NamedTypeAnnotation(Token const &ident);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::NamedTypeAnnotation; }

    TextPosition const &pos() const override { return m_ident.pos(); }

    Token const &ident() const { return m_ident; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_ident;
};

class ScopedBlockStatement : public Statement {
public:
    ScopedBlockStatement(std::vector<Statement::ptr> body);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::ScopedBlockStatement; }

    TextPosition const &pos() const override { return m_body.front()->pos(); }

    std::vector<Statement::ptr> &body() { return m_body; }

    SymbolTable &symbols() { return m_symbols; }

private:
    void add_json_attributes(JSONObject &object) const;

    std::vector<Statement::ptr> m_body;

    SymbolTable m_symbols;
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Expression::ptr expr);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::ExpressionStatement; }

    TextPosition const &pos() const override { return m_expr->pos(); }

    Expression::ptr &expr() { return m_expr; }
    
private:
    void add_json_attributes(JSONObject &object) const;

    Expression::ptr m_expr;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(Expression::ptr value);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::ReturnStatement; }

    TextPosition const &pos() const override { return m_value->pos(); } // todo

    Expression::ptr &value() { return m_value; }
    
private:
    void add_json_attributes(JSONObject &object) const;

    Expression::ptr m_value;
};

class IfElseStatement : public Statement {
public:
    IfElseStatement(Expression::ptr condition, Statement::ptr then_stmt, 
                    Statement::ptr else_stmt);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::IfElseStatement; }

    TextPosition const &pos() const override { return m_condition->pos(); }

    Expression::ptr &condition() { return m_condition; }

    Statement::ptr &then_stmt() { return m_then_stmt; }

    Statement::ptr &else_stmt() { return m_else_stmt; }

private:
    void add_json_attributes(JSONObject &object) const;

    Expression::ptr m_condition;

    Statement::ptr m_then_stmt;

    Statement::ptr m_else_stmt;
};

class WhileStatement : public Statement {
public:
    WhileStatement(Expression::ptr condition, Statement::ptr loop_stmt);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::WhileStatement; }

    TextPosition const &pos() const override { return m_condition->pos(); }

    Expression::ptr &condition() { return m_condition; }

    Statement::ptr &loop_stmt() { return m_loop_stmt; }

private:
    void add_json_attributes(JSONObject &object) const;

    Expression::ptr m_condition;

    Statement::ptr m_loop_stmt;
};

class BreakStatement : public Statement {
public:
    BreakStatement(Token const &token);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::BreakStatement; }

    TextPosition const &pos() const override { return m_token.pos(); }

private:
    void add_json_attributes(JSONObject &) const {}

    Token m_token;
};

class ContinueStatement : public Statement {
public:
    ContinueStatement(Token const &token);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::ContinueStatement; }

    TextPosition const &pos() const override { return m_token.pos(); }

private:
    void add_json_attributes(JSONObject &) const {}

    Token m_token;
};

class UnaryExpression : public Expression {
public:
    UnaryExpression(Token const &op, Expression::ptr operand);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); }

    NodeKind kind() const override { return NodeKind::UnaryExpression; }

    TextPosition const &pos() const override { return m_operand->pos(); }

    Token const &op() const { return m_op; }

    Expression::ptr &operand() { return m_operand; }

private:    
    void add_json_attributes(JSONObject &object) const;

    Token m_op;

    Expression::ptr m_operand;
};

class BinaryExpression : public Expression {
public:
    BinaryExpression(Token const &op, Expression::ptr left, 
                     Expression::ptr right);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); }

    NodeKind kind() const override { return NodeKind::BinaryExpression; }

    TextPosition const &pos() const override { return m_left->pos(); }

    Token const &op() const { return m_op; }

    Expression::ptr &left() { return m_left; }

    Expression::ptr &right() { return m_right; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_op;

    Expression::ptr m_left;

    Expression::ptr m_right;
};

class Call : public Expression {
public:
    Call(Token const &func, std::vector<Expression::ptr> args);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::Call; }

    TextPosition const &pos() const override { return m_func.pos(); }

    Token const &func() { return m_func; }

    std::vector<Expression::ptr> &args() { return m_args; } 

    FunctionDefinition &called() { return *m_called; }

    void set_called(FunctionDefinition &called) { m_called = &called; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_func;

    std::vector<Expression::ptr> m_args;

    FunctionDefinition::unowned_ptr m_called;
};

class Variable : public Expression {
public:
    Variable(Token const &ident);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::Variable; }

    TextPosition const &pos() const override { return m_ident.pos(); }

    Token const &ident() const { return m_ident; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_ident;
};

class Integer : public Expression {
public:
    Integer(Token const &literal);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::Integer; }

    TextPosition const &pos() const override { return m_literal.pos(); }

    Token const &literal() const { return m_literal; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_literal;
};

class BooleanLiteral : public Expression {
public:
    BooleanLiteral(Token const &literal);

    Node &accept(AstVisitor &visitor) override { return visitor.visit(*this); } 

    NodeKind kind() const override { return NodeKind::BooleanLiteral; }

    TextPosition const &pos() const override { return m_literal.pos(); }

    Token const &literal() const { return m_literal; }

private:
    void add_json_attributes(JSONObject &object) const;

    Token m_literal;

};

#endif
