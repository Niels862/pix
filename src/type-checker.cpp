#include "type-checker.hpp"
#include "ast.hpp"
#include "error.hpp"
#include <sstream>

Node &TypeChecker::default_action(Node &node) {
    std::stringstream ss;
    ss << "CodeGenerator(): unimplemented action: " << node.kind();
    throw FatalError(ss.str());
}

Node &TypeChecker::visit(ExpressionStatement &stmt) {
    stmt.expr().accept(*this);
    return stmt;
}

Node &TypeChecker::visit(Program &program) {
    for (Statement::ptr &stmt : program.stmts()) {
        stmt->accept(*this);
    }

    return program;
}

Node &TypeChecker::visit(Call &expr) {
    for (Expression::ptr &expr : expr.args()) {
        expr->accept(*this);
    }

    expr.set_type(Type::VoidType());
    return expr;
}

Node &TypeChecker::visit(Variable &expr) {
    return expr;
}

Node &TypeChecker::visit(Integer &expr) { 
    expr.set_type(Type::IntType());
    return expr;
}
