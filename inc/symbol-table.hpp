#ifndef PIX_SYMBOLTABLE_HPP
#define PIX_SYMBOLTABLE_HPP

#include "symbol.hpp"
#include <unordered_map>
#include <string>

class SymbolTable {
public:
    SymbolTable();

    void insert(std::string const &ident, Symbol::ptr symbol);

    Symbol::unowned_ptr lookup(std::string const &ident) const;

    bool defines(std::string const &ident) const;

    friend std::ostream &operator <<(std::ostream &stream, 
                                     SymbolTable const &table);

    using unowned_ptr = SymbolTable *;

private:
    std::unordered_map<std::string, Symbol::ptr> m_map;
};

#endif
