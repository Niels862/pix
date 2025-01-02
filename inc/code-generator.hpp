#ifndef PIX_CODE_GENERATOR_HPP
#define PIX_CODE_GENERATOR_HPP

#include "visitor.hpp"
#include "instruction.hpp"
#include <vector>
#include <iostream>
#include <variant>

class CodeGenerator : public AstVisitor {
public:
    CodeGenerator();

    using entry_type = std::variant<Instruction, Label>;

    std::vector<entry_type> generate(Node &node);

    Node &default_action(Node &node) override;

    Node &visit(ExpressionStatement &stmt) override;

    Node &visit(Call &expr) override;
    
    Node &visit(Variable &expr) override;

    Node &visit(Integer &expr) override;

private:
    void emit(OpCode opcode);

    template <typename T>
    void emit(OpCode opcode, T arg);

    void emit(Label label);

    std::vector<entry_type> m_data;
};

std::ostream &operator <<(std::ostream &stream, 
                          std::vector<CodeGenerator::entry_type> const &data);

#endif
