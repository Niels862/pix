#include "symbol-table.hpp"
#include <iomanip>

SymbolTable::SymbolTable()
        : m_map{} {}

void SymbolTable::insert(std::string const &ident, Symbol::ptr symbol) {
    m_map[ident] = std::move(symbol);
}

Symbol::unowned_ptr SymbolTable::lookup(std::string const &ident) const {
    auto iter = m_map.find(ident);

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
