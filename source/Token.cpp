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
  case EOTS:
    return "EOTS";
  default:
    // keyword
    if (KW_ENUM_BEGIN < name && name < KW_ENUM_END) {
      return KEYWORDS[attrib];
    }
    // tokens that are a single ascii character
    // this is for the program not to display a mess if compiled with msvc
    char temp[2] = { (char)name, '\0' };
    return temp;
  }
}
