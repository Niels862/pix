#ifndef PIX_CODE_GENERATOR_HPP
#define PIX_CODE_GENERATOR_HPP

#include "visitor.hpp"
#include "instruction.hpp"
#include <vector>
#include <variant>

class CodeGenerator : public AstVisitor {
public:
    CodeGenerator() = default;

    Node &default_action(Node &node) override;

    Node &visit(ExpressionStatement &stmt) override;

    Node &visit(Call &expr) override;
    
    Node &visit(Variable &expr) override;

    Node &visit(Integer &expr) override;

    void dump() const;

private:
    void emit(OpCode opcode);

    void emit(OpCode opcode, Label label);

    void emit(OpCode opcode, uint32_t data);

    void emit(Label label);

    std::vector<std::variant<Instruction, Label>> m_data;
};

#endif
