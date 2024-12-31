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

    virtual ~JSON() {}

    virtual void write(std::ostream &stream, std::size_t depth) const = 0;

    friend std::ostream &operator <<(std::ostream &stream, JSON const &json);

    using ptr = std::unique_ptr<JSON>;
};

class JSONString : public JSON {
public:
    JSONString(std::string const &value);

    virtual void write(std::ostream &stream, std::size_t depth) const;

    using ptr = std::unique_ptr<JSONString>;
private:
    std::string m_value;
};

class JSONInteger : public JSON {
public:
    JSONInteger(int value);

    virtual void write(std::ostream &stream, std::size_t depth) const;

    using ptr = std::unique_ptr<JSONInteger>;
private:
    int m_value;
};

class JSONObject : public JSON {
public:
    JSONObject();

    JSONObject &add_key(std::string const &key, JSON::ptr value);

    virtual void write(std::ostream &stream, std::size_t depth) const;

    using ptr = std::unique_ptr<JSONObject>;
private:
    std::unordered_map<std::string, JSON::ptr> m_keys;
};

class JSONList : public JSON {
public:
    JSONList();

    void add(JSON::ptr value);

    virtual void write(std::ostream &stream, std::size_t depth) const;

    using ptr = std::unique_ptr<JSONList>;
private:
    std::vector<JSON::ptr> m_list;
};

#endif
