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

ctrlStatus Constant::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    vars[dest->name] = RuntimeVal(dest->type, val);
    return false;
}

std::vector<Variable> Constant::liveOut() {
    return {*dest};
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
    }() << " " << this->lhs->name
              << " " << this->rhs->name << ";";
}

std::vector<Variable> BinaryOp::liveIn() {
    return {*lhs, *rhs};
}

std::vector<Variable> BinaryOp::liveOut() {
    return {*dest};
}

ctrlStatus BinaryOp::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    int64_t lhsVal = vars[this->lhs->name].value;
    int64_t rhsVal = vars[this->rhs->name].value;
    int64_t result;
    switch (op) {
        case Add:
            result = lhsVal + rhsVal;
            break;
        case Sub:
            result = lhsVal - rhsVal;
            break;
        case Mul:
            result = lhsVal * rhsVal;
            break;
        case Div:
            result = lhsVal / rhsVal;
            break;
        case And:
            result = lhsVal & rhsVal;
            break;
        case Or:
            result = lhsVal | rhsVal;
            break;
        case Eq:
            result = lhsVal == rhsVal;
            break;
        case Lt:
            result = lhsVal < rhsVal;
            break;
        case Gt:
            result = lhsVal > rhsVal;
            break;
        case Le:
            result = lhsVal <= rhsVal;
            break;
        case Ge:
            result = lhsVal >= rhsVal;
            break;
        default:
            throw std::runtime_error("Invalid binary operator");
    }
    vars[dest->name] = RuntimeVal(dest->type, result);
    return false;  // return false for fall-through
}

std::ostream& UnaryOp::print(std::ostream& os) const {
    return os << *this->dest << " = " << [&]() {
        switch (this->op) {
            case UnaryOpType::Not:
                return "not";
            default:
                throw std::runtime_error("Invalid UnaryOpType: " + std::to_string(static_cast<int>(this->op)));
        }
    }() << " " << this->src->name
              << ";";
}

ctrlStatus UnaryOp::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    int64_t srcVal = vars[this->src->name].value;
    int64_t result;
    switch (op) {
        case Not:
            result = !srcVal;
            break;
        default:
            throw std::runtime_error("Invalid unary operator");
    }
    vars[dest->name] = RuntimeVal(dest->type, result);
    return false;  // return false for fall-through
}

std::ostream& Jump::print(std::ostream& os) const {
    return os << "jmp ." << this->target << ";";
}

bool Jump::isTerminator() const {
    return true;
}

ctrlStatus Jump::execute([[maybe_unused]] varContext& vars, [[maybe_unused]] HeapManager& heap) {
    return true;
}

std::ostream& Branch::print(std::ostream& os) const {
    return os << "br " << this->cond->name << " ." << this->ifTrue << " ." << this->ifFalse << ";";
}

bool Branch::isTerminator() const {
    return true;
}

ctrlStatus Branch::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    return bool(vars[this->cond->name].value != 0);
}

std::vector<Variable> Branch::liveIn() {
    return {*cond};
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

std::vector<Variable> Call::liveIn() {
    std::vector<Variable> liveIn;
    for (const auto& arg : this->argsVar) liveIn.push_back(*arg);
    return liveIn;
}

std::vector<Variable> Call::liveOut() {
    if (this->dest) return {*dest};
    return {};
}

std::ostream& Return::print(std::ostream& os) const {
    return os << "ret" << (this->val ? " " + this->val.value() : "") << ";";
}

bool Return::isTerminator() const {
    return true;
}

ctrlStatus Return::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    if (val)
        return std::optional<int64_t>(vars[*val].value);  // could be int or bool or ptr<>
    else
        return std::optional<int64_t>(std::nullopt);
}

std::ostream& Print::print(std::ostream& os) const {
    os << "print";
    for (const auto& arg : this->args) os << " " << arg;
    return os << ";";
}

ctrlStatus Print::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    for (const auto& arg : this->args)
        std::cout << vars.at(arg).toString() << ' ';
    std::cout << std::endl;
    return false;
}

std::ostream& Id::print(std::ostream& os) const {
    return os << *this->dest << " = id " << this->src << ";";
}

ctrlStatus Id::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    vars[dest->name] = RuntimeVal(dest->type, vars[src].value);
    return false;
}

std::ostream& Nop::print(std::ostream& os) const {
    return os << "nop;";
}

std::ostream& Alloc::print(std::ostream& os) const {
    return os << *this->dest << " = alloc " << this->size << ";";
}

ctrlStatus Alloc::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    int64_t runtimeSize = vars[size].value;
    int64_t* ptr = heap.allocate(runtimeSize);
    vars[dest->name] = RuntimeVal(dest->type, reinterpret_cast<int64_t>(ptr));
    return false;
}

std::ostream& Free::print(std::ostream& os) const {
    return os << "free " << this->site << ";";
}

