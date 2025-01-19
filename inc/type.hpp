#ifndef PIX_TYPE_HPP
#define PIX_TYPE_HPP

#include "json.hpp"
#include <string>
#include <vector>
#include <memory>

class Type {
public:
    Type();

    virtual ~Type() = default;

    using ptr = std::unique_ptr<Type>;    

    using unowned_ptr = Type *;

    static Type::unowned_ptr IntType();
    
    static Type::unowned_ptr VoidType();

    virtual JSONObject::ptr to_json() const = 0;
};

class NamedType : public Type {
public:
    NamedType(std::string const &name);

    JSONObject::ptr to_json() const;

    using ptr = std::unique_ptr<NamedType>;
    using unowned_ptr = NamedType *;

private:
    std::string m_name;
};

class FunctionType : public Type {
public:
    FunctionType();

    JSONObject::ptr to_json() const;

    using ptr = std::unique_ptr<FunctionType>;    
    using unowned_ptr = FunctionType *;

private:
    std::vector<Type::ptr> m_params;
    Type::ptr m_ret_type;
};

#endif
