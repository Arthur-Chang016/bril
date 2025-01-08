#include <IR/Function.h>
#include <IR/Program.h>

#include <cassert>
#include <memory>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ir {

Program::Program(const json& progJson) {
    if (!progJson.contains("functions"))
        throw std::runtime_error("progJson does not contain 'functions'");
    for (const auto& function : progJson["functions"]) {
        functions.push_back(std::make_shared<Function>(function));
    }
}

}  // namespace ir
