#include "type-checker.hpp"
#include "error.hpp"
#include "parser.hpp" // for ParserError
#include <sstream>
#include <string>

Node &TypeChecker::default_action(Node &node) {
    std::stringstream ss;
    ss << "TypeChecker(): unimplemented action: " << node.kind();
    throw FatalError(ss.str());
}

Node &TypeChecker::visit(ExpressionStatement &stmt) {
    stmt.expr().accept(*this);
    return stmt;
}

Node &TypeChecker::visit(ReturnStatement &stmt) {
    stmt.value().accept(*this);
    return stmt;
}

Node &TypeChecker::visit(Program &program) {
    m_scope.enter(program.symbols());

    std::cout << m_scope.current() << std::endl;

    for (Statement::ptr &stmt : program.stmts()) {
        stmt->accept(*this);
    }

    return program;
}

Node &TypeChecker::visit(VariableDeclaration &decl) {
    return decl;
}

Node &TypeChecker::visit(FunctionDeclaration &decl) {
    m_scope.enter(decl.symbols());

    for (Statement::ptr &stmt : decl.body()) {
        stmt->accept(*this);
    }

    m_scope.leave(decl.symbols());

    return decl;
}

Node &TypeChecker::visit(Call &expr) {
    for (Expression::ptr &expr : expr.args()) {
        expr->accept(*this);
    }

    std::string const &name = expr.func().lexeme();
    
    Symbol::unowned_ptr symbol = m_scope.lookup(expr.func());
    FunctionSymbol::unowned_ptr func_symbol = 
            dynamic_cast<FunctionSymbol *>(symbol);

    if (!func_symbol) {
        throw ParserError(expr.func().pos(), name + " is not a function");
    }

    std::vector<FunctionDefinition> &defs = func_symbol->definitions();
    if (defs.size() != 1) {
        throw ParserError(expr.func().pos(), "overloads not implemented");
    }

    FunctionDefinition &def = defs.front();
    std::vector<Type::unowned_ptr> const &param_types = def.type()->param_types();

    if (expr.args().size() != param_types.size()) {
        std::stringstream ss;
        ss << expr.func().lexeme() << " got " << expr.args().size()
           << " arguments, but expected " << param_types.size();
        throw ParserError(expr.func().pos(), ss.str());
    }

    for (std::size_t i = 0; i < expr.args().size(); i++) {
        coerce_types(expr.args()[i], param_types[i], 
                     expr.func().pos(), "In call to " + expr.func().lexeme());
    }

    expr.set_called(def);
    expr.set_type(def.type()->ret_type());
    
    return expr;
}

Node &TypeChecker::visit(Variable &expr) {
    Symbol::unowned_ptr symbol = m_scope.lookup(expr.ident());
    VariableSymbol::unowned_ptr var_symbol 
            = dynamic_cast<VariableSymbol *>(symbol);

    if (!var_symbol) {
        throw ParserError(
                expr.ident().pos(), expr.ident() + " is not a variable");
    }

    expr.set_type(var_symbol->type());
    return expr;
}

Node &TypeChecker::visit(Integer &expr) { 
    expr.set_type(Type::IntType());
    return expr;
}

void TypeChecker::coerce_types(Expression::ptr &target, 
                               Type::unowned_ptr expected, 
                               TextPosition const &pos,
                               std::string const &context) {
    if (target->type() == expected) { // TODO FIXME works for now but fix
        return;
    }

    std::stringstream ss;
    ss << context << ": cannot use value of type `" << *target->type() 
       << "` as `" << *expected << "`";
    throw ParserError(pos, ss.str());
}
