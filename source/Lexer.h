#pragma once

#include "Token.h"
#include <stdlib.h>
#include <string>
#include <unordered_map>

class Lexer {
private:
  char* code;
  size_t i, line, column;
  std::unordered_map<std::string, size_t> symbols;

  char next_char();
  
  void skip_spacers();
  void skip_comment();

  Token next_id_or_kw();
  Token next_number();
  Token next_string();

public:
  Lexer(const char* filepath);
  ~Lexer();

  Token next_token();
};