ctrlStatus Free::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    int64_t* ptr = reinterpret_cast<int64_t*>(vars[site].value);
    heap.deallocate(ptr);
    return false;
}

std::ostream& Load::print(std::ostream& os) const {
    return os << *this->dest << " = load " << this->ptr << ";";
}

ctrlStatus Load::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    int64_t* addr = reinterpret_cast<int64_t*>(vars[ptr].value);
    if (heap.boundCheck(addr) == false)
        throw std::runtime_error(std::format("Load: Uninitialized heap location and/or illegal offset: 0x{:x}", reinterpret_cast<uintptr_t>(addr)));
    vars[dest->name] = RuntimeVal(dest->type, *addr);
    return false;
}

std::ostream& Store::print(std::ostream& os) const {
    return os << "store " << this->ptr << " " << this->val << ";";
}

ctrlStatus Store::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    int64_t* addr = reinterpret_cast<int64_t*>(vars[ptr].value);
    if (heap.boundCheck(addr) == false)
        throw std::runtime_error(std::format("Store: Uninitialized heap location and/or illegal offset: 0x{:x}", reinterpret_cast<uintptr_t>(addr)));
    *addr = vars[val].value;
    return false;
}

std::ostream& PtrAdd::print(std::ostream& os) const {
    return os << *this->dest << " = ptradd " << this->ptr << " " << this->offset << ";";
}

ctrlStatus PtrAdd::execute(varContext& vars, [[maybe_unused]] HeapManager& heap) {
    int64_t* addr = reinterpret_cast<int64_t*>(vars[ptr].value);
    vars[dest->name] = RuntimeVal(dest->type, reinterpret_cast<int64_t>(addr + vars[offset].value));
    return false;
}

// return {BinaryOpType, operand type}
std::pair<BinaryOpType, TypePtr> StrToBinOp(const std::string& op) {
    if (op == "add")
        return {BinaryOpType::Add, std::make_shared<IntType>()};
    else if (op == "sub")
        return {BinaryOpType::Sub, std::make_shared<IntType>()};
    else if (op == "mul")
        return {BinaryOpType::Mul, std::make_shared<IntType>()};
    else if (op == "div")
        return {BinaryOpType::Div, std::make_shared<IntType>()};
    else if (op == "and")
        return {BinaryOpType::And, std::make_shared<BoolType>()};
    else if (op == "or")
        return {BinaryOpType::Or, std::make_shared<BoolType>()};
    else if (op == "eq")
        return {BinaryOpType::Eq, std::make_shared<IntType>()};
    else if (op == "lt")
        return {BinaryOpType::Lt, std::make_shared<IntType>()};
    else if (op == "gt")
        return {BinaryOpType::Gt, std::make_shared<IntType>()};
    else if (op == "le")
        return {BinaryOpType::Le, std::make_shared<IntType>()};
    else if (op == "ge")
        return {BinaryOpType::Ge, std::make_shared<IntType>()};
    else
        return {BinaryOpType::BinInvalid, nullptr};
}

// return {UnaryOpType, operand type}
std::pair<UnaryOpType, TypePtr> StrToUnOp(const std::string& op) {
    if (op == "not")
        return {UnaryOpType::Not, std::make_shared<BoolType>()};
    else
        return {UnaryOpType::UnInvalid, nullptr};
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
    } else if (auto [binOp, argType] = StrToBinOp(op); binOp != BinaryOpType::BinInvalid) {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr lhs = std::make_shared<Variable>(instJson.at("args").at(0), argType);
        VarPtr rhs = std::make_shared<Variable>(instJson.at("args").at(1), argType);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        return std::make_shared<BinaryOp>(binOp, dest, lhs, rhs);
    } else if (auto [unOp, argType] = StrToUnOp(op); unOp != UnaryOpType::UnInvalid) {
        std::string d = instJson["dest"];
        TypePtr type = ParseType(instJson["type"]);
        VarPtr src = std::make_shared<Variable>(instJson.at("args").at(0), argType);
        VarPtr dest = std::make_shared<Variable>(std::move(d), type);
        return std::make_shared<UnaryOp>(unOp, dest, src);
    } else if (op == "jmp") {
        std::string label = instJson["labels"].at(0);
        return std::make_shared<Jump>(std::move(label));
    } else if (op == "br") {
        VarPtr cond = std::make_shared<Variable>(instJson.at("args").at(0), std::make_shared<BoolType>());
        std::string ifTrue = instJson["labels"].at(0), ifFalse = instJson["labels"].at(1);
        return std::make_shared<Branch>(cond, ifTrue, ifFalse);
    } else if (op == "call") {
        TypePtr type = instJson.contains("type") ? ParseType(instJson["type"]) : nullptr;
        VarPtr dest = type ? std::make_shared<Variable>(instJson["dest"], type) : nullptr;
        std::string func = instJson["funcs"].at(0);
        std::vector<std::string> args;
        if (instJson.contains("args"))
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
    } else if (op == "nop") {
        return std::make_shared<Nop>();
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
