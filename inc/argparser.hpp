#ifndef PIX_ARGPARSER_HPP
#define PIX_ARGPARSER_HPP

#include <vector>
#include <string>
#include <unordered_map>

enum class ArgType {
    Invalid,
    String, 
    Integer,
    Flag
};

class ArgParser;

class Argument {
public:
    Argument();

    Argument(void *option, std::string const &name, 
             ArgType type, std::string const &init = "");

    void parse(ArgParser &args);

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

    void parse(std::size_t argc, char *argv[]);

    std::string get_next_argument();

private:
    std::vector<Argument> m_positional_args;

    std::unordered_map<std::string, Argument> m_keyword_args;

    std::size_t m_argc;

    char **m_argv;

    std::size_t m_curr;
};

#endif
