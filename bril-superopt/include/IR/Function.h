#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H

#include <IR/BasicBlock.h>
#include <IR/Heap.h>
#include <IR/Instruction.h>
#include <IR/Type.h>

#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>
#include <vector>
using json = nlohmann::json;

namespace ir {

class Function {
   public:
    Function(const json& funcJson);
    ~Function() = default;
    friend std::ostream& operator<<(std::ostream& os, const Function& func);
    void ConstructCFG(std::vector<InstPtr>& instrs);
    std::optional<int64_t> execute(varContext& vars, HeapManager& heap);

   private:
    std::string name;
    std::vector<VarPtr> args;
    BBPtr entryBB = nullptr;
    std::vector<BBPtr> basicBlocks;
    TypePtr retType = nullptr;
};

using FunctionPtr = std::shared_ptr<Function>;

}  // namespace ir

#endif  // IR_FUNCTION_H