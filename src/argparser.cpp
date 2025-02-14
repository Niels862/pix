#include <argparser.hpp>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <string_view>

Argument::Argument()
        : m_option{nullptr}, m_name{}, m_type{ArgType::Invalid},
          m_init{}, m_value{} {}

Argument::Argument(void *option, std::string const &name, 
                   ArgType type, std::string const &init)
        : m_option{option}, m_name{name}, m_type{type}, 
          m_init{init}, m_value{} {}

void Argument::parse(ArgParser &args) {
    if (m_type == ArgType::Flag) {
        set_value("T");
        m_value = "T";
    } else if (m_type == ArgType::String) {
        set_value(args.get_next_argument());
    } else if (m_type == ArgType::Integer) {
        set_value(args.get_next_argument());

        try {
            std::stoi(m_value);
        } catch (...) {
            throw std::runtime_error("Malformed integer argument: " + m_value);
        }
    } else {
        throw std::runtime_error("not supported");
    }
}

void Argument::set_value(std::string const &value) {
    if (!m_value.empty()) {
        std::stringstream ss;
        ss << "Option `" << m_name << "` was passed twice";
        throw std::runtime_error(ss.str());
    }

    m_value = value;
}

void Argument::fill_option() {
    if (m_type != ArgType::Flag && m_init.empty() && m_value.empty()) {
        std::stringstream ss;
        ss << "Required option `" << m_name << "` was not passed";
        throw std::runtime_error(ss.str());
    }

    std::string const &value = m_value.empty() ? m_init : m_value;

    if (m_type == ArgType::String) {
        std::string &option = *reinterpret_cast<std::string *>(m_option);
        option = value;
    } else if (m_type == ArgType::Flag) {
        bool &option = *reinterpret_cast<bool *>(m_option);
        option = !value.empty();
    } else if (m_type == ArgType::Integer) {
        int &option = *reinterpret_cast<int *>(m_option);
        try {
            option = std::stoi(value);
        } catch (...) {
            std::stringstream ss;
            ss << "Conversion error: `" << value << "` on `" << m_name << "`";
            throw std::runtime_error(ss.str());
        }
    }
}

ArgParser::ArgParser()
        : m_positional_args{}, m_keyword_args{} {}

void ArgParser::add_positional(void *option, std::string const &name, 
                               ArgType type, std::string const &init) {
    m_positional_args.emplace_back(option, name, type, init);
}

void ArgParser::add_keyword(void *option, std::string const &name, 
                            ArgType type, std::string const &init) {
    m_keyword_args[name] = Argument(option, name, type, init);
}

void ArgParser::parse(std::size_t argc, char *argv[]) {
    m_argc = argc;
    m_argv = argv;
    m_curr = 1;

    std::size_t pos = 0;

    for (; m_curr < m_argc; m_curr++) {
        std::string value = argv[m_curr];
        
        if (std::string_view(value).substr(0, 2) == "--") {
            std::string keyword = value.substr(2);

            auto it = m_keyword_args.find(keyword);

            if (it == m_keyword_args.end()) {
                std::stringstream ss;
                ss << "Unknown keyword argument: `" << m_argv[m_curr] << "`";
                throw std::runtime_error(ss.str());
            }

            it->second.parse(*this);
        } else {
            if (pos < m_positional_args.size()) {
                m_positional_args[pos].set_value(value);
                pos++;
            } else {
                std::stringstream ss;
                ss << "Unexpected argument: `" << m_argv[m_curr] << "`";
                throw std::runtime_error(ss.str());
            }
        }
    }

    for (Argument &arg : m_positional_args) {
        arg.fill_option();
    }

    for (auto &it : m_keyword_args) {
        it.second.fill_option();
    }
}

std::string ArgParser::get_next_argument() {
    if (m_curr + 1 >= m_argc) {
        throw std::runtime_error("Expected argument");
    }
    
    std::string value = m_argv[m_curr + 1];
    if (std::string_view(value).substr(0, 2) == "--") {
        throw std::runtime_error("Expected argument, but got " + value);
    }

    m_curr += 1;
    return value;
}