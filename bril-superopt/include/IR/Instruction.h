#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <IR/Heap.h>
#include <IR/Type.h>

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace ir {

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;

std::ostream& operator<<(std::ostream& os, const Instruction& instr);

// union of {return value} and {branch taken/not taken}
class ctrlStatus {
   public:
    ctrlStatus(std::optional<int64_t> ret) : status(ret) {}
    ctrlStatus(bool taken) : status(taken) {}

    bool retValid() const {
        return std::holds_alternative<std::optional<int64_t>>(status);
    }

    std::optional<int64_t> getRet() const {
        return std::get<std::optional<int64_t>>(status);
    }

    bool getTaken() const {
        return std::get<bool>(status);
    }

    ctrlStatus(const ctrlStatus& other) = default;

    ctrlStatus& operator=(const ctrlStatus& other) {
        status = other.status;
        return *this;
    }

   private:
    std::variant<std::optional<int64_t>, bool> status;
};

class Instruction {
   public:
    Instruction() = default;
    virtual ~Instruction() = default;
    virtual std::ostream& print(std::ostream& os) const = 0;
    virtual bool isTerminator() const { return false; }
    virtual std::vector<Variable> liveIn() { return {}; }
    virtual std::vector<Variable> liveOut() { return {}; }

    // return int64_t only when it's 'ret'
    virtual ctrlStatus execute([[maybe_unused]] varContext& vars, [[maybe_unused]] HeapManager& heap) {
        return false;  // default fall-through
    }
};

class Label : public Instruction {
   public:
    std::string name;

    Label(std::string name) : name(name) {}
    ~Label() = default;
    std::ostream& print(std::ostream& os) const override;

   private:
};

class Constant : public Instruction {
   public:
    Constant(VarPtr dest, int64_t val) : dest(dest), val(val) {}
    ~Constant() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;
    std::vector<Variable> liveOut() override;

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
    BinaryOp(BinaryOpType op, VarPtr dest, VarPtr lhs, VarPtr rhs) : dest(dest), op(op), lhs(lhs), rhs(rhs) {}
    ~BinaryOp() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;
    std::vector<Variable> liveIn() override;
    std::vector<Variable> liveOut() override;

   private:
    VarPtr dest;
    BinaryOpType op;
    VarPtr lhs, rhs;
};

enum UnaryOpType {
    Not,
    UnInvalid
};

class UnaryOp : public Instruction {
   public:
    UnaryOp(UnaryOpType op, VarPtr dest, VarPtr src) : dest(dest), op(op), src(src) {}
    ~UnaryOp() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    VarPtr dest;
    UnaryOpType op;
    VarPtr src;
};

class Jump : public Instruction {
   public:
    std::string target;

    Jump(std::string target) : target(target) {}
    ~Jump() = default;
    std::ostream& print(std::ostream& os) const override;
    bool isTerminator() const override;
    ctrlStatus execute([[maybe_unused]] varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
};

class Branch : public Instruction {
   public:
    VarPtr cond;
    std::string ifTrue, ifFalse;

    Branch(VarPtr cond, std::string ifTrue, std::string ifFalse) : cond(cond), ifTrue(ifTrue), ifFalse(ifFalse) {}
    ~Branch() = default;
    std::ostream& print(std::ostream& os) const override;
    bool isTerminator() const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;
    std::vector<Variable> liveIn() override;

   private:
};

// forward declaration to avoid circular dependency
class Function;
using FuncWPtr = std::weak_ptr<Function>;

class Call : public Instruction {
   public:
    const VarPtr dest;  // might be nullptr
    const std::string funcName;
    FuncWPtr func;
    const std::vector<std::string> args;
    std::vector<VarPtr> argsVar;

    Call(VarPtr dest, std::string funcName, std::vector<std::string> args) : dest(dest), funcName(std::move(funcName)), args(std::move(args)) {}
    ~Call() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;
    std::vector<Variable> liveIn() override;
    std::vector<Variable> liveOut() override;

   private:
};

class Return : public Instruction {
   public:
    Return(std::optional<std::string> val) : val(val) {}
    Return(std::string val) : val(std::move(val)) {}
    ~Return() = default;
    std::ostream& print(std::ostream& os) const override;
    bool isTerminator() const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    std::optional<std::string> val;
};

class Print : public Instruction {
   public:
    Print(std::vector<std::string> args) : args(std::move(args)) {}
    ~Print() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    std::vector<std::string> args;
};

class Id : public Instruction {
   public:
    Id(VarPtr dest, std::string src) : dest(dest), src(src) {}
    ~Id() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    VarPtr dest;
    std::string src;
};

class Nop : public Instruction {
   public:
    Nop() = default;
    ~Nop() = default;
    std::ostream& print(std::ostream& os) const override;
};

class Alloc : public Instruction {
   public:
    Alloc(VarPtr dest, std::string size) : dest(dest), size(size) {}
    ~Alloc() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    VarPtr dest;
    std::string size;
};

class Free : public Instruction {
   public:
    Free(std::string site) : site(site) {}
    ~Free() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    std::string site;
};

class Load : public Instruction {
   public:
    Load(VarPtr dest, std::string ptr) : dest(dest), ptr(ptr) {}
    ~Load() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    VarPtr dest;
    std::string ptr;
};

class Store : public Instruction {
   public:
    Store(std::string ptr, std::string val) : ptr(ptr), val(val) {}
    ~Store() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    std::string ptr, val;
};

class PtrAdd : public Instruction {
   public:
    PtrAdd(VarPtr dest, std::string ptr, std::string offset) : dest(dest), ptr(ptr), offset(offset) {}
    ~PtrAdd() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    VarPtr dest;
    std::string ptr, offset;
};

std::pair<BinaryOpType, TypePtr> StrToBinOp(const std::string& op);

std::pair<UnaryOpType, TypePtr> StrToUnOp(const std::string& op);

InstPtr ParseInstr(const json& instJson);

}  // namespace ir

#endif  // IR_INSTRUCTION_H