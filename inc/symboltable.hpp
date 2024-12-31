#ifndef PIX_SYMBOLTABLE_HPP
#define PIX_SYMBOLTABLE_HPP

#include "symbol.hpp"
#include <unordered_map>
#include <string>

class SymbolTable {
public:
    SymbolTable();

private:
    std::unordered_map<std::string, Symbol::ptr> map;
};

#endif
