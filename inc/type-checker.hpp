#ifndef PIX_TYPE_CHECKER_HPP
#define PIX_TYPE_CHECKER_HPP

#include "visitor.hpp"
#include "symbol-table.hpp"

class TypeChecker : public AstVisitor {
public:
    TypeChecker() = default;

    Node &default_action(Node &node) override;

    Node &visit(Program &program) override;

    Node &visit(VariableDeclaration &decl) override;

    Node &visit(FunctionDeclaration &decl) override;

    Node &visit(ExpressionStatement &stmt) override;

    Node &visit(NamedTypeAnnotation &anno) override;

    Node &visit(Call &expr) override;
    
    Node &visit(Variable &expr) override;

    Node &visit(Integer &expr) override;

private:
    SymbolScope m_scope;
};

#endif
