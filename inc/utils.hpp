#ifndef PIX_UTILS_HPP
#define PIX_UTILS_HPP

#include <unordered_map>

template <typename T, typename U>
std::unordered_map<U, T> inverse(std::unordered_map<T, U> map) {
    std::unordered_map<U, T> inverted;

    for (const auto &pair : map) {
        inverted[pair.second] = pair.first;
    }

    return inverted;
}

#endif
