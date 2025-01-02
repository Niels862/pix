#include "lexer.hpp"
#include "parser.hpp"
#include "type-checker.hpp"
#include "code-generator.hpp"
#include "memory.hpp"
#include "assembler.hpp"
#include "virtual-machine.hpp"
#include "renderer.hpp"
#include "json.hpp"
#include "instruction.hpp"
#include <iostream>

int main() {
    try {
        Lexer lexer("test.pix");
        std::vector<Token> tokens = lexer.lex();
        
        Parser parser(tokens);
        Node::ptr node = parser.parse();

        TypeChecker type_checker;
        node->accept(type_checker);

        std::cout << *node->to_json() << std::endl;

        CodeGenerator code_generator;
        std::vector<CodeGenerator::entry_type> data 
                = code_generator.generate(*node);

        std::cout << data << std::endl;

        Memory memory(32 * 32);
        Assembler(data, memory).assemble();
        VirtualMachine vm(memory);

        Renderer renderer(256, 256, 32, 32);
        while (true) {
            if (renderer.process_events()) {
                break;
            }
            
            renderer.draw_frame(memory.raw());
            vm.execute_step();

            SDL_Delay(1000);
        }

    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
