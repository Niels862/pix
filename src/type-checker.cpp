#include "type-checker.hpp"
#include "error.hpp"
#include "parser.hpp" // for ParserError
#include <sstream>
#include <string>

Node &TypeChecker::default_action(Node &node) {
    std::stringstream ss;
    ss << "TypeChecker(): unimplemented action: " << node.kind();
    throw FatalError(ss.str());
}

Node &TypeChecker::visit(Program &program) {
    m_scope.enter(program.symbols());

    for (Statement::ptr &stmt : program.stmts()) {
        stmt->accept(*this);
    }

    m_scope.leave(program.symbols());

    return program;
}

Node &TypeChecker::visit(ParameterDeclaration &decl) {
    return decl;
}

Node &TypeChecker::visit(FunctionDeclaration &decl) {
    FunctionDeclaration::unowned_ptr prev_func = m_curr_function;

    m_curr_function = &decl;
    m_scope.enter(decl.symbols());

    for (Statement::ptr &stmt : decl.body()) {
        stmt->accept(*this);
    }

    m_scope.leave(decl.symbols());
    m_curr_function = prev_func;

    return decl;
}

Node &TypeChecker::visit(VariableDeclaration &decl) {
    decl.value()->accept(*this);
    coerce_types(decl.value(), decl.annotation()->type(), 
                 decl.pos(), "In initial value of " + decl.ident());

    return decl;
}

Node &TypeChecker::visit(ScopedBlockStatement &stmt) {
    m_scope.enter(stmt.symbols());
    
    for (Statement::ptr &substmt : stmt.body()) {
        substmt->accept(*this);
    }

    m_scope.leave(stmt.symbols());

    return stmt;
}

Node &TypeChecker::visit(ExpressionStatement &stmt) {
    stmt.expr()->accept(*this);
    return stmt;
}

Node &TypeChecker::visit(AssignStatement &stmt) {
    stmt.target()->accept(*this);
    stmt.value()->accept(*this);
    return stmt;
}

Node &TypeChecker::visit(ReturnStatement &stmt) {
    stmt.value()->accept(*this);

    std::stringstream ss;
    coerce_types(stmt.value(), m_curr_function->definition().type()->ret_type(), 
                 stmt.pos(), ss.str());
                 
    return stmt;
}

Node &TypeChecker::visit(IfElseStatement &stmt) {
    stmt.condition()->accept(*this);
    coerce_types(stmt.condition(), Type::BoolType(), stmt.condition()->pos(), 
                 "In if-statement condition");

    stmt.then_stmt()->accept(*this);
    stmt.else_stmt()->accept(*this);

    return stmt;
}

Node &TypeChecker::visit(WhileStatement &stmt) {
    stmt.condition()->accept(*this);
    coerce_types(stmt.condition(), Type::BoolType(), stmt.condition()->pos(),
                 "In while-statement condition");

    stmt.loop_stmt()->accept(*this);

    return stmt;
}

Node &TypeChecker::visit(BreakStatement &stmt) {
    return stmt;
}

Node &TypeChecker::visit(ContinueStatement &stmt) {
    return stmt;
}

Node &TypeChecker::visit(UnaryExpression &expr) {
    return expr;
}

Node &TypeChecker::visit(BinaryExpression &expr) {
    expr.left()->accept(*this);
    expr.right()->accept(*this);

    std::stringstream ss;
    ss << "On operation `" << expr.op().lexeme() << "`";

    switch (expr.op().kind()) {
        case TokenKind::Plus:
        case TokenKind::Minus:
        case TokenKind::Times:
        case TokenKind::FloorDiv:
        case TokenKind::Modulo:
            coerce_types(expr.left(), Type::IntType(), 
                         expr.left()->pos(), ss.str());
            coerce_types(expr.right(), Type::IntType(), 
                         expr.right()->pos(), ss.str());
            expr.set_type(Type::IntType());
            break;

        case TokenKind::DoubleEquals:
        case TokenKind::NotEquals:
        case TokenKind::LessThan:
        case TokenKind::LessEquals:
        case TokenKind::GreaterThan:
        case TokenKind::GreaterEquals:
            coerce_types(expr.left(), Type::IntType(), 
                         expr.left()->pos(), ss.str());
            coerce_types(expr.right(), Type::IntType(), 
                         expr.right()->pos(), ss.str());
            expr.set_type(Type::BoolType());
            break;

        default:
            throw FatalError("Unhandled operation: " + expr.op().lexeme());
    }

    return expr;
}

Node &TypeChecker::visit(Call &expr) {
    static std::vector<FunctionDefinition *> defs;
    
    for (Expression::ptr &expr : expr.args()) {
        expr->accept(*this);
    }
    
    defs.clear();
    m_scope.lookup_definitions(expr.func(), defs);

    if (defs.empty()) {
        std::stringstream ss;
        ss << "`" << expr.func().lexeme()
           << "` is not declared as a function in this scope";
        throw ParserError(expr.pos(), ss.str());
    }

    for (auto &def : defs) {
        auto &param_types = def->type()->param_types();

        if (param_types.size() != expr.args().size()) {
            def = nullptr;
            continue;
        }

        for (std::size_t i = 0; i < param_types.size(); i++) {
            if (param_types[i] != expr.args()[i]->type()) {
                def = nullptr;
            }
        }
    }

    FunctionDefinition *def = nullptr;
    for (auto const &candidate : defs) {
        if (!candidate) {
            continue;
        }
        if (def) {
            throw ParserError(expr.pos(), "Multiple definitions, todo");
        } else {
            def = candidate;
        }
    }

    if (!def) {
        std::stringstream ss;
        ss << "No definition of `" << expr.func().lexeme()
           << "` matched the argument types: (";
        bool first = true;
        for (Expression::ptr &arg : expr.args()) {
            if (first) {
                first = false;
            } else {
                ss << ", ";
            }
            ss << *arg->type();
        }
        ss << ")";
        throw ParserError(expr.pos(), ss.str());
    }

    for (std::size_t i = 0; i < expr.args().size(); i++) {
        coerce_types(expr.args()[i], def->type()->param_types()[i], 
                     expr.pos(), "In call to " + expr.func().lexeme());
    }

    expr.set_called(*def);
    expr.set_type(def->type()->ret_type());
    
    return expr;
}

Node &TypeChecker::visit(Variable &expr) {
    Symbol::unowned_ptr symbol = m_scope.lookup(expr.ident());
    VariableSymbol::unowned_ptr var_symbol 
            = dynamic_cast<VariableSymbol *>(symbol);

    if (!var_symbol) {
        throw ParserError(expr.pos(), expr.ident() + " is not a variable");
    }

    expr.set_type(var_symbol->type());
    return expr;
}

Node &TypeChecker::visit(Integer &expr) { 
    expr.set_type(Type::IntType());
    return expr;
}

Node &TypeChecker::visit(BooleanLiteral &expr) {
    expr.set_type(Type::BoolType());
    return expr;
}

void TypeChecker::coerce_types(Expression::ptr &target, 
                               Type::unowned_ptr expected, 
                               TextPosition const &pos,
                               std::string const &context) {
    if (target->type() == expected) { // TODO FIXME works for now but fix
        return;
    }

    /* Implicitly and trivially convertible */
    if (target->type() == Type::WordType() || expected == Type::WordType()) {
        return;
    }

    std::stringstream ss;
    ss << context << ": cannot use value of type `" << *target->type() 
       << "` as `" << *expected << "`";
    throw ParserError(pos, ss.str());
}
