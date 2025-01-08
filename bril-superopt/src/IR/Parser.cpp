#include <IR/Parser.h>
#include <IR/Program.h>

#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <string>

namespace ir {

ProgramPtr parse(std::istream& input) {
    json progJson = json::parse(input);
    // std::cout << progJson.dump(4) << std::endl;
    auto program = std::make_shared<Program>(progJson);
    program->print(std::cout);
    return program;
}

}  // namespace ir
