#include "visitor.hpp"
#include "symbol-table.hpp"

class SymbolResolver : public AstVisitor {
public:
    SymbolResolver();

    Node &visit(Program &program) override;

    Node &visit(VariableDeclaration &decl) override;

    Node &visit(FunctionDeclaration &decl) override;

    Node &visit(NamedTypeAnnotation &anno) override;

    Node &visit(ScopedBlockStatement &stmt) override;

private:
    void declare_basic_type(std::string const &name, Type::unowned_ptr type);

    void declare_basic_function(std::string const &name, 
                                Type::unowned_ptr param_type, 
                                Type::unowned_ptr ret_type, 
                                ECallFunction ecall);

    SymbolScope m_scope;
};
