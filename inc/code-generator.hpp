#ifndef PIX_CODE_GENERATOR_HPP
#define PIX_CODE_GENERATOR_HPP

#include "visitor.hpp"
#include "instruction.hpp"
#include "symbol.hpp"
#include <vector>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <variant>

class CodeGenerator : public AstVisitor {
public:
    CodeGenerator();

    using entry_type = std::variant<Instruction, Label>;

    std::vector<entry_type> generate(Program &ast);

    Node &default_action(Node &node) override;

    Node &visit(Program &program) override;

    Node &visit(FunctionDeclaration &decl) override;

    Node &visit(ExpressionStatement &stmt) override;

    Node &visit(Call &expr) override;
    
    Node &visit(Variable &expr) override;

    Node &visit(Integer &expr) override;

    Label fresh_label();

    void emit(OpCode opcode);

    template <typename T>
    void emit(OpCode opcode, T arg);

    void emit(Label label);

private:
    std::vector<entry_type> m_data;

    std::unordered_map<FunctionDefinition *, Label> m_func_labels;

    std::queue<FunctionDefinition *> m_jobs;

    int m_fresh_id;
};

std::ostream &operator <<(std::ostream &stream, 
                          std::vector<CodeGenerator::entry_type> const &data);

#endif
