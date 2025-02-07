#include "type.hpp"

Type::Type()
        {}    

Type::unowned_ptr Type::IntType() {
    static NamedType type("int");
    return &type;
}

Type::unowned_ptr Type::BoolType() {
    static NamedType type("bool");
    return &type;
}

Type::unowned_ptr Type::WordType() {
    static NamedType type("word");
    return &type;
}

Type::unowned_ptr Type::VoidType() {
    static NamedType type("void");
    return &type;
}

std::ostream &operator <<(std::ostream &stream, Type const &type) {
    type.write(stream);
    return stream;
}

NamedType::NamedType(std::string const &name)
        : m_name{name} {}

JSONObject::ptr NamedType::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();
    object->add_key("name", std::make_unique<JSONString>(m_name));
    return object;
}

void NamedType::write(std::ostream &stream) const {
    stream << m_name;
}

FunctionType::FunctionType(std::vector<Type::unowned_ptr> params, 
                           Type::unowned_ptr ret_type)
        : m_param_types{params}, m_ret_type{ret_type} {}

JSONObject::ptr FunctionType::to_json() const {
    JSONObject::ptr object = std::make_unique<JSONObject>();
    object->add_key("return-type", m_ret_type->to_json());
    return object;
}

void FunctionType::write(std::ostream &stream) const {
    stream << "(";

    bool first = true;
    for (Type::unowned_ptr const &type : m_param_types) {
        if (first) {
            first = false;
        } else {
            stream << ", ";
        }
        stream << *type;
    }

    stream << ") -> " << *m_ret_type;
}
