#include "virtual-machine.hpp"
#include "instruction.hpp"

VirtualMachine::VirtualMachine(Memory &memory)
        : m_memory{memory}, 
          m_ip{0}, m_sp{memory.size()}, m_terminated{false} {}

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

        case OpCode::PushImm:
            m_sp -= 4;
            m_memory.set_word(data, m_sp);
            break;

        case OpCode::ECall:
            execute_ecall(static_cast<ECallFunction>(data));
            break;
    }

    m_ip += 4;
}

void VirtualMachine::execute_ecall(ECallFunction ecall) {
    switch (ecall) {
        case ECallFunction::None:
            break;

        case ECallFunction::PrintInt:
            std::cout << ">> " << m_memory.get_word(m_sp) << std::endl;
            m_sp += 4;
            break;

        case ECallFunction::Exit:
            m_terminated = true;
            break;
    }
}
