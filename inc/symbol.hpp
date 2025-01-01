#ifndef PIX_SYMBOL_HPP
#define PIX_SYMBOL_HPP

#include "type.hpp"
#include <memory>

class Symbol {
public:
    Symbol();

    virtual ~Symbol() = default;

    using ptr = std::unique_ptr<Symbol>;
};

class TypeSymbol {
public:
    TypeSymbol(Type::ptr type);

private:
    Type::ptr m_type;
};

class FunctionSymbol {
public:
    FunctionSymbol(FunctionType::ptr type);

private:
    FunctionType::ptr m_type;
};

#endif