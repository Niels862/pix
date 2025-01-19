#include "symbol-resolver.hpp"
#include "ast.hpp"
#include "error.hpp"
#include <sstream>
#include <memory>

SymbolResolver::SymbolResolver()
        {}

Node &SymbolResolver::default_action(Node &node) {
    std::stringstream ss;
    ss << "CodeGenerator(): unimplemented action: " << node.kind();
    throw FatalError(ss.str());
}

Node &SymbolResolver::visit(Program &program) {
    SymbolTable &scope = program.scope();

    std::vector<Type::unowned_ptr> params = { Type::IntType() };
    Type::unowned_ptr ret_type = Type::VoidType();

    FunctionType::ptr type = std::make_unique<FunctionType>(params, ret_type);

    FunctionSymbol::ptr func = std::make_unique<FunctionSymbol>();
    func->add_definition(std::move(type), ECallFunction::PrintInt);

    scope.insert("print", std::move(func));

    return program;
}
