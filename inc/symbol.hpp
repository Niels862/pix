#ifndef PIX_SYMBOL_HPP
#define PIX_SYMBOL_HPP

#include "ast-forward.hpp"
#include "type.hpp"
#include "instruction.hpp"
#include <iostream>
#include <vector>
#include <variant>
#include <memory>

class Symbol {
public:
    Symbol();

    virtual ~Symbol() = default;

    virtual void write(std::ostream &stream) const = 0;

    friend std::ostream &operator <<(std::ostream &stream, 
                                     Symbol const &symbol);

    using ptr = std::unique_ptr<Symbol>;
    using unowned_ptr = Symbol *;
};

class TypeSymbol : public Symbol {
public:
    TypeSymbol(Type::ptr type);

    void write(std::ostream &stream) const;

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

    void write(std::ostream &stream) const;

    using ptr = std::unique_ptr<FunctionSymbol>;
    using unowned_ptr = FunctionSymbol *;

    void add_definition(FunctionType::ptr, ECallFunction ecall);

    void add_definition(FunctionType::ptr, FunctionDeclaration *decl);

    std::vector<FunctionDefinition> &definitions() { return m_definitions; }

private:
    std::vector<FunctionDefinition> m_definitions;
};

class FunctionDefinition {
public:
    FunctionDefinition(FunctionType::ptr type, ECallFunction ecall);
   
    FunctionDefinition(FunctionType::ptr type, FunctionDeclaration *decl);

    bool is_ecall() const 
            { return std::holds_alternative<ECallFunction>(m_def); }

    FunctionType::unowned_ptr type() { return m_type.get(); }

    ECallFunction ecall() const { return std::get<ECallFunction>(m_def); }

    FunctionDeclaration const *decl() const
            { return std::get<FunctionDeclaration *>(m_def); }

    FunctionDeclaration *decl()
            { return std::get<FunctionDeclaration *>(m_def); }

    using unowned_ptr = FunctionDefinition *;

private:
    FunctionType::ptr m_type;

    std::variant<ECallFunction, FunctionDeclaration *> m_def;
};

#endif
