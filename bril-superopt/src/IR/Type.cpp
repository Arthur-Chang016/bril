#include <IR/Type.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ir {

std::ostream& operator<<(std::ostream& os, const Type& type) {
    return type.print(os);
}

std::ostream& IntType::print(std::ostream& os) const {
    return os << "int";
}

std::ostream& BoolType::print(std::ostream& os) const {
    return os << "bool";
}

std::ostream& PointerType::print(std::ostream& os) const {
    return os << "ptr<" << *this->pointee << ">";
}

TypePtr ParseType(const json& typeJson) {
    if (typeJson.is_string()) {
        std::string type = typeJson;
        if (type == "int")
            return std::make_shared<IntType>();
        else if (type == "bool")
            return std::make_shared<BoolType>();
        else
            throw std::runtime_error("Unknown type: " + type);
    } else if (typeJson.is_object()) {
        if (!typeJson.contains("ptr")) throw std::runtime_error("typeJson does not contain 'ptr'");
        return std::make_shared<PointerType>(ParseType(typeJson["ptr"]));
    } else {
        throw std::runtime_error("Unknown type: " + typeJson.dump());
    }
}

}  // namespace ir
