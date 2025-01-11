#include <IR/Function.h>
#include <IR/Instruction.h>

#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
using json = nlohmann::json;

namespace ir {

std::ostream& operator<<(std::ostream& os, const Instruction& instr) {
    return instr.print(os);
}

std::ostream& Label::print(std::ostream& os) const {
    return os << "." << this->name << ":";
}

std::ostream& Constant::print(std::ostream& os) const {
    if (auto intType = std::dynamic_pointer_cast<IntType>(this->dest->type)) {
        return os << *this->dest << " = const " << this->val << ";";
    } else if (auto boolType = std::dynamic_pointer_cast<BoolType>(this->dest->type)) {
        return os << *this->dest << " = const " << (this->val ? "true" : "false") << ";";
    } else {
        std::stringstream ss;
        ss << *this->dest->type;
        throw std::runtime_error("Invalid type for constant: " + ss.str());
    }
}

std::ostream& BinaryOp::print(std::ostream& os) const {
    return os << *this->dest << " = " << [&]() {
        switch (this->op) {
            case BinaryOpType::Add:
                return "add";
            case BinaryOpType::Sub:
                return "sub";
            case BinaryOpType::Mul:
                return "mul";
            case BinaryOpType::Div:
                return "div";
            case BinaryOpType::And:
                return "and";
            case BinaryOpType::Or:
                return "or";
            case BinaryOpType::Eq:
                return "eq";
            case BinaryOpType::Lt:
                return "lt";
            case BinaryOpType::Gt:
                return "gt";
            case BinaryOpType::Le:
                return "le";
            case BinaryOpType::Ge:
                return "ge";
            default:
                throw std::runtime_error("Invalid BinaryOpType: " + std::to_string(static_cast<int>(this->op)));
        }
    }() << " " << this->lhs
              << " " << this->rhs << ";";
}

std::ostream& UnaryOp::print(std::ostream& os) const {
    return os << *this->dest << " = " << [&]() {
        switch (this->op) {
            case UnaryOpType::Not:
                return "not";
            default:
                throw std::runtime_error("Invalid UnaryOpType: " + std::to_string(static_cast<int>(this->op)));
        }
    }() << " " << this->src
              << ";";
}

std::ostream& Jump::print(std::ostream& os) const {
    return os << "jmp ." << this->target << ";";
}

std::ostream& Branch::print(std::ostream& os) const {
    return os << "br " << this->cond << " ." << this->ifTrue << " ." << this->ifFalse << ";";
}

std::ostream& Call::print(std::ostream& os) const {
    if (this->dest) os << *this->dest << " = ";
    os << "call @" << this->funcName;
    for (const auto& arg : this->args) os << " " << arg;
    return os << ";";
}

ctrlStatus Call::execute(varContext& vars, HeapManager& heap) {
    varContext newVars;
    auto func = this->func.lock();
    // construct newVars for args
    for (size_t i = 0; i < args.size(); ++i) {
        newVars[func->args[i]->name] = vars[this->args[i]];
    }
    std::optional<int64_t> ret = func->execute(newVars, heap);
    if (ret) vars[dest->name] = RuntimeVal(dest->type, *ret);
    return false;
}

std::ostream& Return::print(std::ostream& os) const {
    return os << "ret" << (this->val ? " " + this->val.value() : "") << ";";
}

std::ostream& Print::print(std::ostream& os) const {
    os << "print";
    for (const auto& arg : this->args) os << " " << arg;
    return os << ";";
}

std::ostream& Id::print(std::ostream& os) const {
    return os << *this->dest << " = id " << this->src << ";";
}

std::ostream& Alloc::print(std::ostream& os) const {
    return os << *this->dest << " = alloc " << this->size << ";";
}

std::ostream& Free::print(std::ostream& os) const {
    return os << "free " << this->site << ";";
}

std::ostream& Load::print(std::ostream& os) const {
    return os << *this->dest << " = load " << this->ptr << ";";
}

std::ostream& Store::print(std::ostream& os) const {
    return os << "store " << this->ptr << " " << this->val << ";";
}

std::ostream& PtrAdd::print(std::ostream& os) const {
    return os << *this->dest << " = ptradd " << this->ptr << " " << this->offset << ";";
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
        std::string lhs = instJson.at("args").at(0), rhs = instJson.at("args").at(1);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        return std::make_shared<BinaryOp>(binOp, dest, lhs, rhs);
    } else if (UnaryOpType unOp = StrToUnOp(op); unOp != UnaryOpType::UnInvalid) {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        std::string src = instJson.at("args").at(0);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        return std::make_shared<UnaryOp>(unOp, dest, src);
    } else if (op == "jmp") {
        std::string label = instJson["labels"].at(0);
        return std::make_shared<Jump>(std::move(label));
    } else if (op == "br") {
        std::string cond = instJson.at("args").at(0);
        std::string ifTrue = instJson["labels"].at(0), ifFalse = instJson["labels"].at(1);
        return std::make_shared<Branch>(cond, ifTrue, ifFalse);
    } else if (op == "call") {
        TypePtr type = instJson.contains("type") ? ParseType(instJson["type"]) : nullptr;
        VarPtr dest = std::make_shared<Variable>(instJson["dest"], type);
        std::string func = instJson["funcs"].at(0);
        std::vector<std::string> args;
        for (const auto& arg : instJson.at("args"))
            args.push_back(arg);
        return std::make_shared<Call>(dest, func, std::move(args));
    } else if (op == "ret") {
        std::optional<std::string> ret = std::nullopt;
        if (instJson.contains("args")) ret = instJson.at("args").at(0);
        return std::make_shared<Return>(ret);
    } else if (op == "print") {
        std::vector<std::string> args;
        for (const auto& arg : instJson.at("args")) {
            args.push_back(arg);
        }
        return std::make_shared<Print>(std::move(args));
    } else if (op == "id") {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        std::string src = instJson.at("args").at(0);
        return std::make_shared<Id>(dest, src);
    } else if (op == "alloc") {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        std::string src = instJson.at("args").at(0);
        return std::make_shared<Alloc>(dest, src);
    } else if (op == "free") {
        std::string src = instJson.at("args").at(0);
        return std::make_shared<Free>(src);
    } else if (op == "load") {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        std::string ptr = instJson.at("args").at(0);
        return std::make_shared<Load>(dest, ptr);
    } else if (op == "store") {
        std::string ptr = instJson.at("args").at(0), val = instJson.at("args").at(1);
        return std::make_shared<Store>(ptr, val);
    } else if (op == "ptradd") {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        std::string ptr = instJson.at("args").at(0), offset = instJson.at("args").at(1);
        return std::make_shared<PtrAdd>(dest, ptr, offset);
    } else
        throw std::runtime_error("Unknown instruction: " + instJson.dump());
}

}  // namespace ir
