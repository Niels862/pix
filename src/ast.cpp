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
        { NodeKind::NamedTypeAnnotation, "named-type-annotation" },
        { NodeKind::ScopedBlockStatement, "scoped-block-statement" },
        { NodeKind::ExpressionStatement, "expression-statement" },
        { NodeKind::ReturnStatement, "return-statement" },
        { NodeKind::IfElseStatement, "if-else-statement" },
        { NodeKind::WhileStatement, "while-statement" },
        { NodeKind::BreakStatement, "break-statement" },
        { NodeKind::ContinueStatement, "continue-statement" },
        { NodeKind::UnaryExpression, "unary-expression" },
        { NodeKind::BinaryExpression, "binary-expression" },
        { NodeKind::Call, "call" },
        { NodeKind::Variable, "variable" },
        { NodeKind::Integer, "integer" },
        { NodeKind::BooleanLiteral, "boolean-literal" }
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

template <typename T>
JSONList::ptr Node::to_json_list(std::vector<T> const &list) {
    JSONList::ptr json_list = JSONList::Create();

    for (T const &entry : list) {
        json_list->add(entry->to_json());
    }

    return json_list;
}

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

TypeAnnotation::TypeAnnotation()
        {}

JSON::ptr TypeAnnotation::to_json() const {
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
    object->add_key("stmts", Node::to_json_list(m_stmts));

    return object;
}

VariableDeclaration::VariableDeclaration(Token const &ident, 
                                         TypeAnnotation::ptr type)
        : m_ident{ident}, m_annotation{std::move(type)} {}

void VariableDeclaration::add_json_attributes(JSONObject &object) const {
    object.add_key("identifier", JSONString::Create(m_ident.lexeme()));
    object.add_key("annotation", m_annotation->to_json());
}

FunctionDeclaration::FunctionDeclaration(Token const &func, 
                                         std::vector<VariableDeclaration::ptr> params, 
                                         TypeAnnotation::ptr ret_type_annotation,
                                         std::vector<Statement::ptr> body)
        : m_func{func}, m_params{std::move(params)}, 
          m_ret_type_annotation{std::move(ret_type_annotation)}, 
          m_body{std::move(body)} {}

void FunctionDeclaration::add_json_attributes(JSONObject &object) const {
    object.add_key("function", JSONString::Create(m_func.lexeme()));
    object.add_key("params", Node::to_json_list(m_params));
    object.add_key("body", Node::to_json_list(m_body));
}

NamedTypeAnnotation::NamedTypeAnnotation(Token const &ident)
        : TypeAnnotation{}, m_ident{ident} {}

void NamedTypeAnnotation::add_json_attributes(JSONObject &object) const {
    object.add_key("identifier", JSONString::Create(m_ident.lexeme()));
}

ScopedBlockStatement::ScopedBlockStatement(std::vector<Statement::ptr> body)
        : m_body{std::move(body)}, m_symbols{} {}

void ScopedBlockStatement::add_json_attributes(JSONObject &object) const {
    object.add_key("body", Node::to_json_list(m_body));
}

ExpressionStatement::ExpressionStatement(Expression::ptr expr)
        : Statement{}, m_expr{std::move(expr)} {}

void ExpressionStatement::add_json_attributes(JSONObject &object) const {
    object.add_key("expr", m_expr->to_json());
}

ReturnStatement::ReturnStatement(Expression::ptr value)
        : m_value{std::move(value)} {}

void ReturnStatement::add_json_attributes(JSONObject &object) const {
    object.add_key("value", m_value->to_json());
}

IfElseStatement::IfElseStatement(Expression::ptr condition, 
                                 Statement::ptr then_stmt, 
                                 Statement::ptr else_stmt)
        : m_condition{std::move(condition)}, m_then_stmt{std::move(then_stmt)}, 
          m_else_stmt{std::move(else_stmt)} {}

void IfElseStatement::add_json_attributes(JSONObject &object) const {
    object.add_key("condition", m_condition->to_json());
    object.add_key("then-stmt", m_then_stmt->to_json());
    object.add_key("else-stmt", m_else_stmt->to_json());
}

WhileStatement::WhileStatement(Expression::ptr condition, 
                               Statement::ptr loop_stmt)
        : m_condition{std::move(condition)}, 
          m_loop_stmt{std::move(loop_stmt)} {}

void WhileStatement::add_json_attributes(JSONObject &object) const {
    object.add_key("condition", m_condition->to_json());
    object.add_key("loop-stmt", m_loop_stmt->to_json());
}

BreakStatement::BreakStatement(Token const &token)
        : m_token{token} {}

ContinueStatement::ContinueStatement(Token const &token)
        : m_token{token} {}

UnaryExpression::UnaryExpression(Token const &op, Expression::ptr operand)
        : m_op{op}, m_operand{std::move(operand)} {}

void UnaryExpression::add_json_attributes(JSONObject &object) const {
    object.add_key("operator", JSONString::Create(m_op.lexeme()));
    object.add_key("operand", m_operand->to_json());
}

BinaryExpression::BinaryExpression(Token const &op, Expression::ptr left, 
                                   Expression::ptr right)
        : m_op{op}, m_left{std::move(left)}, m_right{std::move(right)} {}

void BinaryExpression::add_json_attributes(JSONObject &object) const {
    object.add_key("operator", JSONString::Create(m_op.lexeme()));
    object.add_key("left", m_left->to_json());
    object.add_key("right", m_right->to_json());
}

Call::Call(Token const &func, std::vector<Expression::ptr> args)
        : Expression{}, m_func{func}, m_args{std::move(args)}, 
          m_called{nullptr} {}

void Call::add_json_attributes(JSONObject &object) const {
    object.add_key("function", JSONString::Create(m_func.lexeme()));
    object.add_key("args", Node::to_json_list(m_args));
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

BooleanLiteral::BooleanLiteral(Token const &literal)
        : Expression{}, m_literal{literal} {}

void BooleanLiteral::add_json_attributes(JSONObject &object) const {
    object.add_key("value", JSONString::Create(m_literal.lexeme()));
}
