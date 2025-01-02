#ifndef PIX_ASSEMBLER_HPP
#define PIX_ASSEMBLER_HPP

#include "code-generator.hpp"
#include "instruction.hpp"
#include "memory.hpp"
#include <vector>
#include <unordered_map>

class Assembler {
public:
    Assembler(std::vector<CodeGenerator::entry_type> const &data, 
              Memory &memory);

    void assemble();

private:
    void definition_pass();

    void emission_pass();

    std::vector<CodeGenerator::entry_type> const &m_data;

    Label::map_type m_labels;

    Memory &m_memory;
};

#endif
