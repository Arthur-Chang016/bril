#ifndef IR_TYPE_H
#define IR_TYPE_H

#include <cassert>
#include <cpptrace/cpptrace.hpp>
#include <format>
#include <iostream>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
using json = nlohmann::json;

namespace ir {
class Type;
using TypePtr = std::shared_ptr<Type>;

std::ostream& operator<<(std::ostream& os, const Type& type);

class Type {
   public:
    Type() = default;
    virtual ~Type() = default;
    virtual std::ostream& print(std::ostream& os) const = 0;
};

class IntType : public Type {
   public:
    IntType() = default;
    ~IntType() = default;
    std::ostream& print(std::ostream& os) const override;
};

class BoolType : public Type {
   public:
    BoolType() = default;
    ~BoolType() = default;
    std::ostream& print(std::ostream& os) const override;
};

class PointerType : public Type {
   public:
    PointerType(TypePtr pointee) : pointee(pointee) {}
    ~PointerType() = default;
    std::ostream& print(std::ostream& os) const override;

   private:
    TypePtr pointee;
};

// with type and value
class RuntimeVal {
   public:
    TypePtr type;
    int64_t value;

    RuntimeVal() = default;
    RuntimeVal(TypePtr type, int64_t value) : type(std::move(type)), value(value) {
        assert(this->type != nullptr && "Type cannot be null");
    }
    ~RuntimeVal() = default;

    std::string toString() const {
        if (auto intType = std::dynamic_pointer_cast<IntType>(type)) {
            return std::to_string(value);
        } else if (auto boolType = std::dynamic_pointer_cast<BoolType>(type)) {
            return value ? "true" : "false";
        } else if (auto ptrType = std::dynamic_pointer_cast<PointerType>(type)) {
            return std::format("0x{:x}", 42);
        } else {
            cpptrace::generate_trace().print();
            if (type == nullptr)
                throw std::runtime_error("Type is null");
            else
                throw std::runtime_error("Unknown type : " + (std::stringstream() << *type).str());
        }
    }
};

using varContext = std::unordered_map<std::string, RuntimeVal>;

using IntTypePtr = std::shared_ptr<IntType>;
using BoolTypePtr = std::shared_ptr<BoolType>;
using PointerTypePtr = std::shared_ptr<PointerType>;

class Variable {
   public:
    std::string name;
    TypePtr type;

    Variable(std::string name, TypePtr type) : name(std::move(name)), type(type) {}
    virtual ~Variable() = default;

    friend std::ostream& operator<<(std::ostream& os, const Variable& var) {
        return os << var.name << ": " << *var.type;
    }

   private:
};

using VarPtr = std::shared_ptr<Variable>;

TypePtr ParseType(const json& typeJson);

}  // namespace ir

#endif  // IR_TYPE_H