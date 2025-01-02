#ifndef PIX_MEMORY_HPP
#define PIX_MEMORY_HPP

#include <memory>

class Memory {
public:
    Memory(std::size_t size);

    uint32_t get_word(std::size_t addr);

    void set_word(uint32_t word, std::size_t addr);

    char const *raw() const { return m_mem.get(); }

    std::size_t size() const { return m_size; }

private:
    std::unique_ptr<char[]> m_mem;
    std::size_t m_size;
};

#endif
