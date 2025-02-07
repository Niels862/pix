#include "instruction.hpp"
#include "error.hpp"
#include <unordered_map>
#include <sstream>
#include <iomanip>

std::string const &to_string(OpCode instr) {
    static std::unordered_map<OpCode, std::string> const map = {
        { OpCode::Nop, "nop" },
        { OpCode::ECall, "ecall" },
        { OpCode::Call, "call" },
        { OpCode::Ret, "ret" },
        { OpCode::Jump, "jump" },
        { OpCode::JumpIf, "jump-if" },
        { OpCode::JumpIfNot, "jump-if-not" },
        { OpCode::Push, "push" },
        { OpCode::Pop, "pop"},
        { OpCode::LoadRel, "load-rel" },
        { OpCode::LoadAbs, "load-abs" },
        { OpCode::StoreRel, "store-rel" },
        { OpCode::StoreAbs, "store-abs" },
        { OpCode::Enter, "enter" },
        { OpCode::IAdd, "iadd" },
        { OpCode::ISub, "isub" },
        { OpCode::IMul, "imul" },
        { OpCode::IDiv, "idiv" },
        { OpCode::IMod, "imod" },
        { OpCode::ILT, "ilt" },
        { OpCode::ILE, "ile" },
        { OpCode::IGT, "igt" },
        { OpCode::IGE, "ige" },
        { OpCode::Equ, "equ" },
        { OpCode::Neq, "neq" }
    };

    auto const &it = map.find(instr);
    if (it == map.end()) {
        std::stringstream ss;
        ss << "Unmapped opcode: " << static_cast<int>(instr);
        throw FatalError(ss.str());
    }

    return it->second;
}

std::ostream &operator <<(std::ostream &stream, OpCode instr) {
    stream << to_string(instr);
    return stream;
}

std::string const &to_string(ECallFunction ecall) {
    static std::unordered_map<ECallFunction, std::string> const map = {
        { ECallFunction::None, "<none>" },
        { ECallFunction::PrintInt, "print-int" },
        { ECallFunction::PrintBool, "print-bool" },
        { ECallFunction::Exit, "exit" }
    };

    auto const &it = map.find(ecall);
    if (it == map.end()) {
        std::stringstream ss;
        ss << "Unmapped ecall: " << static_cast<int>(ecall);
        throw FatalError(ss.str());
    }

    return it->second;
}

std::ostream &operator <<(std::ostream &stream, ECallFunction ecall) {
    stream << to_string(ecall);
    return stream;
}

Label::Label()
        : m_id{} {}

Label::Label(int id)
        : m_id{id} {}

std::ostream &operator <<(std::ostream &stream, Label const &label) {
    stream << ".L" << label.m_id;
    return stream;
}

Instruction::Instruction(OpCode opcode)
        : m_opcode{opcode}, m_arg{std::nullopt} {}

Instruction::Instruction(OpCode opcode, ECallFunction arg) 
        : m_opcode{opcode}, m_arg{arg} {}

Instruction::Instruction(OpCode opcode, Label arg) 
        : m_opcode{opcode}, m_arg{arg} {}

Instruction::Instruction(OpCode opcode, uint32_t arg) 
        : m_opcode{opcode}, m_arg{arg} {}

Instruction Instruction::Disassemble(uint32_t assembled) {
    auto interpretation 
            = *reinterpret_cast<Instruction::interpretation *>(&assembled);
    return Instruction(interpretation.opcode, 
                       Instruction::sign_extend_24_32(interpretation.data));
}

uint32_t Instruction::assemble(Label::map_type const &labels) const {
    Instruction::interpretation intrp;

    intrp.opcode = m_opcode;
    intrp.data = assemble_arg(labels);

    return *reinterpret_cast<uint32_t *>(&intrp);
}

uint32_t Instruction::assemble_arg(Label::map_type const &labels) const {
    if (!m_arg) {
        return 0;
    }

    auto arg = m_arg.value();

    if (std::holds_alternative<uint32_t>(arg)) {
        return std::get<uint32_t>(arg);
    }

    if (std::holds_alternative<Label>(arg)) {
        Label const &label = std::get<Label>(arg);
        Label::map_type::const_iterator const &iter = labels.find(label.id());
        
        if (iter == labels.end()) {
            std::stringstream ss;
            ss << "Undefined label in instruction " << *this;
            throw FatalError(ss.str());
        }

        return iter->second;
    }

    if (std::holds_alternative<ECallFunction>(arg)) {
        return static_cast<uint32_t>(std::get<ECallFunction>(arg));
    }

    throw FatalError("assemble_arg(): Unhandled argument type");
}

std::ostream &operator <<(std::ostream &stream, Instruction const &instr) {
    stream << instr.m_opcode;
    if (instr.m_arg) {
        stream << " ";

        auto arg = instr.m_arg.value();
        if (std::holds_alternative<uint32_t>(arg)) {
            stream << static_cast<int32_t>(std::get<uint32_t>(arg));
        } else if (std::holds_alternative<Label>(arg)) {
            stream << std::get<Label>(arg);
        } else {
            stream << std::get<ECallFunction>(arg);
        }
    }

    return stream;
}
