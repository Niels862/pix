#include "lexer.hpp"
#include "parser.hpp"
#include "type-checker.hpp"
#include "code-generator.hpp"
#include "json.hpp"
#include "instruction.hpp"
#include <iostream>

int main() {
    try {
        Lexer lexer("test.pix");

        std::vector<Token> tokens = lexer.lex();
        
        std::cout << "[" << std::endl;
        for (Token const &token : tokens) {
            std::cout << token << std::endl;
        }
        std::cout << "]" << std::endl;

        Parser parser(tokens);

        Node::ptr node = parser.parse();

        TypeChecker type_checker;
        node->accept(type_checker);

        CodeGenerator code_generator;
        node->accept(code_generator);
        code_generator.dump();

        std::cout << *node->to_json() << std::endl;
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::cout << OpCode::Nop << std::endl;

    return 0;
}
