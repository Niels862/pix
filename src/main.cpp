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
#include "argparser.hpp"
#include "options.hpp"
#include <iostream>
#include <iomanip>


int const MemWidth = 64, MemHeight = 64;
bool const UseRenderer = false;

ArgParser setup_args() {
    ArgParser args;

    args.add_positional(&options.filename, "filename", ArgType::String);
    args.add_keyword(&options.debug.tokens, "debug-tokens", ArgType::Flag);
    args.add_keyword(&options.debug.ast, "debug-ast", ArgType::Flag);
    args.add_keyword(&options.debug.code, "debug-code", ArgType::Flag);

    return args;
}

int main(int argc, char *argv[]) {
    try {
        ArgParser args = setup_args();
        args.parse(argc, argv);

        Lexer lexer(options.filename);
        std::vector<Token> tokens = lexer.lex();

        if (options.debug.tokens) {
            std::cerr << "{" << std::endl;

            for (Token const &token : tokens) {
                std::cerr << std::setw(2) << "" << token << std::endl;
            }

            std::cerr << "}" << std::endl;
        }

        Parser parser(tokens);
        Program::ptr ast = parser.parse();

        SymbolResolver symbol_resolver;
        ast->accept(symbol_resolver);

        TypeChecker type_checker;
        ast->accept(type_checker);

        if (options.debug.ast) {
            std::cerr << *ast->to_json() << std::endl;
        }

        std::vector<CodeGenerator::entry_type> data 
                = CodeGenerator().generate(*ast);

        if (options.debug.code) {
            std::cerr << data << std::endl;
        }

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
