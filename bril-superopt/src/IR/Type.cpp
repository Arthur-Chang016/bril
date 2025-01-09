#include <IR/Type.h>

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

}  // namespace ir
