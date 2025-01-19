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

int const MemWidth = 16, MemHeight = 16;
bool const UseRenderer = false;

int main() {
    try {
        Lexer lexer("test.pix");
        std::vector<Token> tokens = lexer.lex();
        
        Parser parser(tokens);
        Node::ptr node = parser.parse();

        TypeChecker type_checker;
        node->accept(type_checker);

        std::cout << *node->to_json() << std::endl;

        std::vector<CodeGenerator::entry_type> data 
                = CodeGenerator().generate(*node);

        std::cout << data << std::endl;

        Memory memory(MemWidth * MemHeight);
        Assembler(data, memory).assemble();
        VirtualMachine vm(memory);

        Renderer renderer;
        if (UseRenderer) {
            renderer.init(256, 256, MemWidth, MemHeight);
        }

        while (!vm.terminated()) {
            if (renderer.process_events()) {
                break;
            }
            
            renderer.draw_frame(memory.raw());
            vm.execute_step();
        }

    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
