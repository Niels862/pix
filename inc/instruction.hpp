#ifndef PIX_INSTRUCTION_HPP
#define PIX_INSTRUCTION_HPP

#include <iostream>
#include <string>
#include <variant>
#include <optional>
#include <cinttypes>

enum class OpCode {
    Nop
};

std::string const &to_string(OpCode instr);

std::ostream &operator <<(std::ostream &stream, OpCode instr);

class Label {
public:
    Label();

    friend std::ostream &operator <<(std::ostream &stream, Label const &label);

private:
    int m_id;
};

class Instruction {
public:
    Instruction(OpCode opcode = OpCode::Nop);

    Instruction(OpCode opcode, Label label);

    Instruction(OpCode opcode, uint32_t data);

    friend std::ostream &operator <<(std::ostream &stream, 
                                     Instruction const &instr);

private:
    OpCode m_opcode;

    std::optional<std::variant<uint32_t, Label>> m_arg;
};

#endif
