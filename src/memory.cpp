#include "memory.hpp"
#include "error.hpp"

Memory::Memory(std::size_t size)
        : m_mem{std::make_unique<char[]>(size)}, m_size{size} {}

uint32_t Memory::get_word(std::size_t addr) {
    if (addr % 4 != 0) {
        throw FatalError("set_word(): Unaligned access");
    }
    return *reinterpret_cast<uint32_t *>(&m_mem[addr]);
}

void Memory::set_word(uint32_t word, std::size_t addr) {
    if (addr % 4 != 0) {
        throw FatalError("set_word(): Unaligned access");
    }
    *reinterpret_cast<uint32_t *>(&m_mem[addr]) = word;
}
