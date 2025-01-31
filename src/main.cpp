#include "lexer.hpp"
#include "parser.hpp"
#include "symbol-resolver.hpp"
#include "type-checker.hpp"
#include "code-generator.hpp"
#include "memory.hpp"
#include "assembler.hpp"
#include "virtual-machine.hpp"
#include "renderer.hpp"
#include "json.hpp"
#include "instruction.hpp"
#include <iostream>

int const MemWidth = 64, MemHeight = 64;
bool const UseRenderer = false;

int main() {
    try {
        Lexer lexer("test.pix");
        std::vector<Token> tokens = lexer.lex();
        
        for (Token const &token : tokens) {
            std::cout << token << std::endl;
        }

        Parser parser(tokens);
        Program::ptr ast = parser.parse();

        SymbolResolver symbol_resolver;
        ast->accept(symbol_resolver);

        TypeChecker type_checker;
        ast->accept(type_checker);

        std::cout << *ast->to_json() << std::endl;

        std::vector<CodeGenerator::entry_type> data 
                = CodeGenerator().generate(*ast);

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
