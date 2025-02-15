#ifndef PIX_TYPE_CHECKER_HPP
#define PIX_TYPE_CHECKER_HPP

#include "visitor.hpp"
#include "ast.hpp"
#include "symbol-table.hpp"

class TypeChecker : public AstVisitor {
public:
    TypeChecker() = default;

    Node &default_action(Node &node) override;

    Node &visit(Program &program) override;

    Node &visit(ParameterDeclaration &decl) override;

    Node &visit(FunctionDeclaration &decl) override;

    Node &visit(VariableDeclaration &decl) override;

    Node &visit(ScopedBlockStatement &stmt) override;

    Node &visit(ExpressionStatement &stmt) override;

    Node &visit(AssignStatement &stmt) override;

    Node &visit(ReturnStatement &stmt) override;

    Node &visit(IfElseStatement &stmt) override;

    Node &visit(WhileStatement &stmt) override;

    Node &visit(BreakStatement &stmt) override;

    Node &visit(ContinueStatement &stmt) override;

    Node &visit(UnaryExpression &expr) override;

    Node &visit(BinaryExpression &expr) override;

    Node &visit(Call &expr) override;
    
    Node &visit(Variable &expr) override;

    Node &visit(Integer &expr) override;

    Node &visit(BooleanLiteral &expr) override;

private:
    void coerce_types(Expression::ptr &target, Type::unowned_ptr expected, 
                      TextPosition const &pos, std::string const &context);

    SymbolScope m_scope;

    FunctionDeclaration::unowned_ptr m_curr_function;
};

#endif
