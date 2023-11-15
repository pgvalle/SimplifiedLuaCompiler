#include "Token.h"
#include <cstdio>

Token::Token() {
  name = INVALID;
  attrib = 0;
  line = 1;
  column = 1;
}

Token::Token(int name_, int attrib_, size_t line_, size_t column_) {
  name = name_;
  attrib = attrib_;
  line = line_;
  column = column_;
}

std::string Token::name_str() const {
  switch (name) {
  case INVALID:
    return "ERROR";
  case ID:
    return "id";
  case RELOP:
    return "relop";
  case NUMBER:
    return "number";
  case STRING:
    return "string";
  case CONCAT:
    return "..";
  case KW_ENUM_BEGIN ... KW_ENUM_END: // actually BEGIN+1...END-1
    return KEYWORDS[attrib];
  case EOTS:
    return "EOTS";
  default: // tokens that are a single ascii character
    char c = name;
    return &c;
  }
}
