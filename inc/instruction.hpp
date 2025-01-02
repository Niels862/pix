#ifndef PIX_INSTRUCTION_HPP
#define PIX_INSTRUCTION_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <optional>
#include <cinttypes>

enum class OpCode {
    Nop,
    ECall,
    PushImm
};

std::string const &to_string(OpCode instr);

std::ostream &operator <<(std::ostream &stream, OpCode instr);

enum class ECallFunction {
    None,
    PrintInt,
    Exit
};

std::string const &to_string(ECallFunction ecall);

std::ostream &operator <<(std::ostream &stream, ECallFunction ecall);

class Label {
public:
    Label();

    friend std::ostream &operator <<(std::ostream &stream, Label const &label);

    using id_type = int;

    using map_type = std::unordered_map<id_type, uint32_t>;

    id_type id() const { return m_id; }

private:
    id_type m_id;
};

class Instruction {
public:
    Instruction(OpCode opcode = OpCode::Nop);

    Instruction(OpCode opcode, ECallFunction arg);

    Instruction(OpCode opcode, Label arg);

    Instruction(OpCode opcode, uint32_t arg);

    static Instruction Disassemble(uint32_t assembled);

    static OpCode unpack_opcode(uint32_t assembled)
            { return static_cast<OpCode>(assembled & 0xFF); }

    static uint32_t unpack_data(uint32_t assembled)
            { return (assembled >> 8) & 0xFFFFFF; }

    uint32_t assemble(Label::map_type const &labels) const;

    uint32_t assemble_arg(Label::map_type const &labels) const;

    friend std::ostream &operator <<(std::ostream &stream, 
                                     Instruction const &instr);

    struct interpretation {
        OpCode opcode : 8;
        uint32_t data : 24;
    };

private:
    OpCode m_opcode;

    std::optional<std::variant<uint32_t, Label, ECallFunction>> m_arg;
};

#endif
