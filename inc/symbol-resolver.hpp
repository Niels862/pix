#include "visitor.hpp"
#include "symbol-table.hpp"

class SymbolResolver : public AstVisitor {
public:
    SymbolResolver();

    Node &visit(Program &program);

    Node &visit(FunctionDeclaration &decl);

private:
    SymbolTable::unowned_ptr scope;
};
