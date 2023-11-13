#include "Parser.h"

Parser::Parser(const char* filepath) : lex(filepath) {}

bool Parser::find_token(const TkNameList& symbols) const {
  for (TkName symbol : symbols) {
    if (symbol == tk.name) {
      return true;
    }
  }
  return tk.name == EOTS;
}

void Parser::log_and_skip_error(TkNameList&& symbols) {
  // print what token we actually got
  printf("\n       Got "); tk.print();
  // skip tokens until found one that is inside sync set
  do {
    fetch_next_token();
  } while (!find_token(symbols));
  // show where we're at now
  printf("       Continuing from "); tk.print();
}

void Parser::parse() {
  fetch_next_token(); // first token ready
  block();
}
