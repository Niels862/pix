#include "json.hpp"
#include "error.hpp"
#include "options.hpp"
#include <iomanip>

JSON::JSON()
        {} 

std::ostream &operator <<(std::ostream &stream, JSON const &json) {
    json.write(stream, 0);
    return stream;
}

JSONString::JSONString(std::string const &value)
        : m_value{value} {}

JSONString::ptr JSONString::Create(std::string const &value) {
    return std::make_unique<JSONString>(value);
}

void JSONString::write(std::ostream &stream, std::size_t) const {
    stream << "\"" << m_value << "\"";
}

JSONInteger::JSONInteger(int value)
        : m_value{value} {}

JSONInteger::ptr JSONInteger::Create(int value) {
    return std::make_unique<JSONInteger>(value);
}

void JSONInteger::write(std::ostream &stream, std::size_t) const {
    stream << m_value;
}

JSONObject::JSONObject() 
        : m_keys{} {}

JSONObject::ptr JSONObject::Create() {
    return std::make_unique<JSONObject>();
}

JSONObject &JSONObject::add_key(std::string const &key, JSON::ptr value) {
    for (auto const &kv : m_keys) {
        if (kv.first == key) {
            throw FatalError("add_key(): duplicate key: " + key);
        }
    }

    m_keys.emplace_back(key, std::move(value));

    return *this;
}

void JSONObject::write(std::ostream &stream, std::size_t depth) const {
    if (m_keys.empty()) {
        stream << "{ }";
        return;
    }
    
    stream << "{";

    bool first = true;
    for (auto const &kv : m_keys) {
        if (!first) {
            stream << ",\n";
        } else {
            stream << "\n";
        }
        stream << std::setw(options.json.spacing * (depth + 1)) << "" 
               << "\"" << kv.first << "\": ";
        kv.second->write(stream, depth + 1);
        first = false;
    }

    stream << "\n" << std::setw(options.json.spacing * depth) << "" << "}";
}

JSONList::JSONList()
        : m_list{} {}

JSONList::ptr JSONList::Create() {
    return std::make_unique<JSONList>();
}

void JSONList::add(JSON::ptr value) {
    m_list.push_back(std::move(value));
}

void JSONList::write(std::ostream &stream, std::size_t depth) const {
    if (m_list.empty()) {
        stream << "[ ]";
        return;
    }
    
    stream << "[";

    bool first = true;
    for (JSON::ptr const &value : m_list) {
        if (!first) {
            stream << ",\n";
        } else {
            stream << "\n";
        }
        stream << std::setw(options.json.spacing * (depth + 1)) << "";
        value->write(stream, depth + 1);
        first = false;
    }
    
    stream << "\n" << std::setw(options.json.spacing * depth) << "" << "]";
}
