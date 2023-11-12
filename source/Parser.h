#pragma once

#include "Lexer.h"
#include <initializer_list>

class Parser {
private:
  using TokenNameList = std::initializer_list<TokenName>;

  Lexer lex;
  Token tk;

  void log_token_name(TokenName name) const;
  bool find_token(const TokenNameList& syncs);
  void log_and_skip_error(TokenNameList&& syncs);

  // all symbols as procedures here
  void variable_or_call();
  void block();
  void statement();
  void expressions();
  void arguments();
  void identifiers();

public:
  Parser(const char* filepath);

  void parse();
};
