#include "assembler.hpp"
#include "error.hpp"
#include <sstream>

Assembler::Assembler(std::vector<CodeGenerator::entry_type> const &data, 
                     Memory &memory)
        : m_data{data}, m_labels{}, m_memory{memory} {}

void Assembler::assemble() {
    definition_pass();
    emission_pass();
}

void Assembler::definition_pass() {
    m_labels.clear();

    std::size_t p = 0;

    for (CodeGenerator::entry_type const &entry : m_data) {
        if (std::holds_alternative<Instruction>(entry)) {
            p++;
        } else {
            Label::id_type id = std::get<Label>(entry).id();

            if (m_labels.find(id) != m_labels.end()) {
                std::stringstream ss;
                ss << "Redefined label: " << id;
                throw FatalError(ss.str());
            }

            m_labels[id] = p;
        }
    }
}

void Assembler::emission_pass() {
    std::size_t p = 0;

    for (CodeGenerator::entry_type const &entry : m_data) {
        if (std::holds_alternative<Instruction>(entry)) {
            Instruction const &instr = std::get<Instruction>(entry);
            uint32_t assembled = instr.assemble(m_labels);
            
            m_memory.set_word(assembled, 4 * p);
            p++;
        }
    }
}
