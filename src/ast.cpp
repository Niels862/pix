#include "ast.hpp"
#include "error.hpp"
#include <sstream>
#include <utility>

std::string const &to_string(NodeKind kind) {
    static std::unordered_map<NodeKind, std::string> const map = {
        { NodeKind::None, "<none>" },
        { NodeKind::Program, "program" },
        { NodeKind::ExpressionStatement, "expression-statement" },
        { NodeKind::Call, "call" },
        { NodeKind::Variable, "variable" },
        { NodeKind::Integer, "integer" }
    };

    auto const &it = map.find(kind);
    if (it == map.end()) {
        std::stringstream ss;
        ss << "Unmapped nodekind: " << static_cast<int>(kind);
        throw FatalError(ss.str());
    }

    return it->second;
}

std::ostream &operator <<(std::ostream &stream, NodeKind kind) {
    stream << to_string(kind);
    return stream;
}

Node::Node() 
        : m_type{} {}

Statement::Statement() 
        : Node{} {}

JSON::ptr Statement::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();
    object->add_key("kind", std::make_unique<JSONString>(to_string(kind())));
    add_json_attributes(*object);
    return object;
}

Expression::Expression() 
        : Node{} {}

JSON::ptr Expression::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();

    object->add_key("kind", std::make_unique<JSONString>(to_string(kind())));
    if (m_type != nullptr) {
        object->add_key("type", m_type->to_json());
    }
    add_json_attributes(*object);

    return object;
}

Program::Program(std::vector<Statement::ptr> stmts)
        : Node{}, m_stmts{std::move(stmts)}, m_scope{} {}

JSON::ptr Program::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();

    object->add_key("kind", std::make_unique<JSONString>(to_string(kind())));

    JSONList::ptr list = std::make_unique<JSONList>();
    for (Statement::ptr const &expr : m_stmts) {
        list->add(expr->to_json());
    }

    object->add_key("stmts", std::move(list));

    return object;
}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr)
        : Statement{}, m_expr{std::move(expr)} {}

void ExpressionStatement::add_json_attributes(JSONObject &object) const {
    object.add_key("expr", m_expr->to_json());
}

Call::Call(Token const &func, std::vector<Expression::ptr> args)
        : Expression{}, m_func{func}, m_args{std::move(args)}, 
          m_called{nullptr} {}

void Call::add_json_attributes(JSONObject &object) const {
    object.add_key("function", std::make_unique<JSONString>(m_func.lexeme()));

    JSONList::ptr list = std::make_unique<JSONList>();
    for (Expression::ptr const &expr : m_args) {
        list->add(expr->to_json());
    }

    object.add_key("args", std::move(list));
}

Variable::Variable(Token const &ident)
        : Expression{}, m_ident{ident} {}

void Variable::add_json_attributes(JSONObject &object) const {
    object.add_key("identifier", 
                    std::make_unique<JSONString>(m_ident.lexeme()));
}
 
Integer::Integer(Token const &literal)
        : Expression{}, m_literal{literal} {}

void Integer::add_json_attributes(JSONObject &object) const {
    int value = std::stoi(m_literal.lexeme());
    object.add_key("value", std::make_unique<JSONInteger>(value));
}
