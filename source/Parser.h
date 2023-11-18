#pragma once

#include "Lexer.h"
#include <set>

class Parser {
private:
  Lexer lex;
  Token tk;

  void block();

  void do_statement();
  void while_statement();
  void if_statement();
  void return_statement();
  void for_statement();
  void decl_statement();
  void statement();
  
  void function();

  void prefix_expression();
  void prefix_expression2();
  
  void expression();
  void expression2();
  void expressions();

  void variable();
  void variable2();
  void variables();

  void field();
  void fields();
  
  void identifiers();

  void fetch_next_token();

  using TkNames = std::set<TkName>;
  bool TkName_in(const TkNames& names) const;
  void panic(const char* msg, const TkNames& names);

public:
  Parser(const char* filepath);

  void parse();
};
