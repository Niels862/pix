#ifndef PIX_CODE_GENERATOR_HPP
#define PIX_CODE_GENERATOR_HPP

#include "visitor.hpp"
#include "instruction.hpp"
#include "symbol.hpp"
#include "symbol-table.hpp"
#include <vector>
#include <queue>
#include <stack>
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

    Node &visit(VariableDeclaration &decl) override;

    Node &visit(ScopedBlockStatement &stmt) override;

    Node &visit(ExpressionStatement &stmt) override;

    Node &visit(ReturnStatement &stmt) override;

    Node &visit(IfElseStatement &stmt) override;

    Node &visit(WhileStatement &stmt) override;

    Node &visit(BreakStatement &stmt) override;

    Node &visit(ContinueStatement &stmt) override;

    Node &visit(UnaryExpression &expr) override;

    Node &visit(BinaryExpression &expr) override;

    Node &visit(Call &expr) override;
    
    Node &visit(Variable &expr) override;

    Node &visit(Integer &expr) override;

    Node &visit(BooleanLiteral &expr) override;

    Label fresh_label();

    void emit(OpCode opcode);

    template <typename T>
    void emit(OpCode opcode, T arg);

    void emit(Label label);

private:
    std::vector<entry_type> m_data;

    std::unordered_map<FunctionDefinition *, Label> m_func_labels;

    std::queue<FunctionDefinition *> m_jobs;

    FunctionDefinition *m_curr_job;

    std::stack<Label> m_break_labels;

    std::stack<Label> m_continue_labels;

    int m_fresh_id;

    SymbolScope m_scope;
};

std::ostream &operator <<(std::ostream &stream, 
                          std::vector<CodeGenerator::entry_type> const &data);

#endif
