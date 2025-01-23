#include "virtual-machine.hpp"
#include "instruction.hpp"

VirtualMachine::VirtualMachine(Memory &memory)
        : m_memory{memory}, 
          m_ip{0}, m_sp{memory.size()}, m_terminated{false} {
    m_memory.set_top(m_sp);
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

    std::cout << Instruction::Disassemble(assembled) << std::endl;

    switch (opcode) {
        case OpCode::Nop:
            break;

        case OpCode::ECall:
            execute_ecall(static_cast<ECallFunction>(data));
            break;

        case OpCode::Call:
            m_memory.push_word(m_ip + 4);
            jump_to(data);
            break;

        case OpCode::Ret:
            jump_to_address(m_memory.pop_word());
            break;

        case OpCode::PushImm:
            m_memory.push_word(data);
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
