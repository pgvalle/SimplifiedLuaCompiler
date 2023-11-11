#include "Token.h"
#include <stdio.h>

Token::Token() {
  name = INVALID;
  attrib = 0;
}

Token::Token(int name_, int attrib_) {
  name = name_;
  attrib = attrib_;
}

bool Token::is_last() const {
  return name == INVALID || name == EOTS;
}

void Token::print() const {
  // simply for pretty printing relops
  static const char* RELOPS[] = {
    "==", "~=", "<", ">", "<=", ">="
  };

  switch (name) {
  case INVALID:
    printf("<ERROR>");
    break;
  case IDENTIFIER:
    printf("<id, %d>", attrib);
    break;
  case RELOP:
    printf("<relop, %s>", RELOPS[attrib - RELOP_BEGIN - 1]);
    break;
  case NUMBER:
    printf("<num, %d>", attrib);
    break;
  case STRING:
    printf("<str, %d>", attrib);
    break;
  case CONCAT:
    printf("<..>");
    break;
  case KW_ENUM_BEGIN ... KW_ENUM_END: // actually BEGIN+1...END-1
    printf("<kw, %s>", KEYWORDS[attrib]);
    break;
  case EOTS:
    printf("<EOTS>");
    break;
  default: // tokens that are a single ascii character
    printf("<%c>", name);
  }

  printf("\n");
}
