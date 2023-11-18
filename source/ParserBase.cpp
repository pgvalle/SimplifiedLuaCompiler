#include "Parser.h"
#include <unordered_map>
#include <string>

Parser::Parser(const char* filepath) : lex(filepath) {}

void Parser::fetch_next_token() {
  tk = lex.next_token();
}

bool Parser::TkName_in(const TkNames& names) const {
  return names.find(TkName(tk.name)) != names.end();
}

void Parser::panic(const char* msg, const TkNames& names) {
  // show error
  printf("%lu:%lu:Error: Expected %s. Got <%s>.\n",
    tk.line, tk.column, msg, tk.name_str().c_str());
  // skip tokens until found one that is inside sync set
  while (tk.name != EOTS && !TkName_in(names)) {
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
}
