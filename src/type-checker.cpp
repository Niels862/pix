#include "type-checker.hpp"
#include "ast.hpp"

Node &TypeChecker::visit(ExpressionStatement &stmt) {
    stmt.expr().accept(*this);
    return stmt;
}

Node &TypeChecker::visit(Call &expr) {
    expr.func()->accept(*this);

    for (Expression::ptr &expr : expr.args()) {
        expr->accept(*this);
    }

    return expr;
}

Node &TypeChecker::visit(Variable &expr) {
    return expr;
}

Node &TypeChecker::visit(Integer &expr) { 
    expr.set_type(Type::IntType());
    return expr;
}
