#pragma once

#include <set>
#include "Keywords.h"

#define KW(kw) KW_##kw
// the remaining token names that are not here are ascii characters
enum TkName {
  INVALID = 0, ID, RELOP, NUMBER, STRING, // basic stuff
  CONCAT, // particular operator
  // keywords
  KW_ENUM_BEGIN = 200,
    KW_LIST
  KW_ENUM_END,
  EOTS // End Of Token Stream
};
#undef KW
typedef std::set<TkName> TkNames;

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

  std::string name_str() const;
};
