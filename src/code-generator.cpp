#include "code-generator.hpp"
#include "ast.hpp"
#include "error.hpp"
#include <sstream>
#include <iomanip>

CodeGenerator::CodeGenerator()
        : m_data{}, m_func_labels{}, m_jobs{}, m_fresh_id{1}, m_scope{} {}

std::vector<CodeGenerator::entry_type> CodeGenerator::generate(Program &ast) {
    m_data.clear();

    m_scope.enter(ast.symbols());

    emit(Label(0));

    for (Statement::ptr const &stmt : ast.stmts()) {
        if (stmt->kind() != NodeKind::FunctionDeclaration) {
            stmt->accept(*this);
        }
    }

    emit(OpCode::Push, 0);
    emit(OpCode::ECall, ECallFunction::Exit);
    
    while (!m_jobs.empty()) {
        FunctionDefinition &def = *m_jobs.front();
        
        emit(m_func_labels.find(&def)->second);
        def.decl()->accept(*this);

        emit(OpCode::Push, 0);
        emit(OpCode::Ret);

        m_jobs.pop();
    }

    m_scope.leave(ast.symbols());

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
    m_scope.enter(decl.symbols());

    int offset = 4 * (decl.params().size() + 1);

    for (VariableDeclaration::ptr const &decl : decl.params()) {
        Symbol::unowned_ptr symbol = m_scope.lookup(decl->ident());
        LocalVariableSymbol::unowned_ptr param 
                = dynamic_cast<LocalVariableSymbol *>(symbol);

        param->set_offset(offset);
        offset -= 4;
    }

    for (Statement::ptr &stmt : decl.body()) {
        stmt->accept(*this);
    }

    m_scope.leave(decl.symbols());

    return decl;
}

Node &CodeGenerator::visit(ScopedBlockStatement &stmt) {
    m_scope.enter(stmt.symbols());
    
    for (Statement::ptr &substmt : stmt.body()) {
        substmt->accept(*this);
    }

    m_scope.leave(stmt.symbols());

    return stmt;
}

Node &CodeGenerator::visit(ExpressionStatement &stmt) {
    stmt.expr()->accept(*this);
    emit(OpCode::Pop);
    
    return stmt;
}

Node &CodeGenerator::visit(ReturnStatement &stmt) {
    stmt.value()->accept(*this);
    emit(OpCode::Ret);

    return stmt;
}

Node &CodeGenerator::visit(IfElseStatement &stmt) {
    Label label_else = fresh_label();
    Label label_end = fresh_label();
    
    stmt.condition()->accept(*this);
    emit(OpCode::JumpIfNot, label_else);

    stmt.then_stmt()->accept(*this);
    emit(OpCode::Jump, label_end);

    emit(label_else);
    stmt.else_stmt()->accept(*this);
    emit(label_end);

    return stmt;
}

Node &CodeGenerator::visit(WhileStatement &stmt) {
    Label label_loop = fresh_label();
    Label label_end = fresh_label();

    emit(label_loop);
    stmt.condition()->accept(*this);
    emit(OpCode::JumpIfNot, label_end);

    stmt.loop_stmt()->accept(*this);
    emit(OpCode::Jump, label_loop);
    emit(label_end);

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
    VariableSymbol::unowned_ptr var_symbol
            = dynamic_cast<VariableSymbol *>(m_scope.lookup(expr.ident()));

    LocalVariableSymbol::unowned_ptr var_local 
            = dynamic_cast<LocalVariableSymbol *>(var_symbol);

    if (var_local) {
        emit(OpCode::LoadRel, var_local->offset());
    } else {
        throw FatalError("not implemented");
    }

    return expr;
}

Node &CodeGenerator::visit(Integer &expr) {
    emit(OpCode::Push, std::stoi(expr.literal().lexeme()));
    return expr;
}

Node &CodeGenerator::visit(BooleanLiteral &expr) {
    emit(OpCode::Push, expr.literal().lexeme() == "True" ? 1 : 0);
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
