#include <IR/Parser.h>

#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <string>

namespace ir {

Parser::Parser(std::istream& input) {
    json j = json::parse(input);
    std::cout << j.dump(4) << std::endl;
}

}  // namespace ir
