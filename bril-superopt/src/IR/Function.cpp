#include <IR/BasicBlock.h>
#include <IR/Function.h>
#include <IR/Instruction.h>
#include <IR/Type.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace ir {

TypePtr ParseType(const json& typeJson) {
    if (typeJson.is_string()) {
        std::string type = typeJson;
        if (type == "int")
            return std::make_shared<IntType>();
        else if (type == "bool")
            return std::make_shared<BoolType>();
        else
            throw std::runtime_error("Unknown type: " + type);
    } else if (typeJson.is_object()) {
        if (!typeJson.contains("ptr")) throw std::runtime_error("typeJson does not contain 'ptr'");
        return std::make_shared<PointerType>(ParseType(typeJson["ptr"]));
    } else {
        throw std::runtime_error("Unknown type: " + typeJson.dump());
    }
}

BinaryOpType StrToBinOp(const std::string& op) {
    if (op == "add")
        return BinaryOpType::Add;
    else if (op == "sub")
        return BinaryOpType::Sub;
    else if (op == "mul")
        return BinaryOpType::Mul;
    else if (op == "div")
        return BinaryOpType::Div;
    else if (op == "and")
        return BinaryOpType::And;
    else if (op == "or")
        return BinaryOpType::Or;
    else if (op == "eq")
        return BinaryOpType::Eq;
    else if (op == "lt")
        return BinaryOpType::Lt;
    else if (op == "gt")
        return BinaryOpType::Gt;
    else if (op == "le")
        return BinaryOpType::Le;
    else if (op == "ge")
        return BinaryOpType::Ge;
    else
        return BinaryOpType::BinInvalid;
}

UnaryOpType StrToUnOp(const std::string& op) {
    if (op == "not")
        return UnaryOpType::Not;
    else
        return UnaryOpType::UnInvalid;
}

InstPtr ParseInstr(const json& instJson) {
    // std::cout << "Parsing instruction: " << instJson << std::endl;
    if (instJson.contains("label")) {
        std::string label = instJson["label"];
        return std::make_shared<Label>(std::move(label));
    }
    std::string op = instJson["op"];  // assume to have "op" key
    if (op == "const") {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        int64_t value = instJson["value"].is_number_integer() ? int64_t(instJson["value"]) : int64_t(bool(instJson["value"]));
        return std::make_shared<Constant>(dest, value);
    } else if (BinaryOpType binOp = StrToBinOp(op); binOp != BinaryOpType::BinInvalid) {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        std::string lhs = instJson["args"][0], rhs = instJson["args"][1];
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        return std::make_shared<BinaryOp>(binOp, dest, lhs, rhs);
    } else if (UnaryOpType unOp = StrToUnOp(op); unOp != UnaryOpType::UnInvalid) {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        std::string src = instJson["args"][0];
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        return std::make_shared<UnaryOp>(unOp, dest, src);
    } else if (op == "jmp") {
        std::string label = instJson["labels"].at(0);
        return std::make_shared<Jump>(std::move(label));
    } else if (op == "br") {
        std::string cond = instJson["args"].at(0);
        std::string ifTrue = instJson["labels"].at(0), ifFalse = instJson["labels"].at(1);
        return std::make_shared<Branch>(cond, ifTrue, ifFalse);
    } else if (op == "call") {
        TypePtr type = instJson.contains("type") ? ParseType(instJson["type"]) : nullptr;
        VarPtr dest = std::make_shared<Variable>(instJson["dest"], type);
        std::string func = instJson["funcs"].at(0);
        std::vector<std::string> args;
        for (const auto& arg : instJson["args"])
            args.push_back(arg);
        return std::make_shared<Call>(dest, func, std::move(args));
    } else if (op == "ret") {
        std::string ret = instJson["args"].at(0);
        return std::make_shared<Return>(ret);
    } else if (op == "print") {
        std::vector<std::string> args;
        for (const auto& arg : instJson["args"]) {
            args.push_back(arg);
        }
        return std::make_shared<Print>(std::move(args));
    } else if (op == "id") {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        std::string src = instJson["args"].at(0);
        return std::make_shared<Id>(dest, src);
    } else if (op == "alloc") {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        std::string src = instJson["args"].at(0);
        return std::make_shared<Alloc>(dest, src);
    } else if (op == "free") {
        std::string src = instJson["args"].at(0);
        return std::make_shared<Free>(src);
    } else if (op == "load") {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        std::string ptr = instJson["args"].at(0);
        return std::make_shared<Load>(dest, ptr);
    } else if (op == "store") {
        std::string ptr = instJson["args"].at(0), val = instJson["args"].at(1);
        return std::make_shared<Store>(ptr, val);
    } else if (op == "ptradd") {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        std::string ptr = instJson["args"].at(0), offset = instJson["args"].at(1);
        return std::make_shared<PtrAdd>(dest, ptr, offset);
    } else
        throw std::runtime_error("Unknown instruction: " + instJson.dump());
}

