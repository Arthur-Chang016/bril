#include <IR/Function.h>
#include <IR/Program.h>

#include <cassert>
#include <memory>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ir {

Program::Program(const json& progJson) {
    assert(progJson.contains("functions"));
    for (const auto& function : progJson["functions"]) {
        functions.push_back(std::make_shared<Function>(function));
    }
}

}  // namespace ir
