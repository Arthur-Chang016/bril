#include <IR/Instruction.h>

#include <iostream>

namespace ir {

std::ostream& operator<<(std::ostream& os, const Type& type) {
    return type.print(os);
}

}  // namespace ir
