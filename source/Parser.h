#pragma once

#include "Lexer.h"
#include <initializer_list>

class Parser {
private:
  Lexer lex;
  Token tk;

  void fetch_next_token() {
    tk = lex.next_token();
  }

  using TkNameList = std::initializer_list<TkName>;
  bool find_token(const TkNameList& symbols) const;
  void log_and_skip_error(TkNameList&& symbols);

  void variable_or_call();

  void block();

  void do_statement();
  void while_statement();
  void if_statement();
  void return_statement();
  void for_statement();
  void decl_statement();
  void statement();
  
  void function();
  void expressions();
  void expression();

  void field();

  void arguments();

  void identifiers();

public:
  Parser(const char* filepath);

  void parse();
};
