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
    Constant(VarPtr dest, int64_t val) : dest(dest), val(val) {}
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
    BinInvalid,
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
    UnInvalid
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

class Id : public Instruction {
   public:
    Id(VarPtr dest, std::string src) : dest(dest), src(src) {}
    ~Id() = default;

   private:
    VarPtr dest;
    std::string src;
};

class Alloc : public Instruction {
   public:
    Alloc(VarPtr dest, std::string size) : dest(dest), size(size) {}
    ~Alloc() = default;

   private:
    VarPtr dest;
    std::string size;
};

class Free : public Instruction {
   public:
    Free(std::string site) : site(site) {}
    ~Free() = default;

   private:
    std::string site;
};

class Load : public Instruction {
   public:
    Load(VarPtr dest, std::string ptr) : dest(dest), ptr(ptr) {}
    ~Load() = default;

   private:
    VarPtr dest;
    std::string ptr;
};

class Store : public Instruction {
   public:
    Store(std::string ptr, std::string val) : ptr(ptr), val(val) {}
    ~Store() = default;

   private:
    std::string ptr, val;
};

class PtrAdd : public Instruction {
   public:
    PtrAdd(VarPtr dest, std::string ptr, std::string offset) : dest(dest), ptr(ptr), offset(offset) {}
    ~PtrAdd() = default;

   private:
    VarPtr dest;
    std::string ptr, offset;
};

}  // namespace ir

#endif  // IR_INSTRUCTION_H