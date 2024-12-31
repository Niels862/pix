#ifndef PIX_ERROR_HPP
#define PIX_ERROR_HPP

#include <string>
#include <stdexcept>

class FatalError : public std::exception {
public:
    explicit FatalError(std::string const &msg) 
            : m_msg{msg} {}

    char const *what() const noexcept override {
        return m_msg.c_str();
    }
private:
    std::string m_msg;
};

#endif
