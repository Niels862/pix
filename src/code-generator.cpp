#include "code-generator.hpp"
#include "ast.hpp"
#include "error.hpp"
#include <sstream>
#include <iomanip>

CodeGenerator::CodeGenerator()
        : m_data{}, m_func_labels{}, m_jobs{}, m_fresh_id{1} {}

std::vector<CodeGenerator::entry_type> CodeGenerator::generate(Program &ast) {
    m_data.clear();

    emit(Label(0));

    for (Statement::ptr const &stmt : ast.stmts()) {
        if (stmt->kind() != NodeKind::FunctionDeclaration) {
            stmt->accept(*this);
        }
    }

    emit(OpCode::PushImm, 0);
    emit(OpCode::ECall, ECallFunction::Exit);
    
    while (!m_jobs.empty()) {
        FunctionDefinition &def = *m_jobs.front();
        
        emit(m_func_labels.find(&def)->second);
        def.decl()->accept(*this);
        emit(OpCode::Ret);

        m_jobs.pop();
    }

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

Node &CodeGenerator::visit(FunctionDeclaration &decl) {
    for (Statement::ptr &stmt : decl.body()) {
        stmt->accept(*this);
    }

    return decl;
}

Node &CodeGenerator::visit(ExpressionStatement &stmt) {
    stmt.expr().accept(*this);
    return stmt;
}

Node &CodeGenerator::visit(Call &expr) {
    for (Expression::ptr &expr : expr.args()) {
        expr->accept(*this);
    }

    FunctionDefinition &def = expr.called();
    if (def.is_ecall()) {
        emit(OpCode::ECall, def.ecall());
    } else {
        auto iter = m_func_labels.find(&def);

        Label label;
        if (iter == m_func_labels.end()) {
            label = fresh_label();
            m_func_labels[&def] = label;
            m_jobs.push(&def);
        } else {
            label = iter->second;
        }

        emit(OpCode::Call, label);
    }

    return expr;
}

Node &CodeGenerator::visit(Variable &expr) {
    return expr;
}

Node &CodeGenerator::visit(Integer &expr) {
    emit(OpCode::PushImm, std::stoi(expr.literal().lexeme()));
    return expr;
}

Label CodeGenerator::fresh_label() {
    Label label(m_fresh_id);
    m_fresh_id++;
    return label;
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
