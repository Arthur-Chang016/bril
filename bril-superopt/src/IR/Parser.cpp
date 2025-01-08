#include <IR/Parser.h>
#include <IR/Program.h>

#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <string>

namespace ir {

ProgramPtr parse(std::istream& input) {
    json j = json::parse(input);
    std::cout << j.dump(4) << std::endl;
    // auto program = std::make_shared<Program>(j);
    return nullptr;
}

}  // namespace ir
