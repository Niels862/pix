#include "virtual-machine.hpp"
#include "instruction.hpp"
#include <iomanip>

VirtualMachine::VirtualMachine(Memory &memory)
        : m_memory{memory}, 
          m_ip{0}, m_base{133}, m_terminated{false} {
    m_memory.set_top(memory.size());
}

void VirtualMachine::execute_quantum(int q) {
    if (m_terminated) {
        return;
    }

    for (int i = 0; i < q; i++) {
        execute_step();
    }
}

void VirtualMachine::execute_step() {
    if (m_terminated) {
        return;
    }

    uint32_t assembled = m_memory.get_word(m_ip);

    OpCode opcode = Instruction::unpack_opcode(assembled);
    uint32_t data = Instruction::unpack_data(assembled);

    uint32_t x, addr;

    std::cout << Instruction::Disassemble(assembled) << std::endl;

    switch (opcode) {
        case OpCode::Nop:
            break;

        case OpCode::ECall:
            execute_ecall(static_cast<ECallFunction>(data));
            break;

        case OpCode::Call:
            m_memory.push_word(m_base);
            m_memory.push_word(m_ip + 4);

            jump_to(data);
            
            m_base = m_memory.top();
            break;

        case OpCode::Ret:
            x = m_memory.pop_word();

            m_memory.set_top(m_base);

            jump_to_address(m_memory.pop_word());
            m_base = m_memory.pop_word();

            m_memory.push_word(x);
            break;

        case OpCode::Push:
            m_memory.push_word(data);
            break;

        case OpCode::Pop:
            m_memory.pop_word();
            break;

        case OpCode::LoadRel:
            addr = m_base + Instruction::sign_extend_24_32(data);
            x = m_memory.get_word(addr);
            m_memory.push_word(x);
            break;

        case OpCode::LoadAbs:
            x = m_memory.get_word(data);
            m_memory.push_word(x);
            break;

        case OpCode::StoreRel:
            addr = m_base + Instruction::sign_extend_24_32(data);
            x = m_memory.pop_word();
            m_memory.set_word(x, addr);
            break;

        case OpCode::StoreAbs:
            x = m_memory.pop_word();
            m_memory.set_word(x, data);
            break;
    }

    m_ip += 4;
}

void VirtualMachine::execute_ecall(ECallFunction ecall) {
    switch (ecall) {
        case ECallFunction::None:
            break;

        case ECallFunction::PrintInt:
            std::cout << ">> " << m_memory.pop_word() << std::endl;
            m_memory.push_word(0);
            break;

        case ECallFunction::Exit:
            m_terminated = true;
            break;
    }
}

void VirtualMachine::jump_to(std::size_t target) {
    jump_to_address(4 * target);
}

void VirtualMachine::jump_to_address(std::size_t addr) {
    m_ip = addr - 4;
}
