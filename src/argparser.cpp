#include <argparser.hpp>
#include <stdexcept>
#include <sstream>
#include <iostream>

Argument::Argument(void *option, std::string const &name, 
                   ArgType type, std::string const &init)
        : m_option{option}, m_name{name}, m_type{type}, 
          m_init{init}, m_value{} {}

void Argument::set_value(std::string const &value) {
    if (!m_value.empty()) {
        std::stringstream ss;
        ss << "Option `" << m_name << "` was passed twice";
        throw std::runtime_error(ss.str());
    }

    m_value = value;
}

void Argument::fill_option() {
    if (m_type == ArgType::String && m_init.empty() && m_value.empty()) {
        std::stringstream ss;
        ss << "Required option `" << m_name << "` was not passed";
        throw std::runtime_error(ss.str());
    }

    std::string const &value = m_value.empty() ? m_init : m_value;

    if (m_type == ArgType::String) {
        std::string &option = *reinterpret_cast<std::string *>(m_option);
        option = value;
    } else {
        throw std::runtime_error("not supported");
    }
}

ArgParser::ArgParser()
        : m_positional_args{} {}

void ArgParser::add_positional(void *option, std::string const &name, 
                               ArgType type, std::string const &init) {
    m_positional_args.emplace_back(option, name, type, init);
}

void ArgParser::parse(std::size_t argc, char *argv[]) {
    for (std::size_t i = 0; i < argc - 1; i++) {
        std::string arg = argv[i + 1];
        
        if (i < m_positional_args.size()) {
            m_positional_args[i].set_value(arg);
        } else {
            std::stringstream ss;
            ss << "Unexpected argument: `" << arg << "`";
            throw std::runtime_error(ss.str());
        }
    }

    for (Argument &arg : m_positional_args) {
        arg.fill_option();
    }
}
