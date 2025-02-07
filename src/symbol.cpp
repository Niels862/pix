#include "symbol.hpp"

Symbol::Symbol()
        {}

std::ostream &operator <<(std::ostream &stream, Symbol const &symbol) {
    symbol.write(stream);
    return stream;
}

TypeSymbol::TypeSymbol()
        : Symbol{} {}

void TypeSymbol::write(std::ostream &stream) const {
    stream << "Type: " << *type();
}

BasicTypeSymbol::BasicTypeSymbol(Type::unowned_ptr type)
        : TypeSymbol{}, m_type{type} {}

DeclaredTypeSymbol::DeclaredTypeSymbol(Type::ptr type)  
        : TypeSymbol{}, m_type{std::move(type)} {}

VariableSymbol::VariableSymbol(Type::unowned_ptr type)
        : Symbol{}, m_type{type} {}

LocalVariableSymbol::LocalVariableSymbol(Type::unowned_ptr type)
        : VariableSymbol{type}, m_offset{} {}

void LocalVariableSymbol::write(std::ostream &stream) const {
    stream << "LocalVariable: " << *m_type;
}

FunctionSymbol::FunctionSymbol()
        : Symbol{}, m_definitions{} {}

void FunctionSymbol::write(std::ostream &stream) const {
    stream << "Function: ";

    bool first = true;
    for (FunctionDefinition const &def : m_definitions) {
        if (first) {
            first = false;
        } else {
            stream << " | ";
        }
        stream << def;
    }
}

void FunctionSymbol::add_definition(FunctionDefinition &&def) {
    m_definitions.push_back(std::move(def));
}

FunctionDefinition::FunctionDefinition(FunctionType::ptr type, 
                                       ECallFunction ecall)
        : m_type{std::move(type)}, m_def{ecall} {}

FunctionDefinition::FunctionDefinition(FunctionType::ptr type, 
                                       FunctionDeclaration *decl,
                                       std::vector<LocalVariableSymbol::unowned_ptr> params,
                                       std::vector<LocalVariableSymbol::unowned_ptr> locals)
        : m_type{std::move(type)}, m_def{decl}, 
          m_params{params}, m_locals{locals} {}

std::ostream &operator <<(std::ostream &stream, FunctionDefinition const &def) {
    stream << *def.m_type;
    return stream;
}
