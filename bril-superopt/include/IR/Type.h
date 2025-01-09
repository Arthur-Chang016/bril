#ifndef IR_TYPE_H
#define IR_TYPE_H

#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <ostream>
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