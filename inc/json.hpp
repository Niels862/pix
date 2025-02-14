#ifndef PIX_JSON_HPP
#define PIX_JSON_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

class JSON {
public:
    JSON();

    virtual ~JSON() = default;

    virtual void write(std::ostream &stream, std::size_t depth) const = 0;

    friend std::ostream &operator <<(std::ostream &stream, JSON const &json);

    using ptr = std::unique_ptr<JSON>;
};

class JSONString : public JSON {
public:
    JSONString(std::string const &value);

    using ptr = std::unique_ptr<JSONString>;

    static JSONString::ptr Create(std::string const &value);

    virtual void write(std::ostream &stream, std::size_t depth) const;

private:
    std::string m_value;
};

class JSONInteger : public JSON {
public:
    JSONInteger(int value);

    using ptr = std::unique_ptr<JSONInteger>;

    static JSONInteger::ptr Create(int value);

    virtual void write(std::ostream &stream, std::size_t depth) const;

private:
    int m_value;
};

class JSONObject : public JSON {
public:
    JSONObject();

    using ptr = std::unique_ptr<JSONObject>;

    static JSONObject::ptr Create();

    JSONObject &add_key(std::string const &key, JSON::ptr value);

    virtual void write(std::ostream &stream, std::size_t depth) const;

private:
    std::vector<std::pair<std::string, JSON::ptr>> m_keys;
};

class JSONList : public JSON {
public:
    JSONList();

    using ptr = std::unique_ptr<JSONList>;

    static JSONList::ptr Create();

    void add(JSON::ptr value);

    virtual void write(std::ostream &stream, std::size_t depth) const;

private:
    std::vector<JSON::ptr> m_list;
};

#endif
