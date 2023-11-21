#include "Parser.h"
#include <algorithm>

Parser::Parser(const char* filepath) : lex(filepath) {}

void Parser::next_token() {
  tk = lex.next_token();
}

bool Parser::TkName_in(const TkNames& names) const {
  return names.find(TkName(tk.name)) != names.end();
}

void Parser::panic(const char* msg, std::list<TkNames>&& sets) {
  // show error
  printf("%lu:%lu: Expected %s. Got <%s>.\n",
    tk.line, tk.column, msg, tk.name_str().c_str());

  // get the union of sets passed
  TkNames all_names;
  for (const auto& names : sets) {
    all_names.insert(names.begin(), names.end());
  }
  // skip tokens until found one that is inside sync set 
  while (!TkName_in(all_names) && tk.name != EOTS) {
    next_token();
  }

  // End of token stream. Simply halt.
  if (tk.name == EOTS) {
    printf("%lu:%lu: Unexpected end of token stream.\n",
      tk.line, tk.column);
    exit(0);
  } else { // Show where we're at now
    printf("\tContinuing from <%s> at %lu:%lu.\n",
      tk.name_str().c_str(), tk.line, tk.column);
  }
}

namespace First {
  extern const TkNames block;
}

void Parser::parse() {
  next_token();
  block();
  while (tk.name != EOTS) {
    panic("statement or end of token stream", { First::block });
    block();
  }  
}
