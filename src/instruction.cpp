#include "instruction.hpp"
#include "error.hpp"
#include <unordered_map>
#include <sstream>

std::string const &to_string(OpCode instr) {
    static std::unordered_map<OpCode, std::string> const map = {
        { OpCode::Nop, "nop" },
    };

    auto const &it = map.find(instr);
    if (it == map.end()) {
        std::stringstream ss;
        ss << "Unmapped tokenkind: " << static_cast<int>(instr);
        throw FatalError(ss.str());
    }

    return it->second;
}

std::ostream &operator <<(std::ostream &stream, OpCode instr) {
    stream << to_string(instr);
    return stream;
}

Label::Label() {
    static int id = 0;

    id++;
    m_id = id;
}

std::ostream &operator <<(std::ostream &stream, Label const &label) {
    stream << ".L" << label.m_id;
    return stream;
}

Instruction::Instruction(OpCode opcode)
        : m_opcode{opcode}, m_arg{std::nullopt} {}

Instruction::Instruction(OpCode opcode, Label label)
        : m_opcode{opcode}, m_arg{label} {}

Instruction::Instruction(OpCode opcode, uint32_t data)
        : m_opcode{opcode}, m_arg{data} {}

std::ostream &operator <<(std::ostream &stream, Instruction const &instr) {
    stream << instr.m_opcode;
    if (instr.m_arg) {
        if (std::holds_alternative<Label>(instr.m_arg.value())) {
            stream << " " << std::get<Label>(instr.m_arg.value());
        } else {
            stream << " " << std::get<uint32_t>(instr.m_arg.value());
        }
    }

    return stream;
}
