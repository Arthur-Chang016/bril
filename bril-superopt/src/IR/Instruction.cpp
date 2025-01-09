#include <IR/Instruction.h>

#include <iostream>

namespace ir {

std::ostream& operator<<(std::ostream& os, const Instruction& instr) {
    return instr.print(os);
}

std::ostream& Label::print(std::ostream& os) const {
    return os << "." << this->name << ":";
}

std::ostream& Constant::print(std::ostream& os) const {
    return os << *this->dest << " = const " << this->val << ";";
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
    os << "call @" << this->func;
    for (const auto& arg : this->args) os << " " << arg;
    return os << ";";
}

std::ostream& Return::print(std::ostream& os) const {
    return os << "ret " << this->val << ";";
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

}  // namespace ir
