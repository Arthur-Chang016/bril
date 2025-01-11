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
    std::string name;
    std::vector<VarPtr> args;
    std::vector<BBPtr> basicBlocks;

    Function(const json& funcJson);
    ~Function() = default;
    friend std::ostream& operator<<(std::ostream& os, const Function& func);
    void ConstructCFG(std::vector<InstPtr>& instrs);
    std::optional<int64_t> execute(varContext& vars, HeapManager& heap);

   private:
    BBPtr entryBB = nullptr;
    TypePtr retType = nullptr;
};

using FuncPtr = std::shared_ptr<Function>;
using FuncWPtr = std::weak_ptr<Function>;

}  // namespace ir

#endif  // IR_FUNCTION_H