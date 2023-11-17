#pragma once

#include "Lexer.h"
#include <vector>

#define parser_log printf

class Parser {
private:
  Lexer lex;
  Token tk;

  void fetch_next_token();

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
  void expression2();
  void expressions();

  void variable();
  void variables();

  void field();
  void fields();
  
  void arguments();
  
  void identifiers();

  using TkNameList = std::initializer_list<TkName>;
  bool token_in(const TkNameList& tokens) const;
  void panic(const char* msg, const TkNameList& tokens);

public:
  Parser(const char* filepath);

  void parse();
};
