#include "ast.hpp"
#include "error.hpp"
#include <sstream>
#include <utility>

std::string const &to_string(NodeKind kind) {
    static std::unordered_map<NodeKind, std::string> const map = {
        { NodeKind::None, "<none>" },
        { NodeKind::Program, "program" },
        { NodeKind::VariableDeclaration, "variable-declaration" },
        { NodeKind::FunctionDeclaration, "function-declaration" },
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
    JSONObject::ptr object = JSONObject::Create();
    object->add_key("kind", JSONString::Create(to_string(kind())));
    add_json_attributes(*object);
    return object;
}

Expression::Expression() 
        : Node{} {}

JSON::ptr Expression::to_json() const {
    JSONObject::ptr object = JSONObject::Create();

    object->add_key("kind", JSONString::Create(to_string(kind())));
    if (m_type != nullptr) {
        object->add_key("type", m_type->to_json());
    }
    add_json_attributes(*object);

    return object;
}

Program::Program(std::vector<Statement::ptr> stmts)
        : Node{}, m_stmts{std::move(stmts)}, m_symbols{} {}

JSON::ptr Program::to_json() const {
    JSONObject::ptr object = JSONObject::Create();

    object->add_key("kind", JSONString::Create(to_string(kind())));

    JSONList::ptr list = JSONList::Create();
    for (Statement::ptr const &expr : m_stmts) {
        list->add(expr->to_json());
    }

    object->add_key("stmts", std::move(list));

    return object;
}

VariableDeclaration::VariableDeclaration(Token const &ident)
        : m_ident{ident} {}

void VariableDeclaration::add_json_attributes(JSONObject &object) const {
    object.add_key("identifier", JSONString::Create(m_ident.lexeme()));
}

FunctionDeclaration::FunctionDeclaration(Token const &func, 
                                         std::vector<VariableDeclaration::ptr> params, 
                                         std::vector<Statement::ptr> body)
        : m_func{func}, m_params{std::move(params)}, m_body{std::move(body)} {}

void FunctionDeclaration::add_json_attributes(JSONObject &object) const {
    object.add_key("function", JSONString::Create(m_func.lexeme()));
    
    JSONList::ptr params = JSONList::Create();
    for (VariableDeclaration::ptr const &param : m_params) {
        params->add(param->to_json());
    }

    object.add_key("params", std::move(params));

    JSONList::ptr body = JSONList::Create();
    for (Statement::ptr const &stmt : m_body) {
        body->add(stmt->to_json());
    }

    object.add_key("body", std::move(body));
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
    object.add_key("function", JSONString::Create(m_func.lexeme()));

    JSONList::ptr list = JSONList::Create();
    for (Expression::ptr const &expr : m_args) {
        list->add(expr->to_json());
    }

    object.add_key("args", std::move(list));
}

Variable::Variable(Token const &ident)
        : Expression{}, m_ident{ident} {}

void Variable::add_json_attributes(JSONObject &object) const {
    object.add_key("identifier", JSONString::Create(m_ident.lexeme()));
}
 
Integer::Integer(Token const &literal)
        : Expression{}, m_literal{literal} {}

void Integer::add_json_attributes(JSONObject &object) const {
    int value = std::stoi(m_literal.lexeme());
    object.add_key("value", JSONInteger::Create(value));
}
