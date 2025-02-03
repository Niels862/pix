#include "visitor.hpp"
#include "ast.hpp"

Node &AstVisitor::default_action(Node &node) {
    return node;
}

Node &AstVisitor::visit(Program &program) {
    return default_action(program);
}

Node &AstVisitor::visit(VariableDeclaration &decl) {
    return default_action(decl);
}

Node &AstVisitor::visit(FunctionDeclaration &decl) {
    return default_action(decl);
}

Node &AstVisitor::visit(NamedTypeAnnotation &anno) {
    return default_action(anno);
}

Node &AstVisitor::visit(ExpressionStatement &stmt) {
    return default_action(stmt);
}

Node &AstVisitor::visit(ReturnStatement &stmt) {
    return default_action(stmt);
}

Node &AstVisitor::visit(Call &expr) {
    return default_action(expr);
}

Node &AstVisitor::visit(Variable &expr) {
    return default_action(expr);
}

Node &AstVisitor::visit(Integer &expr) {
    return default_action(expr);
}

Node &AstVisitor::visit(BooleanLiteral &literal) {
    return default_action(literal);
}
