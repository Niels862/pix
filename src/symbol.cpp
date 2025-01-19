#include "symbol.hpp"

Symbol::Symbol()
        {}

TypeSymbol::TypeSymbol(Type::ptr type)
        : Symbol{}, m_type{std::move(type)} {}

FunctionSymbol::FunctionSymbol()
        : Symbol{}, m_definitions{} {}

void FunctionSymbol::add_definition(FunctionType::ptr type, 
                                    ECallFunction ecall) {
    m_definitions.emplace_back(std::move(type), ecall);
}

FunctionDefinition::FunctionDefinition(FunctionType::ptr type, 
                                       ECallFunction ecall)
        : m_type{std::move(type)}, m_ecall{ecall} {}

