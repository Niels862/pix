#ifndef PIX_OPTIONS_HPP
#define PIX_OPTIONS_HPP

#include <string>

struct Options {
    std::string filename;
    bool no_exec;

    struct {
        bool tokens;
        bool ast;
        bool code;
    } debug;
    
    struct {
        std::string title;
        bool visualize;
        int width;
        int height;
    } vis;

    struct {
        int width;
        int height;
    } mem;

    struct {
        int spacing;
    } json;
};

extern Options options;

#endif
