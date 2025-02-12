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

        stream << std::setw(2) << "" << kv.first << ": [" << *kv.second << "]";
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

void SymbolScope::lookup_definitions(Token const &ident, 
                                     std::vector<FunctionDefinition *> &defs) {
    for (auto it = m_tables.rbegin(); it != m_tables.rend(); it++) {
        SymbolTable const &table = **it;
        Symbol *symbol = table.lookup(ident);

        if (!symbol) {
            continue;
        }
        
        if (auto *func_symbol = dynamic_cast<FunctionSymbol *>(symbol)) {
            for (auto &def : func_symbol->definitions()) {
                defs.push_back(def.get());
            }
        } else {
            return;
        }
    }
}

void SymbolScope::declare(std::string const &ident, Symbol::ptr symbol) {
    std::string const lexeme = ident;
    Token const token(TextPosition(), TokenKind::Synthetic, std::move(lexeme));

    try {
        declare(token, std::move(symbol));
    } catch (ParserError const &e) {
        std::stringstream ss;
        ss << "Fatal: " << e.what();
        throw FatalError(ss.str());
    }
}

void SymbolScope::declare(Token const &ident, Symbol::ptr symbol) {
    if (current().defines(ident.lexeme())) {
        std::stringstream ss;
        ss << "`" << ident.lexeme() << "` was already declared in this scope";
        throw ParserError(ident.pos(), ss.str());
    }

    if (dynamic_cast<TypeSymbol *>(global().lookup(ident))) {
        std::stringstream ss;
        ss << "`" << ident.lexeme() << "` shadows a type";
        throw ParserError(ident.pos(), ss.str());
    }

    current().insert(ident.lexeme(), std::move(symbol));
}

FunctionDefinition &SymbolScope::declare_function(std::string const &ident, 
                                                  FunctionDefinition &&def) {
    std::string const lexeme = ident;
    Token const token(TextPosition(), TokenKind::Synthetic, std::move(lexeme));

    try {
        return declare_function(token, std::move(def));
    } catch (ParserError const &e) {
        std::stringstream ss;
        ss << "Fatal: " << e.what();
        throw FatalError(ss.str());
    }
}

FunctionDefinition &SymbolScope::declare_function(Token const &ident, 
                                                  FunctionDefinition &&def) {
    Symbol *symbol = current().lookup(ident);

    if (auto func_symbol = dynamic_cast<FunctionSymbol *>(symbol)) {
        func_symbol->add_definition(std::move(def));
        FunctionDefinition &def_inplace = *func_symbol->definitions().back();

        return def_inplace;
    } else if (symbol) {
        std::stringstream ss;
        ss << "`" << ident.lexeme() << "` was already declared in this scope";
        throw ParserError(ident.pos(), ss.str());
    } else {
        FunctionSymbol::ptr func_symbol = std::make_unique<FunctionSymbol>();
        func_symbol->add_definition(std::move(def));
        FunctionDefinition &def_inplace = *func_symbol->definitions().back();

        declare(ident, std::move(func_symbol));

        return def_inplace;
    }
}

