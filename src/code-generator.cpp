#include "code-generator.hpp"
#include "ast.hpp"
#include "error.hpp"
#include <sstream>
#include <iomanip>

Node &CodeGenerator::default_action(Node &node) {
    std::stringstream ss;
    ss << "CodeGenerator(): unimplemented action: " << node.kind();
    throw FatalError(ss.str());
}

Node &CodeGenerator::visit(ExpressionStatement &stmt) {
    return stmt;
}

Node &CodeGenerator::visit(Call &expr) {
    return expr;
}

Node &CodeGenerator::visit(Variable &expr) {
    return expr;
}

Node &CodeGenerator::visit(Integer &expr) {
    return expr;
}

void CodeGenerator::dump() const {
    for (auto const &entry : m_data) {
        if (std::holds_alternative<Instruction>(entry)) {
            std::cout << std::setw(2) << "" 
                      << std::get<Instruction>(entry) << std::endl;
        } else {
            std::cout << std::get<Label>(entry) << std::endl;
        }
    }
}

void CodeGenerator::emit(OpCode opcode) {
    m_data.emplace_back(opcode);
}

void CodeGenerator::emit(OpCode opcode, Label label) {
    m_data.emplace_back(std::in_place_type<Instruction>, opcode, label);
}

void CodeGenerator::emit(OpCode opcode, uint32_t data) {
    m_data.emplace_back(std::in_place_type<Instruction>, opcode, data);
}

void CodeGenerator::emit(Label label) {
    m_data.emplace_back(label);
}
