#include "ast.hpp"
#include "error.hpp"
#include <sstream>
#include <utility>

std::string const &to_string(NodeKind kind) {
    static std::unordered_map<NodeKind, std::string> const map = {
        { NodeKind::None, "<none>" },
        { NodeKind::ExpressionStatement, "expression-statement" },
        { NodeKind::Call, "call" },
        { NodeKind::Variable, "variable" },
        { NodeKind::Integer, "integer" }
    };

    auto const &it = map.find(kind);
    if (it == map.end()) {
        std::stringstream ss;
        ss << "Unmapped tokenkind: " << static_cast<int>(kind);
        throw FatalError(ss.str());
    }

    return it->second;
}

std::ostream &operator <<(std::ostream &stream, NodeKind kind) {
    stream << to_string(kind);
    return stream;
}

Node::Node() 
        {}

Statement::Statement() 
        {}

JSON::ptr Statement::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();
    object->add_key("kind", std::make_unique<JSONString>(to_string(kind())));
    add_json_attributes(*object);
    return object;
}

Expression::Expression()
        : m_type{nullptr} {}

JSON::ptr Expression::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();

    object->add_key("kind", std::make_unique<JSONString>(to_string(kind())));
    if (m_type != nullptr) {
        object->add_key("type", m_type->to_json());
    }
    add_json_attributes(*object);

    return object;
}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr)
        : m_expr{std::move(expr)} {}

Type::unowned_ptr ExpressionStatement::check_type() {
    m_expr->check_type();
    return Type::VoidType();
}

void ExpressionStatement::add_json_attributes(JSONObject &object) const {
    object.add_key("expr", m_expr->to_json());
}

Call::Call(Expression::ptr func, std::vector<Expression::ptr> args)
        : m_func{std::move(func)}, m_args{std::move(args)} {}

Type::unowned_ptr Call::check_type() {
    m_func->check_type();

    for (Expression::ptr const &expr : m_args) {
        expr->check_type();
    }

    return Type::VoidType(); /* todo */
}

void Call::add_json_attributes(JSONObject &object) const {
    object.add_key("function", m_func->to_json());

    JSONList::ptr list = std::make_unique<JSONList>();
    for (Expression::ptr const &expr : m_args) {
        list->add(expr->to_json());
    }

    object.add_key("args", std::move(list));
}

Variable::Variable(Token const &ident)
        : m_ident{ident} {}

Type::unowned_ptr Variable::check_type() {
    return Type::VoidType();
}

void Variable::add_json_attributes(JSONObject &object) const {
    object.add_key("identifier", 
                    std::make_unique<JSONString>(m_ident.lexeme()));
}
 
Integer::Integer(Token const &literal)
        : m_literal{literal} {}

Type::unowned_ptr Integer::check_type() {
    return (m_type = Type::IntType());
}

void Integer::add_json_attributes(JSONObject &object) const {
    object.add_key("value", std::make_unique<JSONInteger>(m_literal));
}
