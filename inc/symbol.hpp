#ifndef PIX_SYMBOL_HPP
#define PIX_SYMBOL_HPP

#include "type.hpp"
#include "instruction.hpp"
#include <vector>
#include <memory>

class Symbol {
public:
    Symbol();

    virtual ~Symbol() = default;

    using ptr = std::unique_ptr<Symbol>;
    using unowned_ptr = Symbol *;
};

class TypeSymbol : public Symbol {
public:
    TypeSymbol(Type::ptr type);

    using ptr = std::unique_ptr<TypeSymbol>;
    using unowned_ptr = TypeSymbol *;

    Type::unowned_ptr type() const { return m_type.get(); }

private:
    Type::ptr m_type;
};

class FunctionDefinition;

class FunctionSymbol : public Symbol {
public:
    FunctionSymbol();

    using ptr = std::unique_ptr<FunctionSymbol>;
    using unowned_ptr = FunctionSymbol *;

    void add_definition(FunctionType::ptr, ECallFunction ecall);

    std::vector<FunctionDefinition> &definitions() { return m_definitions; }

private:
    std::vector<FunctionDefinition> m_definitions;
};

class FunctionDefinition {
public:
    FunctionDefinition(FunctionType::ptr type, ECallFunction ecall);

    FunctionType::unowned_ptr type() { return m_type.get(); }

    ECallFunction ecall() const { return m_ecall; }

    using unowned_ptr = FunctionDefinition *;

private:
    FunctionType::ptr m_type;

    ECallFunction m_ecall;
};

#endif
