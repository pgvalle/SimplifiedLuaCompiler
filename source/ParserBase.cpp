#include "Parser.h"
#include <algorithm>

Parser::Parser(const char* filepath) : lex(filepath) {}

void Parser::next_token() {
  tk = lex.next_token();
}

bool Parser::TkName_in(const TkNames& names) const {
  return names.find(TkName(tk.name)) != names.end();
}

Parser::TkNames sets_union(const std::list<Parser::TkNames>& sets) {
  Parser::TkNames all_names;
  for (const auto& names : sets) {
    all_names.insert(names.begin(), names.end());
  }
  return all_names;
}

void Parser::panic(const char* msg, std::list<TkNames>&& sets) {
  // show error
  printf("%lu:%lu:Error: Expected %s. Got <%s>.\n",
    tk.line, tk.column, msg, tk.name_str().c_str());

  // skip tokens until found one that is inside sync set 
  const TkNames all_names = sets_union(sets);
  while (!TkName_in(all_names) && tk.name != EOTS) {
    next_token();
  }

  // End of token stream. Simply halt.
  if (tk.name == EOTS) {
    exit(0);
  } else { // Show where we're at now
    printf("\tContinuing from <%s> at %lu:%lu.\n",
      tk.name_str().c_str(), tk.line, tk.column);
  }
}

void Parser::parse() {
  next_token();
  block();
}
