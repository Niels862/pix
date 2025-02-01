#ifndef PIX_SYMBOLTABLE_HPP
#define PIX_SYMBOLTABLE_HPP

#include "symbol.hpp"
#include "token.hpp"
#include <unordered_map>
#include <string>

class SymbolScope;

class SymbolTable {
public:
    SymbolTable();

    void insert(std::string const &ident, Symbol::ptr symbol);

    Symbol::unowned_ptr lookup(Token const &ident) const;

    bool defines(std::string const &ident) const;

    friend std::ostream &operator <<(std::ostream &stream, 
                                     SymbolTable const &table);

    using unowned_ptr = SymbolTable *;

private:
    std::unordered_map<std::string, Symbol::ptr> m_map;

    friend SymbolScope;
};

class SymbolScope {
public:
    SymbolScope();

    void enter(SymbolTable &symbols);

    void leave(SymbolTable &symbols);

    Symbol::unowned_ptr lookup(Token const &ident) const;

    void declare(std::string const &ident, Symbol::ptr symbol);

    void declare(Token const &ident, Symbol::ptr symbol);

    SymbolTable &current() { return *m_tables.back(); }

    SymbolTable &global() { return *m_tables.front(); }

private:
    std::vector<SymbolTable *> m_tables;
};

#endif
