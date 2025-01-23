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

    virtual void write(std::ostream &stream) const = 0;

    friend std::ostream &operator <<(std::ostream &stream, Type const &type);
};

class NamedType : public Type {
public:
    NamedType(std::string const &name);

    JSONObject::ptr to_json() const;

    void write(std::ostream &stream) const override;

    using ptr = std::unique_ptr<NamedType>;
    using unowned_ptr = NamedType *;

private:
    std::string m_name;
};

class FunctionType : public Type {
public:
    FunctionType(std::vector<Type::unowned_ptr> params, 
                 Type::unowned_ptr ret_type);

    JSONObject::ptr to_json() const;

    void write(std::ostream &stream) const override;

    std::vector<Type::unowned_ptr> const &param_types() 
            { return m_param_types; }

    Type::unowned_ptr ret_type() { return m_ret_type; }

    using ptr = std::unique_ptr<FunctionType>;    
    using unowned_ptr = FunctionType *;

private:
    std::vector<Type::unowned_ptr> m_param_types;
    Type::unowned_ptr m_ret_type;
};

#endif
