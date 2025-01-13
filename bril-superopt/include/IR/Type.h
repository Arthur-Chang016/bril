#ifndef IR_TYPE_H
#define IR_TYPE_H

#include <cassert>
#include <cpptrace/cpptrace.hpp>
#include <format>
#include <iostream>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <optional>
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
    virtual bool operator==(const Type& other) const = 0;
    virtual std::size_t computeHash() const = 0;

    std::size_t hash() const {
        if (!hashValue) hashValue = computeHash();
        return *hashValue;
    }

   private:
    mutable std::optional<std::size_t> hashValue;
};

class IntType : public Type {
   public:
    IntType() = default;
    ~IntType() = default;
    std::ostream& print(std::ostream& os) const override;

    bool operator==(const Type& other) const override {
        return dynamic_cast<const IntType*>(&other) != nullptr;
    };

    std::size_t computeHash() const override {
        return typeid(IntType).hash_code();
    }
};

class BoolType : public Type {
   public:
    BoolType() = default;
    ~BoolType() = default;
    std::ostream& print(std::ostream& os) const override;

    bool operator==(const Type& other) const override {
        return dynamic_cast<const BoolType*>(&other) != nullptr;
    }

    std::size_t computeHash() const override {
        return typeid(BoolType).hash_code();
    }
};

class PointerType : public Type {
   public:
    PointerType(TypePtr pointee) : pointee(pointee) {}
    ~PointerType() = default;
    std::ostream& print(std::ostream& os) const override;

    bool operator==(const Type& other) const override {
        if (auto otherPtr = dynamic_cast<const PointerType*>(&other)) {
            return *pointee == *otherPtr->pointee;
        }
        return false;
    }

    std::size_t computeHash() const override {
        auto seed = typeid(PointerType).hash_code();
        hash_combine(seed, pointee->hash());
        return seed;
    }

   private:
    const TypePtr pointee;

    static void hash_combine(std::size_t& seed, std::size_t hash) {
        seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};

struct TypePtrHash {
    std::size_t operator()(const TypePtr& type) const {
        if (type == nullptr)
            return 0;
        return type->hash();
    }
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
    const std::string name;
    const TypePtr type;

    Variable(std::string name, TypePtr type) : name(std::move(name)), type(type) {
        assert(type != nullptr && "Type cannot be null");
    }
    virtual ~Variable() = default;

    friend std::ostream& operator<<(std::ostream& os, const Variable& var) {
        return os << var.name << ": " << *var.type;
    }

    bool operator==(const Variable& other) const {
        return name == other.name && *type == *other.type;
    }

   private:
};

using VarPtr = std::shared_ptr<Variable>;

TypePtr ParseType(const json& typeJson);

}  // namespace ir

namespace std {
template <>
struct hash<ir::Variable> {
    std::size_t operator()(const ir::Variable& var) const {
        std::size_t seed = std::hash<std::string>{}(var.name);
        hash_combine(seed, ir::TypePtrHash{}(var.type));
        return seed;
    }

   private:
    void hash_combine(std::size_t& seed, std::size_t hash) const {
        seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};

}  // namespace std

#endif  // IR_TYPE_H