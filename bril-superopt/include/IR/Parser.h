#ifndef IR_PARSER_H
#define IR_PARSER_H

#include <istream>

namespace ir {

class Parser {
   public:
    Parser() = default;
    ~Parser() = default;

    Parser(std::istream& input);
};

}  // namespace ir

#endif  // IR_PARSER_H