#include "code-generator.hpp"
#include "ast.hpp"
#include "error.hpp"
#include <sstream>
#include <iomanip>

CodeGenerator::CodeGenerator()
        : m_data{} {}

std::vector<CodeGenerator::entry_type> CodeGenerator::generate(Node &node) {
    m_data.clear();

    emit(Label());

    node.accept(*this);

    emit(OpCode::PushImm, 0);
    emit(OpCode::ECall, ECallFunction::Exit);
    
    return m_data;
}

Node &CodeGenerator::default_action(Node &node) {
    std::stringstream ss;
    ss << "CodeGenerator(): unimplemented action: " << node.kind();
    throw FatalError(ss.str());
}

Node &CodeGenerator::visit(Program &program) {
    for (Statement::ptr &stmt : program.stmts()) {
        stmt->accept(*this);
    }

    return program;
}

Node &CodeGenerator::visit(ExpressionStatement &stmt) {
    stmt.expr().accept(*this);
    return stmt;
}

Node &CodeGenerator::visit(Call &expr) {
    for (Expression::ptr &expr : expr.args()) {
        expr->accept(*this);
    }

    emit(OpCode::ECall, expr.called().ecall());

    return expr;
}

Node &CodeGenerator::visit(Variable &expr) {
    return expr;
}

Node &CodeGenerator::visit(Integer &expr) {
    emit(OpCode::PushImm, std::stoi(expr.literal().lexeme()));
    return expr;
}

void CodeGenerator::emit(OpCode opcode) {
    m_data.emplace_back(opcode);
}

template <typename T>
void CodeGenerator::emit(OpCode opcode, T arg)  {
    m_data.emplace_back(std::in_place_type<Instruction>, opcode, arg);
}

void CodeGenerator::emit(Label label) {
    m_data.emplace_back(label);
}

std::ostream &operator <<(std::ostream &stream, 
                          std::vector<CodeGenerator::entry_type> const &data) {
    bool first = true;
    for (auto const &entry : data) {
        if (!first) {
            stream << std::endl;
        }
        if (std::holds_alternative<Instruction>(entry)) {
            stream << std::setw(2) << "" 
                   << std::get<Instruction>(entry);
        } else {
            stream << std::get<Label>(entry) << ":";
        }
        first = false;
    }

    return stream;
}
