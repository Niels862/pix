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
    TypeSymbol();

    void write(std::ostream &stream) const override;

    using ptr = std::unique_ptr<TypeSymbol>;
    using unowned_ptr = TypeSymbol *;

    virtual Type::unowned_ptr type() const = 0;
};

class BasicTypeSymbol : public TypeSymbol {
public:
    BasicTypeSymbol(Type::unowned_ptr type);

    using ptr = std::unique_ptr<BasicTypeSymbol>;
    using unowned_ptr = BasicTypeSymbol *;

    Type::unowned_ptr type() const override { return m_type; }

private:
    Type::unowned_ptr m_type;
};

class DeclaredTypeSymbol : public TypeSymbol {
public:
    DeclaredTypeSymbol(Type::ptr type);

    using ptr = std::unique_ptr<BasicTypeSymbol>;
    using unowned_ptr = BasicTypeSymbol *;

    Type::unowned_ptr type() const override { return m_type.get(); }

private:
    Type::ptr m_type;
};

class VariableSymbol : public Symbol {
public:
    VariableSymbol(Type::unowned_ptr type);

    using ptr = std::unique_ptr<VariableSymbol>;
    using unowned_ptr = VariableSymbol *;

    Type::unowned_ptr type() const { return m_type; }

protected:
    Type::unowned_ptr m_type;
};

class LocalVariableSymbol : public VariableSymbol {
public:
    LocalVariableSymbol(Type::unowned_ptr);

    void write(std::ostream &stream) const override;

    using ptr = std::unique_ptr<LocalVariableSymbol>;
    using unowned_ptr = LocalVariableSymbol *;

    int offset() const { return m_offset; }

    void set_offset(int offset) { m_offset = offset; }

private:
    int m_offset;
};

class FunctionDefinition;

class FunctionSymbol : public Symbol {
public:
    FunctionSymbol();

    void write(std::ostream &stream) const override;

    using ptr = std::unique_ptr<FunctionSymbol>;
    using unowned_ptr = FunctionSymbol *;

    void add_definition(FunctionDefinition &&def);

    std::vector<std::unique_ptr<FunctionDefinition>> &definitions() 
            { return m_definitions; }

private:
    std::vector<std::unique_ptr<FunctionDefinition>> m_definitions;
};

class FunctionDefinition {
public:
    FunctionDefinition(FunctionType::ptr type, ECallFunction ecall);
   
    FunctionDefinition(FunctionType::ptr type, FunctionDeclaration *decl,
                       std::vector<LocalVariableSymbol::unowned_ptr> params,
                       std::vector<LocalVariableSymbol::unowned_ptr> locals);

    bool is_ecall() const 
            { return std::holds_alternative<ECallFunction>(m_def); }

    FunctionType::unowned_ptr type() { return m_type.get(); }

    ECallFunction ecall() const { return std::get<ECallFunction>(m_def); }

    FunctionDeclaration const *decl() const
            { return std::get<FunctionDeclaration *>(m_def); }

    FunctionDeclaration *decl()
            { return std::get<FunctionDeclaration *>(m_def); }

    std::vector<LocalVariableSymbol::unowned_ptr> const &params()
            { return m_params; }

    std::vector<LocalVariableSymbol::unowned_ptr> const &locals() 
            { return m_locals; }

    friend std::ostream &operator <<(std::ostream &stream, 
                                     FunctionDefinition const &def);

    using ptr = std::unique_ptr<FunctionDefinition>;

    using unowned_ptr = FunctionDefinition *;

private:
    FunctionType::ptr m_type;

    std::variant<ECallFunction, FunctionDeclaration *> m_def;

    std::vector<LocalVariableSymbol::unowned_ptr> m_params;

    std::vector<LocalVariableSymbol::unowned_ptr> m_locals; 
};

#endif