Function::Function(const json& funcJson) {
    if (!funcJson.contains("name")) throw std::runtime_error("funcJson does not contain 'name'");
    this->name = funcJson["name"];
    if (!funcJson.contains("instrs")) throw std::runtime_error("funcJson does not contain 'instrs'");

    if (funcJson.contains("args")) {
        for (const auto& arg : funcJson["args"]) {
            if (!arg["name"].is_string()) throw std::runtime_error("arg does not contain 'name'");
            std::string name = arg["name"];
            TypePtr type = ParseType(arg["type"]);
            this->args.push_back(std::make_shared<Variable>(std::move(name), type));
        }
    }
    if (funcJson.contains("type")) {
        this->retType = ParseType(funcJson["type"]);
    }

    // TODO args, doens't have to exist
    std::vector<InstPtr> instrs;
    for (const auto& instr : funcJson["instrs"]) {
        instrs.push_back(ParseInstr(instr));
    }
    // find all used labels
    std::unordered_set<std::string> usedLabels;
    for (const auto& instr : instrs) {
        if (auto branch = std::dynamic_pointer_cast<Branch>(instr)) {
            usedLabels.insert(branch->ifTrue);
            usedLabels.insert(branch->ifFalse);
        } else if (auto jump = std::dynamic_pointer_cast<Jump>(instr)) {
            usedLabels.insert(jump->target);
        }
    }

    // construct basic blocks
    std::unordered_map<std::string, BBWPtr> label2bb;
    BBPtr curBlock = std::make_shared<BasicBlock>();
    for (const auto& instr : instrs) {
        auto label = std::dynamic_pointer_cast<Label>(instr);
        if (label && usedLabels.contains(label->name)) {
            if (!curBlock->instrs.empty()) {  // create a new block if it starts with a used label
                this->basicBlocks.push_back(curBlock);
                curBlock = std::make_shared<BasicBlock>();
            }
        }
        if (label && curBlock->instrs.empty())  // tag the block with the label
            label2bb[label->name] = curBlock;
        curBlock->instrs.push_back(instr);

        if (instr->isTerminator()) {  // create a new block if it ends with a terminator
            this->basicBlocks.push_back(curBlock);
            curBlock = std::make_shared<BasicBlock>();
        }
    }
    if (!curBlock->instrs.empty()) this->basicBlocks.push_back(curBlock);
    // connect branch and jump instructions to their corresponding basic blocks
    for (auto bb : this->basicBlocks) {
        assert(!bb->instrs.empty() && "Basic block is empty");
        if (auto branch = std::dynamic_pointer_cast<Branch>(bb->instrs.back())) {
            bb->taken = label2bb[branch->ifTrue];
            bb->notTaken = label2bb[branch->ifFalse];
        } else if (auto jump = std::dynamic_pointer_cast<Jump>(bb->instrs.back())) {
            bb->taken = label2bb[jump->target];
        }
    }
    // connect fall-through basic blocks
    for (size_t i = 0; i < this->basicBlocks.size() - 1; i++) {
        auto cur = this->basicBlocks[i], next = this->basicBlocks[i + 1];
        if (cur->instrs.back()->isTerminator() == false) {
            cur->notTaken = next;
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Function& func) {
    os << "@" << func.name;
    if (func.args.size() > 0) {
        os << "(";
        for (size_t i = 0; i < func.args.size(); i++) {
            os << *func.args[i];
            if (i < func.args.size() - 1)
                os << ", ";
        }
        os << ")";
    }
    if (func.retType)
        os << ": " << *func.retType;
    // print basic blocks
    os << " {\n";
    for (const auto& bb : func.basicBlocks) {
        os << *bb;
    }
    os << "}\n";
    return os;
}

}  // namespace ir
