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
    // virtual std::vector<std::string> liveIn() { return {}; }
    // virtual std::vector<std::string> liveOut() { return {}; }

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

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        vars[dest->name] = RuntimeVal(dest->type, val);
        return false;
    }

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
    std::ostream& print(std::ostream& os) const override;

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        int64_t lhsVal = vars[lhs].value;
        int64_t rhsVal = vars[rhs].value;
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
    std::ostream& print(std::ostream& os) const override;

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        int64_t srcVal = vars[src].value;
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

   private:
    VarPtr dest;
    UnaryOpType op;
    std::string src;
};

class Jump : public Instruction {
   public:
    std::string target;

    Jump(std::string target) : target(target) {}
    ~Jump() = default;
    std::ostream& print(std::ostream& os) const override;
    bool isTerminator() const override { return true; }
    ctrlStatus execute([[maybe_unused]] varContext& vars, [[maybe_unused]] HeapManager& heap) override { return true; }

   private:
};

class Branch : public Instruction {
   public:
    std::string cond;
    std::string ifTrue, ifFalse;

    Branch(std::string cond, std::string ifTrue, std::string ifFalse) : cond(cond), ifTrue(ifTrue), ifFalse(ifFalse) {}
    ~Branch() = default;
    std::ostream& print(std::ostream& os) const override;
    bool isTerminator() const override { return true; }
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        return bool(vars[cond].value != 0);
    }

   private:
};

// forward declaration to avoid circular dependency
class Function;
using FuncWPtr = std::weak_ptr<Function>;

class Call : public Instruction {
   public:
    std::string funcName;
    FuncWPtr func;

    Call(VarPtr dest, std::string funcName, std::vector<std::string> args) : funcName(std::move(funcName)), dest(dest), args(std::move(args)) {}
    ~Call() = default;
    std::ostream& print(std::ostream& os) const override;
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override;

   private:
    VarPtr dest;  // might be nullptr
    std::vector<std::string> args;
};

class Return : public Instruction {
   public:
    Return(std::optional<std::string> val) : val(val) {}
    Return(std::string val) : val(std::move(val)) {}
    ~Return() = default;
    std::ostream& print(std::ostream& os) const override;
    bool isTerminator() const override { return true; }
    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        if (val)
            return std::optional<int64_t>(vars[*val].value);  // could be int or bool or ptr<>
        else
            return std::optional<int64_t>(std::nullopt);
    }

   private:
    std::optional<std::string> val;
};

class Print : public Instruction {
   public:
    Print(std::vector<std::string> args) : args(std::move(args)) {}
    ~Print() = default;
    std::ostream& print(std::ostream& os) const override;

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        for (const auto& arg : args)
            std::cout << vars.at(arg).toString() << ' ';
        std::cout << std::endl;
        return false;
    }

   private:
    std::vector<std::string> args;
};

class Id : public Instruction {
   public:
    Id(VarPtr dest, std::string src) : dest(dest), src(src) {}
    ~Id() = default;
    std::ostream& print(std::ostream& os) const override;

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        vars[dest->name] = RuntimeVal(dest->type, vars[src].value);
        return false;
    }

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

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        int64_t runtimeSize = vars[size].value;
        int64_t* ptr = heap.allocate(runtimeSize);
        vars[dest->name] = RuntimeVal(dest->type, reinterpret_cast<int64_t>(ptr));
        return false;
    }

   private:
    VarPtr dest;
    std::string size;
};

class Free : public Instruction {
   public:
    Free(std::string site) : site(site) {}
    ~Free() = default;
    std::ostream& print(std::ostream& os) const override;

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        int64_t* ptr = reinterpret_cast<int64_t*>(vars[site].value);
        heap.deallocate(ptr);
        return false;
    }

   private:
    std::string site;
};

class Load : public Instruction {
   public:
    Load(VarPtr dest, std::string ptr) : dest(dest), ptr(ptr) {}
    ~Load() = default;
    std::ostream& print(std::ostream& os) const override;

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        int64_t* addr = reinterpret_cast<int64_t*>(vars[ptr].value);
        if (heap.boundCheck(addr) == false)
            throw std::runtime_error(std::format("Load: Uninitialized heap location and/or illegal offset: 0x{:x}", reinterpret_cast<uintptr_t>(addr)));
        vars[dest->name] = RuntimeVal(dest->type, *addr);
        return false;
    }

   private:
    VarPtr dest;
    std::string ptr;
};

class Store : public Instruction {
   public:
    Store(std::string ptr, std::string val) : ptr(ptr), val(val) {}
    ~Store() = default;
    std::ostream& print(std::ostream& os) const override;

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        int64_t* addr = reinterpret_cast<int64_t*>(vars[ptr].value);
        if (heap.boundCheck(addr) == false)
            throw std::runtime_error(std::format("Store: Uninitialized heap location and/or illegal offset: 0x{:x}", reinterpret_cast<uintptr_t>(addr)));
        *addr = vars[val].value;
        return false;
    }

   private:
    std::string ptr, val;
};

class PtrAdd : public Instruction {
   public:
    PtrAdd(VarPtr dest, std::string ptr, std::string offset) : dest(dest), ptr(ptr), offset(offset) {}
    ~PtrAdd() = default;
    std::ostream& print(std::ostream& os) const override;

    ctrlStatus execute(varContext& vars, [[maybe_unused]] HeapManager& heap) override {
        int64_t* addr = reinterpret_cast<int64_t*>(vars[ptr].value);
        vars[dest->name] = RuntimeVal(dest->type, reinterpret_cast<int64_t>(addr + vars[offset].value));
        return false;
    }

   private:
    VarPtr dest;
    std::string ptr, offset;
};

BinaryOpType StrToBinOp(const std::string& op);

UnaryOpType StrToUnOp(const std::string& op);

InstPtr ParseInstr(const json& instJson);

}  // namespace ir

#endif  // IR_INSTRUCTION_H