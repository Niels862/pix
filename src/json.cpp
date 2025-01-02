#include "json.hpp"
#include "error.hpp"
#include <iomanip>

JSON::JSON()
        {} 

std::ostream &operator <<(std::ostream &stream, JSON const &json) {
    json.write(stream, 0);
    return stream;
}

JSONString::JSONString(std::string const &value)
        : m_value{value} {}

void JSONString::write(std::ostream &stream, std::size_t) const {
    stream << "\"" << m_value << "\"";
}

JSONInteger::JSONInteger(int value)
        : m_value{value} {}

void JSONInteger::write(std::ostream &stream, std::size_t) const {
    stream << m_value;
}

JSONObject::JSONObject() 
        : m_keys{} {}

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
    stream << "{";

    bool first = true;
    for (auto const &kv : m_keys) {
        if (!first) {
            stream << ",\n";
        } else {
            stream << "\n";
        }
        stream << std::setw(PrettyPrintSpacing * (depth + 1)) << "" 
               << "\"" << kv.first << "\": ";
        kv.second->write(stream, depth + 1);
        first = false;
    }

    stream << "\n" << std::setw(PrettyPrintSpacing * depth) << "" << "}";
}

JSONList::JSONList()
        : m_list{} {}

void JSONList::add(JSON::ptr value) {
    m_list.push_back(std::move(value));
}

void JSONList::write(std::ostream &stream, std::size_t depth) const {
    stream << "[";

    bool first = true;
    for (JSON::ptr const &value : m_list) {
        if (!first) {
            stream << ",\n";
        } else {
            stream << "\n";
        }
        stream << std::setw(PrettyPrintSpacing * (depth + 1)) << "";
        value->write(stream, depth + 1);
        first = false;
    }
    
    stream << "\n" << std::setw(PrettyPrintSpacing * depth) << "" << "]";
}
