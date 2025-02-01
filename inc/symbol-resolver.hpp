#include "visitor.hpp"
#include "symbol-table.hpp"

class SymbolResolver : public AstVisitor {
public:
    SymbolResolver();

    Node &visit(Program &program) override;

    Node &visit(VariableDeclaration &decl) override;

    Node &visit(FunctionDeclaration &decl) override;

    Node &visit(NamedTypeAnnotation &anno) override;

private:
    SymbolScope m_scope;
};
