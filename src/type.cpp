#include "type.hpp"

Type::Type()
        {}    

Type::unowned_ptr Type::IntType() {
    static NamedType type("int");
    return &type;
}

Type::unowned_ptr Type::VoidType() {
    static NamedType type("void");
    return &type;
}

NamedType::NamedType(std::string const &name)
        : m_name{name} {}

JSONObject::ptr NamedType::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();
    object->add_key("name", std::make_unique<JSONString>(m_name));
    return object;
}

FunctionType::FunctionType()
        {}

JSONObject::ptr FunctionType::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();
    object->add_key("return-type", m_ret_type->to_json());
    return object;
}
