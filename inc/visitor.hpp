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

    virtual Node &visit(ScopedBlockStatement &stmt);

    virtual Node &visit(ExpressionStatement &stmt);

    virtual Node &visit(ReturnStatement &stmt);

    virtual Node &visit(IfElseStatement &stmt);

    virtual Node &visit(WhileStatement &stmt);

    virtual Node &visit(BreakStatement &stmt);

    virtual Node &visit(ContinueStatement &stmt);

    virtual Node &visit(UnaryExpression &expr);

    virtual Node &visit(BinaryExpression &expr);

    virtual Node &visit(Call &expr);

    virtual Node &visit(Variable &expr);

    virtual Node &visit(Integer &expr);

    virtual Node &visit(BooleanLiteral &literal);
};

#endif
