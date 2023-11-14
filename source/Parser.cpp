#include "Parser.h"

Parser::Parser(const char* filepath) : lex(filepath) {}

void Parser::fetch_next_token() {
  tk = lex.next_token();
  tk.print();
}

bool Parser::token_in(const TkNameList& symbols) const {
  for (TkName symbol : symbols) {
    if (symbol == tk.name) {
      return true;
    }
  }
  return false;
}

void Parser::log_and_skip_error(const TkNameList& symbols) {
  // show what token we actually got
  printf(" instead of "); tk.print();
  // skip tokens until found one that is inside sync set
  while (tk.name != EOTS && !token_in(symbols)) {
    fetch_next_token();
  }
  // End of token stream. Simply halt.
  if (tk.name == EOTS) {
    printf("       Syntax analysis stopped.\n");
    exit(0);
  } else {
    // show where we're at now
    printf("       Continuing from "); tk.print();
  }
}

void Parser::parse() {
  fetch_next_token(); // first token ready
  block();
  // must be EOTS here. If not we still got to keep going.
  while (tk.name != EOTS) {
    printf("Error: Expected <EOTS>"); // Follow
    log_and_skip_error({
      KW_do, KW_while, KW_if, KW_return,
      KW_break, KW_for,KW_local,
      KW_function, ID, EOTS, TkName('(')
    });
    block();
  }
}
