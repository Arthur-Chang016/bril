#ifndef IR_PARSER_H
#define IR_PARSER_H

#include <IR/Program.h>

#include <istream>

namespace ir {

ProgramPtr parse(std::istream& input);

}  // namespace ir

#endif  // IR_PARSER_H