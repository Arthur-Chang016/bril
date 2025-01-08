#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <memory>

namespace ir {

// TODO move functions to .cpp

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
    std::ostream& print(std::ostream& os) const {
        return os << "int";
    }
};

class BoolType : public Type {
   public:
    BoolType() = default;
    ~BoolType() = default;
    std::ostream& print(std::ostream& os) const {
        return os << "bool";
    }
};

class PointerType : public Type {
   public:
    PointerType(TypePtr pointee) : pointee(pointee) {}
    ~PointerType() = default;
    std::ostream& print(std::ostream& os) const {
        return os << "ptr<" << *this->pointee << ">";
    }

   private:
    TypePtr pointee;
};

using IntTypePtr = std::shared_ptr<IntType>;
using BoolTypePtr = std::shared_ptr<BoolType>;
using PointerTypePtr = std::shared_ptr<PointerType>;

class Variable {
   public:
    Variable(std::string name, TypePtr type) : name(std::move(name)), type(type) {}
    virtual ~Variable() = default;

    friend std::ostream& operator<<(std::ostream& os, const Variable& var) {
        return os << var.name << ": " << *var.type;
    }

   private:
    std::string name;
    TypePtr type;
};

using VarPtr = std::shared_ptr<Variable>;

class Instruction {
   public:
    Instruction() = default;
    virtual ~Instruction() = default;

    // Add pure virtual member functions here
    virtual void execute() = 0;
};

using InstPtr = std::shared_ptr<Instruction>;

}  // namespace ir

#endif  // IR_INSTRUCTION_H