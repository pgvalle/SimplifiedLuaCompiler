#pragma once

#include "Keywords.h"

#define KW(kw) KW_##kw
// the remaining token names that are not here are ascii characters
// and don't match any other token beginning 
enum TokenName {
  // basic???
  INVALID = 0, IDENTIFIER, RELOP, NUMBER, STRING,
  
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

  Token();
  Token(int name_, int attrib_);

  void print() const;
};