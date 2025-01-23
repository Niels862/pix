#include "type-checker.hpp"
#include "ast.hpp"
#include "error.hpp"
#include "parser.hpp" // for ParserError
#include <sstream>

Node &TypeChecker::default_action(Node &node) {
    std::stringstream ss;
    ss << "TypeChecker(): unimplemented action: " << node.kind();
    throw FatalError(ss.str());
}

Node &TypeChecker::visit(ExpressionStatement &stmt) {
    stmt.expr().accept(*this);
    return stmt;
}

Node &TypeChecker::visit(Program &program) {
    scope = &program.scope();

    std::cout << *scope << std::endl;

    for (Statement::ptr &stmt : program.stmts()) {
        stmt->accept(*this);
    }

    return program;
}

Node &TypeChecker::visit(FunctionDeclaration &decl) {
    for (Statement::ptr &stmt : decl.body()) {
        stmt->accept(*this);
    }

    return decl;
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
        throw ParserError(expr.func().pos(), name + " is not a function");
    }

    auto &definitions = func_symbol->definitions();
    if (definitions.size() != 1) {
        throw ParserError(expr.func().pos(), "overloads not implemented");
    }

    FunctionDefinition &def = definitions.front();

    if (expr.args().size() != def.type()->param_types().size()) {
        throw ParserError(expr.func().pos(), "argcount error, todo");
    }

    expr.set_called(def);
    expr.set_type(def.type()->ret_type());
    
    return expr;
}

Node &TypeChecker::visit(Variable &expr) {
    return expr;
}

Node &TypeChecker::visit(Integer &expr) { 
    expr.set_type(Type::IntType());
    return expr;
}
