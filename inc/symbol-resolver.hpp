#include "visitor.hpp"

class SymbolResolver : public AstVisitor {
public:
    SymbolResolver();

    Node &default_action(Node &node);

    Node &visit(Program &program);
};
