#ifndef PIX_OPTIONS_HPP
#define PIX_OPTIONS_HPP

#include <string>

struct Options {
    std::string filename;
    struct {
        bool tokens;
        bool ast;
        bool code;
    } debug;
};

extern Options options;

#endif
