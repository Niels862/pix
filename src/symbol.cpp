#include "symbol.hpp"

Symbol::Symbol()
        {}

std::ostream &operator <<(std::ostream &stream, Symbol const &symbol) {
    symbol.write(stream);
    return stream;
}

TypeSymbol::TypeSymbol(Type::ptr type)
        : Symbol{}, m_type{std::move(type)} {}

void TypeSymbol::write(std::ostream &stream) const {
    stream << "Type<" << *m_type << ">";
}

FunctionSymbol::FunctionSymbol()
        : Symbol{}, m_definitions{} {}

void FunctionSymbol::write(std::ostream &stream) const {
    stream << "Function<" << m_definitions.size() << " definitions>";
}

void FunctionSymbol::add_definition(FunctionType::ptr type, 
                                    ECallFunction ecall) {
    m_definitions.emplace_back(std::move(type), ecall);
}

void FunctionSymbol::add_definition(FunctionType::ptr type, 
                                    FunctionDeclaration *decl) {
    m_definitions.emplace_back(std::move(type), decl);
}

FunctionDefinition::FunctionDefinition(FunctionType::ptr type, 
                                       ECallFunction ecall)
        : m_type{std::move(type)}, m_def{ecall} {}

FunctionDefinition::FunctionDefinition(FunctionType::ptr type, 
                                       FunctionDeclaration *decl)
        : m_type{std::move(type)}, m_def{decl} {}
