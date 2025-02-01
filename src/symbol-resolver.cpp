#include "symbol-resolver.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "error.hpp"
#include <sstream>
#include <memory>

SymbolResolver::SymbolResolver()
        : m_scope{} {}

Node &SymbolResolver::visit(Program &program) {
    m_scope.enter(program.symbols());

    /* TEMP, TODO: std functions  */

    std::vector<Type::unowned_ptr> params = { Type::IntType() };
    Type::unowned_ptr ret_type = Type::VoidType();

    FunctionType::ptr type = std::make_unique<FunctionType>(params, ret_type);

    FunctionSymbol::ptr func = std::make_unique<FunctionSymbol>();
    func->add_definition(std::move(type), ECallFunction::PrintInt);

    m_scope.current().insert("print", std::move(func));

    /* TEMP, TODO: basic types */

    BasicTypeSymbol::ptr int_type = 
            std::make_unique<BasicTypeSymbol>(Type::IntType());
    m_scope.current().insert("int", std::move(int_type));

    BasicTypeSymbol::ptr void_type = 
            std::make_unique<BasicTypeSymbol>(Type::VoidType());
    m_scope.current().insert("void", std::move(void_type));

    /*for (Statement::ptr &stmt : program.stmts()) {
        // TODO ... forward declare classes here
    }*/

    for (Statement::ptr &stmt : program.stmts()) {
        stmt->accept(*this);
    }

    m_scope.leave(program.symbols());
    return program;
}

Node &SymbolResolver::visit(VariableDeclaration &decl) {
    decl.annotation().accept(*this);

    LocalVariableSymbol::ptr var 
            = std::make_unique<LocalVariableSymbol>(decl.annotation().type());
    m_scope.current().insert(decl.ident().lexeme(), std::move(var));
    
    return decl;
}

Node &SymbolResolver::visit(FunctionDeclaration &decl) {
    m_scope.enter(decl.symbols());

    std::vector<Type::unowned_ptr> params;
    Type::unowned_ptr ret_type = Type::VoidType();

    for (VariableDeclaration::ptr &param : decl.params()) {
        param->accept(*this);
        params.push_back(param->annotation().type());
    }

    FunctionType::ptr type = std::make_unique<FunctionType>(params, ret_type);

    FunctionSymbol::ptr func = std::make_unique<FunctionSymbol>();
    func->add_definition(std::move(type), &decl);

    m_scope.leave(decl.symbols());

    m_scope.current().insert(decl.func().lexeme(), std::move(func));

    return decl;
}

Node &SymbolResolver::visit(NamedTypeAnnotation &anno) {
    Symbol::unowned_ptr symbol = m_scope.lookup(anno.ident());
    TypeSymbol::unowned_ptr type_symbol = 
            dynamic_cast<TypeSymbol *>(symbol);

    if (!type_symbol) {
        throw ParserError(anno.ident().pos(), 
                          anno.ident().lexeme() + " is not a type");
    }

    anno.set_type(type_symbol->type());
    return anno;
}