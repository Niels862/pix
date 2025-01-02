#ifndef PIX_VIRTUAL_MACHINE_HPP
#define PIX_VIRTUAL_MACHINE_HPP

#include "memory.hpp"
#include "instruction.hpp"

class VirtualMachine {
public:
    VirtualMachine(Memory &memory);

    void execute_quantum(int q);

    void execute_step();
 
    bool terminated() const { return m_terminated; }

private:
    void execute_ecall(ECallFunction ecall);

    Memory &m_memory;

    std::size_t m_ip;

    std::size_t m_sp;

    bool m_terminated;
};

#endif
