#include <IR/BasicBlock.h>
#include <IR/Function.h>
#include <IR/Instruction.h>
#include <IR/Type.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <iostream>
#include <memory>

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

// Parsing instruction: {"dest":"inc","op":"const","type":"int","value":1}
// Parsing instruction: {"dest":"v","op":"const","type":"int","value":1000}
// Parsing instruction: {"dest":"max","op":"const","type":"int","value":1000000}
// Parsing instruction: {"dest":"count","op":"const","type":"int","value":0}
// Parsing instruction: {"args":["v"],"dest":"pi","op":"alloc","type":{"ptr":"int"}}
// Parsing instruction: {"args":["v"],"dest":"pp","op":"alloc","type":{"ptr":{"ptr":"int"}}}
// Parsing instruction: {"label":"lbl"}
// Parsing instruction: {"args":["count","inc"],"dest":"count","op":"add","type":"int"}
// Parsing instruction: {"args":["pp","pi"],"op":"store"}
// Parsing instruction: {"args":["pp"],"dest":"pi","op":"load","type":{"ptr":"int"}}
// Parsing instruction: {"args":["count","max"],"dest":"loop","op":"ge","type":"bool"}
// Parsing instruction: {"args":["loop"],"labels":["end","lbl"],"op":"br"}
// Parsing instruction: {"label":"end"}
// Parsing instruction: {"args":["pi"],"op":"free"}
// Parsing instruction: {"args":["pp"],"op":"free"}
// Parsing instruction: {"args":["count"],"op":"print"}
// Parsing instruction: {"args":["n_1","queens","icount","site"],"dest":"icount","funcs":["queen"],"op":"call","type":"int"}
// Parsing instruction: {"args":["one"],"dest":"ite","op":"id","type":"int"}
InstPtr ParseInstr(const json& instJson) {
    std::cout << "Parsing instruction: " << instJson << std::endl;
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

    std::cout << "instrs size: " << instrs.size() << std::endl;

    // construct basic blocks
    BBPtr currentBlock = std::make_shared<BasicBlock>(std::move(instrs));
    // for (const auto& instr : instrs) {
    //     currentBlock->addInstruction(instr);
    //     if (instr->isTerminator()) {
    //         basicBlocks.push_back(currentBlock);
    //         currentBlock = std::make_shared<BasicBlock>();
    //     }
    // }
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

    // TODO
    // print basic blocks
    os << " {\n";
    // for (const auto& bb : func.BBs) {
    //     os << "  ";
    //     bb->print(os);
    //     os << "\n";
    // }
    os << "}\n";
    return os;
}

}  // namespace ir
