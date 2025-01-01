#include "visitor.hpp"
#include "ast.hpp"

Node &AstVisitor::default_action(Node &node) {
    return node;
}

Node &AstVisitor::visit(ExpressionStatement &stmt) {
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
