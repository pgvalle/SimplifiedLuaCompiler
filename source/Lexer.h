#pragma once

#include "Token.h"
#include <string>
#include <unordered_map>

class Lexer {
public:
  Lexer(const char* filepath);
  ~Lexer();

  Token next_token();

private:
  char* code;
  size_t i, line, column;
  std::unordered_map<std::string, size_t> symbols;

  char next_char();
  
  void skip_invalid();
  void skip_spacers();
  void skip_comment();

  Token next_id_or_kw();
  Token next_number();
  Token next_string();
};
