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

    declare_basic_function("iprint", Type::IntType(), Type::VoidType(), 
                           ECallFunction::PrintInt);
    declare_basic_function("bprint", Type::BoolType(), Type::VoidType(),
                           ECallFunction::PrintBool);

    declare_basic_type("int", Type::IntType());
    declare_basic_type("bool", Type::BoolType());
    declare_basic_type("void", Type::VoidType());

    /*for (Statement::ptr &stmt : program.stmts()) {
        // TODO ... forward declare classes here
    }*/

    for (Statement::ptr &stmt : program.stmts()) {
        stmt->accept(*this);
    }

    m_scope.leave(program.symbols());
    return program;
}

Node &SymbolResolver::visit(ParameterDeclaration &decl) {
    decl.annotation()->accept(*this);

    LocalVariableSymbol::ptr var 
            = std::make_unique<LocalVariableSymbol>(decl.annotation()->type());
    m_declared.push_back(var.get());
    m_scope.declare(decl.ident(), std::move(var));
    
    return decl;
}

Node &SymbolResolver::visit(FunctionDeclaration &decl) {
    m_scope.enter(decl.symbols());

    decl.ret_type_annotation()->accept(*this);
    Type::unowned_ptr ret_type = decl.ret_type_annotation()->type();

    m_declared.clear();
    std::vector<Type::unowned_ptr> param_types;
    for (ParameterDeclaration::ptr &param : decl.params()) {
        param->accept(*this);
        param_types.push_back(param->annotation()->type());
    }

    std::vector<LocalVariableSymbol::unowned_ptr> params = m_declared;
    m_declared.clear();

    for (Statement::ptr &stmt : decl.body()) {
        stmt->accept(*this);
    }

    m_scope.leave(decl.symbols());

    std::vector<LocalVariableSymbol::unowned_ptr> locals = m_declared;

    std::cout << "Declared {" << std::endl;
    for (auto const &sym : locals) {
        std::cout << *sym << std::endl;
    }
    std::cout << "}" << std::endl;

    FunctionType::ptr type = std::make_unique<FunctionType>(param_types, 
                                                            ret_type);
    FunctionSymbol::ptr func = std::make_unique<FunctionSymbol>();
    FunctionDefinition def(std::move(type), &decl, params, locals);

    func->add_definition(std::move(def));
    decl.set_definition(func->definitions().back());

    m_scope.declare(decl.func(), std::move(func));

    return decl;
}

Node &SymbolResolver::visit(VariableDeclaration &decl) {
    decl.annotation()->accept(*this);

    LocalVariableSymbol::ptr var 
            = std::make_unique<LocalVariableSymbol>(decl.annotation()->type());
    m_declared.push_back(var.get());
    m_scope.declare(decl.ident(), std::move(var));

    return decl;
}

Node &SymbolResolver::visit(NamedTypeAnnotation &anno) {
    Symbol::unowned_ptr symbol = m_scope.lookup(anno.ident());
    TypeSymbol::unowned_ptr type_symbol = 
            dynamic_cast<TypeSymbol *>(symbol);

    if (!type_symbol) {
        throw ParserError(anno.pos(), 
                          anno.ident().lexeme() + " is not a type");
    }

    anno.set_type(type_symbol->type());
    return anno;
}

Node &SymbolResolver::visit(ScopedBlockStatement &stmt) {
    m_scope.enter(stmt.symbols());

    for (Statement::ptr &substmt : stmt.body()) {
        substmt->accept(*this);
    }

    m_scope.leave(stmt.symbols());

    return stmt;
}

Node &SymbolResolver::visit(IfElseStatement &stmt) {
    stmt.then_stmt()->accept(*this);
    stmt.else_stmt()->accept(*this);
    return stmt;
}

Node &SymbolResolver::visit(WhileStatement &stmt) {
    stmt.loop_stmt()->accept(*this);
    return stmt;
}

void SymbolResolver::declare_basic_type(std::string const &name, 
                                        Type::unowned_ptr type) {
    BasicTypeSymbol::ptr symbol =
            std::make_unique<BasicTypeSymbol>(type);
    m_scope.declare(name, std::move(symbol));
}

void SymbolResolver::declare_basic_function(std::string const &name, 
                                            Type::unowned_ptr param_type, 
                                            Type::unowned_ptr ret_type, 
                                            ECallFunction ecall) {
    std::vector<Type::unowned_ptr> params = { param_type };
    FunctionType::ptr type = std::make_unique<FunctionType>(params, ret_type);

    FunctionSymbol::ptr func = std::make_unique<FunctionSymbol>();
    FunctionDefinition def(std::move(type), ecall);

    func->add_definition(std::move(def));
    m_scope.declare(name, std::move(func));
}
