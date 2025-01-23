#include "symbol-resolver.hpp"
#include "ast.hpp"
#include "error.hpp"
#include <sstream>
#include <memory>

SymbolResolver::SymbolResolver()
        : scope{nullptr} {}

Node &SymbolResolver::visit(Program &program) {
    scope = &program.scope();

    std::vector<Type::unowned_ptr> params = { Type::IntType() };
    Type::unowned_ptr ret_type = Type::VoidType();

    FunctionType::ptr type = std::make_unique<FunctionType>(params, ret_type);

    FunctionSymbol::ptr func = std::make_unique<FunctionSymbol>();
    func->add_definition(std::move(type), ECallFunction::PrintInt);

    scope->insert("print", std::move(func));

    for (Statement::ptr &stmt : program.stmts()) {
        stmt->accept(*this);
    }

    return program;
}

Node &SymbolResolver::visit(FunctionDeclaration &decl) {
    std::vector<Type::unowned_ptr> params;
    Type::unowned_ptr ret_type = Type::VoidType();

    FunctionType::ptr type = std::make_unique<FunctionType>(params, ret_type);

    FunctionSymbol::ptr func = std::make_unique<FunctionSymbol>();
    func->add_definition(std::move(type), &decl);

    scope->insert(decl.func().lexeme(), std::move(func));

    return decl;
}
