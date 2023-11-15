#include "Parser.h"

Parser::Parser(const char* filepath) : lex(filepath) {}

void Parser::fetch_next_token() {
  tk = lex.next_token();
}

bool Parser::token_in(const TkNameList& names) const {
  for (TkName name : names) {
    if (name == tk.name) {
      return true;
    }
  }
  return false;
}

void Parser::panic(const char* msg, const TkNameList& names) {
  // show error
  printf("%lu:%lu:Error: Expected %s. Got <%s>.\n",
    tk.line, tk.column, msg, tk.name_str().c_str());
  // skip tokens until found one that is inside sync set
  while (tk.name != EOTS && !token_in(names)) {
    fetch_next_token();
  }
  // End of token stream. Simply halt.
  if (tk.name == EOTS) {
    exit(0);
  } else { // show where we're at now
    printf("\tContinuing from <%s> at %lu:%lu.\n",
      tk.name_str().c_str(), tk.line, tk.column);
  }
}

void Parser::parse() {
  fetch_next_token();
  block();
  // EOTS should be here.
  while (tk.name != EOTS) {
    panic("file ending or statement", {
      // first(block)
      KW_do, KW_while, KW_if, KW_return, KW_break,
      KW_for, KW_local,  KW_function, ID, TkName('('),
      // follow(block)
      EOTS
    });
    block();
  }
}
