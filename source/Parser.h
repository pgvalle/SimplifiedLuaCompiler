#pragma once

#include "Lexer.h"
#include <vector>

class Parser {
private:
  Lexer lex;
  Token tk;

  void log_token_name(TokenName name) const;
  void log_and_skip_error(std::vector<TokenName>&& sync_set);

  // all symbols as procedures here
  void arguments();
  void identifiers();

public:
  Parser(const char* filepath);

  void parse();
};