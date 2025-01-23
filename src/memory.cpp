#include "memory.hpp"
#include "error.hpp"
#include <sstream>

Memory::Memory(std::size_t size)
        : m_mem{std::make_unique<char[]>(size)}, m_size{size}, m_top{0} {}

uint32_t Memory::get_word(std::size_t addr) {
    if (addr % 4 != 0) {
        std::stringstream ss;
        ss << "get_word(): Unaligned access at " << addr;
        throw FatalError(ss.str());
    }
    return *reinterpret_cast<uint32_t *>(&m_mem[addr]);
}

void Memory::set_word(uint32_t word, std::size_t addr) {
    if (addr % 4 != 0) {
        std::stringstream ss;
        ss << "set_word(): Unaligned access to " << addr;
        throw FatalError(ss.str());
    }
    *reinterpret_cast<uint32_t *>(&m_mem[addr]) = word;
}


uint32_t Memory::pop_word() {
    uint32_t word = get_word(m_top);
    m_top += 4;
    return word;
}

void Memory::push_word(uint32_t word) {
    m_top -= 4;
    set_word(word, m_top);
}

void Memory::set_top(std::size_t top) {
    m_top = top;
}
