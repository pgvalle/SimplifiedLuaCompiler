#pragma once

#include "Lexer.h"
#include <initializer_list>

#define parser_log printf

class Parser {
private:
  Lexer lex;
  Token tk;

  void fetch_next_token();

  using TkNameList = std::initializer_list<TkName>;
  bool token_in(const TkNameList& symbols) const;
  void log_and_skip_error(const TkNameList& symbols);

  void block();

  void do_statement();
  void while_statement();
  void if_statement();
  void return_statement();
  void for_statement();
  void decl_statement();
  void statement();
  
  void function();
  void expression();
  void variable();

  void field();

  void expressions();
  void variables();
  void fields();
  void arguments();
  void identifiers();

public:
  Parser(const char* filepath);

  void parse();
};
