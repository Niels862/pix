#ifndef PIX_MEMORY_HPP
#define PIX_MEMORY_HPP

#include <memory>

class Memory {
public:
    Memory(std::size_t size);

    uint32_t get_word(std::size_t addr);

    void set_word(uint32_t word, std::size_t addr);

    uint32_t pop_word();

    void push_word(uint32_t word);

    void set_top(std::size_t base);

    char const *raw() const { return m_mem.get(); }

    std::size_t size() const { return m_size; }

    std::size_t top() const { return m_top; }

private:
    std::unique_ptr<char[]> m_mem;

    std::size_t m_size;

    std::size_t m_top;
};

#endif
