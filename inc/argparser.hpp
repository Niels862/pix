#ifndef PIX_ARGPARSER_HPP
#define PIX_ARGPARSER_HPP

#include <vector>
#include <string>
#include <unordered_map>

enum class ArgType {
    Invalid,
    String, 
    Flag
};

class Argument {
public:
    Argument();

    Argument(void *option, std::string const &name, 
             ArgType type, std::string const &init = "");

    void parse();

    void set_value(std::string const &value);

    void fill_option();

private:    
    void *m_option;

    std::string m_name;

    ArgType m_type;

    std::string m_init;

    std::string m_value;
};

class ArgParser {
public:
    ArgParser();

    void add_positional(void *option, std::string const &name, 
                        ArgType type, std::string const &init = "");

    void add_keyword(void *option, std::string const &name,
                     ArgType type, std::string const &init = "");

    void parse(std::size_t arg, char *argv[]);

private:
    std::vector<Argument> m_positional_args;

    std::unordered_map<std::string, Argument> m_keyword_args;
};

#endif
