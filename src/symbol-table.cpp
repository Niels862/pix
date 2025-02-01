#include "symbol-table.hpp"
#include "error.hpp"
#include "parser.hpp" // For ParserError, TODO
#include <iomanip>

SymbolTable::SymbolTable()
        : m_map{} {}

void SymbolTable::insert(std::string const &ident, Symbol::ptr symbol) {
    m_map[ident] = std::move(symbol);
}

Symbol::unowned_ptr SymbolTable::lookup(Token const &ident) const {
    auto iter = m_map.find(ident.lexeme());

    if (iter == m_map.end()) {
        return nullptr;
    }
    
    return iter->second.get();
}

bool SymbolTable::defines(std::string const &ident) const {
    return m_map.find(ident) != m_map.end();
}

std::ostream &operator <<(std::ostream &stream, SymbolTable const &table) {
    stream << "{\n";

    bool first = true;
    for (auto const &kv : table.m_map) {
        if (first) {
            first = false;
        } else {
            stream << ",\n";
        }

        stream << std::setw(2) << "" << kv.first << ": " << *kv.second;
    }

    stream << "\n}";
    return stream;
}

SymbolScope::SymbolScope()
        : m_tables{} {}

void SymbolScope::enter(SymbolTable &symbols) {
    m_tables.push_back(&symbols);
}

void SymbolScope::leave(SymbolTable &symbols) {
    if (&current() != &symbols) {
        throw FatalError(
                "Attempted to leave different scope from current scope");
    }
    m_tables.pop_back();
}

Symbol::unowned_ptr SymbolScope::lookup(Token const &ident) const {
    for (auto it = m_tables.rbegin(); it != m_tables.rend(); it++) {
        SymbolTable const &table = **it;
        Symbol::unowned_ptr symbol = table.lookup(ident);
        
        if (symbol != nullptr) {
            return symbol;
        }
    }

    std::stringstream ss;
    ss << "`" << ident.lexeme() << "` is not defined in this scope";
    throw ParserError(ident.pos(), ss.str());
}
