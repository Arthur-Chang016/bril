#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <IR/Type.h>

#include <memory>
#include <string>
#include <vector>

namespace ir {

class Instruction {
   public:
    Instruction() = default;
    virtual ~Instruction() = default;
};

using InstPtr = std::shared_ptr<Instruction>;

class Label : public Instruction {
   public:
    Label(std::string name) : name(name) {}
    ~Label() = default;

   private:
    std::string name;
};

class Constant : public Instruction {
   public:
    Constant(int64_t val) : val(val) {}
    ~Constant() = default;

   private:
    VarPtr dest;
    int64_t val;
};

enum BinaryOpType {
    Add,
    Sub,
    Mul,
    Div,
    And,
    Or,
    Eq,
    Lt,
    Gt,
    Le,
    Ge,
};

class BinaryOp : public Instruction {
   public:
    BinaryOp(BinaryOpType op, VarPtr dest, std::string lhs, std::string rhs) : dest(dest), op(op), lhs(lhs), rhs(rhs) {}
    ~BinaryOp() = default;

   private:
    VarPtr dest;
    BinaryOpType op;
    std::string lhs, rhs;
};

enum UnaryOpType {
    Not,
};

class UnaryOp : public Instruction {
   public:
    UnaryOp(UnaryOpType op, VarPtr dest, std::string src) : dest(dest), op(op), src(src) {}
    ~UnaryOp() = default;

   private:
    VarPtr dest;
    UnaryOpType op;
    std::string src;
};

class Jump : public Instruction {
   public:
    Jump(std::string target) : target(target) {}
    ~Jump() = default;

   private:
    std::string target;
};

class Branch : public Instruction {
   public:
    Branch(std::string cond, std::string ifTrue, std::string ifFalse) : cond(cond), ifTrue(ifTrue), ifFalse(ifFalse) {}
    ~Branch() = default;

   private:
    std::string cond, ifTrue, ifFalse;
};

class Call : public Instruction {
   public:
    Call(VarPtr dest, std::string func, std::vector<std::string> args) : dest(dest), func(std::move(func)), args(std::move(args)) {}
    ~Call() = default;

   private:
    VarPtr dest;  // might be nullptr
    std::string func;
    std::vector<std::string> args;
};

class Return : public Instruction {
   public:
    Return(std::string val) : val(std::move(val)) {}
    ~Return() = default;

   private:
    std::string val;
};

class Print : public Instruction {
   public:
    Print(std::vector<std::string> args) : args(std::move(args)) {}
    ~Print() = default;

   private:
    std::vector<std::string> args;
};

}  // namespace ir

#endif  // IR_INSTRUCTION_H