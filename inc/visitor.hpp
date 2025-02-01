#ifndef PIX_VISITOR_HPP
#define PIX_VISITOR_HPP

#include "ast-forward.hpp"

class AstVisitor {
public:
    AstVisitor() = default;

    virtual ~AstVisitor() = default;

    virtual Node &default_action(Node &node);

    virtual Node &visit(Program &program);

    virtual Node &visit(VariableDeclaration &decl);

    virtual Node &visit(FunctionDeclaration &decl);

    virtual Node &visit(NamedTypeAnnotation &anno);

    virtual Node &visit(ExpressionStatement &stmt);

    virtual Node &visit(Call &expr);

    virtual Node &visit(Variable &expr);

    virtual Node &visit(Integer &expr);
};

#endif
