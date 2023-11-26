#pragma once

#include "Lexer.h"
#include <list>

class Parser {
public:
  Parser(const char* filepath);
  void parse();

private:
  Lexer lex;
  Token tk;

  void block();

  void function();

  void do_statement();
  void if_statement();
  void for_statement();
  void decl_statement();
  void statement();

  void prefix_expression();
  
  void expression();
  void expression2();
  void expressions();

  void variable();
  void variable2();
  void variables();

  void field();
  
  void identifiers();


  void next_token();
  
  bool TkName_in(const TkNames& names) const;
  void panic(const char* msg, std::list<TkNames>&& sets);
};
