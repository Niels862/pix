#ifndef PIX_ARGPARSER_HPP
#define PIX_ARGPARSER_HPP

#include <vector>
#include <string>

enum class ArgType {
    String, 
    Flag
};

class Argument {
public:
    Argument(void *option, std::string const &name, 
             ArgType type, std::string const &init = "");

    void set_value(std::string const &value);

    void fill_option();

private:    
    void *m_option;

    std::string const &m_name;

    ArgType m_type;

    std::string m_init;

    std::string m_value;
};

class ArgParser {
public:
    ArgParser();

    void add_positional(void *option, std::string const &name, 
                        ArgType type, std::string const &init = "");

    void parse(std::size_t arg, char *argv[]);

private:
    std::vector<Argument> m_positional_args;
};

#endif
