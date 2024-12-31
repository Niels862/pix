#include "ast.hpp"
#include <utility>

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr)
        : m_expr{std::move(expr)} {}

JSON::ptr ExpressionStatement::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();
    object->add_key("expr", m_expr->to_json());
    return object;
}

Call::Call(Expression::ptr func, std::vector<Expression::ptr> args)
        : m_func{std::move(func)}, m_args{std::move(args)} {}

JSON::ptr Call::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();
    object->add_key("function", m_func->to_json());
    
    JSONList::ptr list = std::make_unique<JSONList>();
    for (Expression::ptr const &expr : m_args) {
        list->add(expr->to_json());
    }

    object->add_key("args", std::move(list));
    return object;
}

Variable::Variable(Token const &ident)
        : m_ident{ident} {}

JSON::ptr Variable::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();
    JSONString::ptr string = std::make_unique<JSONString>(m_ident.lexeme());
    object->add_key("identifier", std::move(string));
    return object;
}
 
Integer::Integer(Token const &literal)
        : m_literal{literal} {}

JSON::ptr Integer::to_json() const {
    return std::make_unique<JSONInteger>(m_literal);
}
