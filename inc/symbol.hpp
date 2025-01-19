#ifndef PIX_SYMBOL_HPP
#define PIX_SYMBOL_HPP

#include "type.hpp"
#include "instruction.hpp"
#include <memory>

class Symbol {
public:
    Symbol();

    virtual ~Symbol() = default;

    using ptr = std::unique_ptr<Symbol>;
    using unowned_ptr = Symbol *;
};

class TypeSymbol {
public:
    TypeSymbol(Type::ptr type);

    using ptr = std::unique_ptr<TypeSymbol>;
    using unowned_ptr = TypeSymbol *;

    Type::unowned_ptr type() const { return m_type.get(); }

private:
    Type::ptr m_type;
};

class FunctionSymbol {
public:
    FunctionSymbol(FunctionType::ptr type);

    using ptr = std::unique_ptr<FunctionSymbol>;
    using unowned_ptr = FunctionSymbol *;

    FunctionSymbol::unowned_ptr next() const { return m_next.get(); }

    FunctionType::unowned_ptr type() const { return m_type.get(); }

    ECallFunction ecall() const { return m_ecall; }

private:
    FunctionSymbol::ptr m_next;

    FunctionType::ptr m_type;

    ECallFunction m_ecall;
};

#endif