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
    scope = &program.scope();

    for (Statement::ptr &stmt : program.stmts()) {
        stmt->accept(*this);
    }

    return program;
}

Node &TypeChecker::visit(Call &expr) {
    for (Expression::ptr &expr : expr.args()) {
        expr->accept(*this);
    }

    std::string const &name = expr.func().lexeme();
    
    Symbol::unowned_ptr symbol = scope->lookup(name);
    FunctionSymbol::unowned_ptr func_symbol = 
            dynamic_cast<FunctionSymbol *>(symbol);

    if (!func_symbol) {
        throw std::runtime_error(name + " was not defined");
    }

    auto &definitions = func_symbol->definitions();
    if (definitions.size() != 1) {
        throw std::runtime_error("todo");
    }

    expr.set_called(definitions.front());
    expr.set_type(definitions.front().type()->ret_type());
    
    return expr;
}

Node &TypeChecker::visit(Variable &expr) {
    return expr;
}

Node &TypeChecker::visit(Integer &expr) { 
    expr.set_type(Type::IntType());
    return expr;
}
