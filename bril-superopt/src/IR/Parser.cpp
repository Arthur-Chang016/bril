#include <IR/Parser.h>

#include <iostream>
#include <string>

namespace ir {

Parser::Parser(std::istream& input) {
    std::string s;
    while (input >> s) {
        std::cout << s << std::endl;
    }
}

}  // namespace ir
