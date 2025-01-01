#ifndef PIX_TYPE_CHECKER_HPP
#define PIX_TYPE_CHECKER_HPP

#include "visitor.hpp"

class TypeChecker : public AstVisitor {
public:
    TypeChecker() = default;

    Node &visit(ExpressionStatement &stmt) override;

    Node &visit(Call &expr) override;
    
    Node &visit(Variable &expr) override;

    Node &visit(Integer &expr) override;
};

#endif
