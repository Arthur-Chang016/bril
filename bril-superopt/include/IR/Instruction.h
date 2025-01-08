#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <memory>

namespace ir {

class Type;
using TypePtr = std::shared_ptr<Type>;

class Type {
   public:
    Type() = default;
    virtual ~Type() = default;
    virtual void print(std::ostream& os) const = 0;
};

class IntType : public Type {
   public:
    IntType() = default;
    ~IntType() = default;
    void print(std::ostream& os) const {
        os << "int";
    }
};

class BoolType : public Type {
   public:
    BoolType() = default;
    ~BoolType() = default;
    void print(std::ostream& os) const {
        os << "bool";
    }
};

class PointerType : public Type {
   public:
    PointerType(TypePtr pointee) : pointee(pointee) {}
    ~PointerType() = default;
    void print(std::ostream& os) const {
        os << "ptr<";
        this->pointee->print(os);
        os << ">";
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
    void print(std::ostream& os) const {
        os << this->name << ": ";
        if (this->type)  // TODO should never be empty
            this->type->print(os);
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