#pragma once

#include <cstddef>
#include "Keywords.h"

#define KW(kw) KW_##kw
// the remaining token names that are not here are ascii characters
// and don't match any other token beginning 
enum TkName {
  // basic???
  INVALID = 0, ID, RELOP, NUMBER, STRING,
  
  // ???
  CONCAT,

  KW_ENUM_BEGIN = 200,
    KW_LIST
  KW_ENUM_END,

  EOTS // End Of Token Stream
};
#undef KW

// types of relop tokens
enum RelOp {
  RELOP_BEGIN = 128,
  EQ, NE, LT, GT, LE, GE
};

struct Token {
  int name;
  int attrib;
  size_t line, column;

  Token();
  Token(int name, int attrib, size_t line, size_t column);

  void print() const;
};
