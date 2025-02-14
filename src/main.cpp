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

ArgParser setup_args() {
    ArgParser args;

    args.add_positional(&options.filename, "filename",
                        ArgType::String);
    args.add_keyword(&options.no_exec, "no-exec",
                     ArgType::Flag);

    args.add_keyword(&options.debug.tokens, "debug-tokens",
                     ArgType::Flag);
    args.add_keyword(&options.debug.ast, "debug-ast",
                     ArgType::Flag);
    args.add_keyword(&options.debug.code, "debug-code",
                     ArgType::Flag);

    args.add_keyword(&options.vis.visualize, "visualize", 
                     ArgType::Flag);
    args.add_keyword(&options.vis.title, "vis-title", 
                     ArgType::String, ".");
    args.add_keyword(&options.vis.width, "vis-width", 
                     ArgType::Integer, "512");
    args.add_keyword(&options.vis.height, "vis-height", 
                     ArgType::Integer, "512");

    args.add_keyword(&options.mem.width, "mem-width", 
                     ArgType::Integer, "128");
    args.add_keyword(&options.mem.height, "mem-height",
                     ArgType::Integer, "128");

    args.add_keyword(&options.json.spacing, "json-spacing",
                     ArgType::Integer, "2");

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

        if (options.no_exec) {
            return 0;
        }

        Memory memory(options.mem.width * options.mem.height);
        Assembler(data, memory).assemble();
        VirtualMachine vm(memory);

        Renderer renderer;
        if (options.vis.visualize) {
            renderer.init();
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
