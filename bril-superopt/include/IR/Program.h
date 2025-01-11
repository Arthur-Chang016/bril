#ifndef IR_PROGRAM_H
#define IR_PROGRAM_H

#include <IR/Function.h>
#include <IR/Heap.h>
#include <IR/Type.h>

#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>
#include <vector>
using json = nlohmann::json;

namespace ir {

class Program {
   public:
    FuncPtr mainFunc = nullptr;

    Program(const json& progJson);
    ~Program() = default;
    void ConstructCallLink(const std::unordered_map<std::string, FuncWPtr>& name2func);
    void SetupMainFunc(const std::unordered_map<std::string, FuncWPtr>& name2func);
    varContext SetupVarContext(int argc, char** argv);
    friend std::ostream& operator<<(std::ostream& os, const Program& prog);
    void execute(varContext& vars, HeapManager& heap);

   private:
    std::vector<FuncPtr> functions;
};

using ProgramPtr = std::shared_ptr<Program>;

}  // namespace ir

#endif  // IR_PROGRAM_H
