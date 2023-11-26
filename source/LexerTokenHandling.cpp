#include "Lexer.h"
#include <cstdint>

bool is_part_of_id(char c) {
  return isalnum(c) || c == '_';
}

Token Lexer::next_id_or_kw() {
  const size_t cl = line, cc = column; // current line/column -> cl/cc
  // read and store id
  std::string id;
  for (; is_part_of_id(code[i]); next_char()) {
    id += code[i];
  }
  // check if it's a keyword. It might be.
  const size_t kw_enum_len = KW_ENUM_END - KW_ENUM_BEGIN - 1;
  for (size_t j = 0; j < kw_enum_len; j++) {
    if (id == KEYWORDS[j]) {
      return Token(j + KW_ENUM_BEGIN + 1, j, cl, cc);
    }
  }
  // put id in symbol table if not there
  static size_t symbol_table_line = 1;
  if (symbols.find(id) == symbols.end()) {
    symbols.insert({ id, symbol_table_line++ });
  }
  return Token(ID, symbols[id], cl, cc);
}

Token Lexer::next_number() {
  const size_t cl = line, cc = column;
  int number = 0;
  for (; isdigit(code[i]); next_char()) {
    number = 10 * number + int(code[i] - '0');
  }
  return Token(NUMBER, number, cl, cc);
}

Token Lexer::next_string() {
  const size_t cl = line, cc = column; // current line/column -> cl/cc
  uint8_t state = 0;
  for (next_char(); code[i]; next_char()) {
    switch (state) {
    case 0: // reading string
      if (code[i] == '\\') { // control character code comming
        state = 1;
      } else if (code[i]  == '"') {
        next_char();
        return Token(STRING, 0, cl, cc);
      }
      break;
    case 1: // skip control character
      state = 0;
      break;
    }
  }
  return Token(INVALID, 0, cl, cc);
}

Token Lexer::next_token() {
  const size_t a = line, b = column; // fix eots token location bug
  start:
  skip_spacers();

  const size_t cl = line, cc = column; // current line/column -> cl/cc
  switch (code[i]) {
  case '"':
    return next_string();
  case '-':
    if (next_char() == '-') {
      skip_comment();
      goto start;
    }
    return Token('-', 0, cl, cc);
  case '.':
    if (next_char() == '.') {
      next_char();
      return Token(CONCAT, 0, cl, cc);
    }
    return Token('.', 0, cl, cc);
  case '=':
    if (next_char() == '=') {
      next_char();
      return Token(RELOP, EQ, cl, cc);
    }
    return Token('=', 0, cl, cc);
  case '<':
    if (next_char() == '=') {
      next_char();
      return Token(RELOP, LE, cl, cc);
    }
    return Token(RELOP, '<', cl, cc);
  case '>':
    if (next_char() == '=') {
      next_char();
      return Token(RELOP, GE, cl, cc);
    }
    return Token(RELOP, '>', cl, cc);
  case '~':
    if (next_char() == '=') {
      next_char();
      return Token(RELOP, NE, cl, cc);
    }
    break;
  case '\0':
    return Token(EOTS, 0, a, b);
  default:
    // id/keyword
    if (isalpha(code[i]) || code[i] == '_') {
      return next_id_or_kw();
    }
    // number
    if (isdigit(code[i])) {
      return next_number();
    }
    // tokens of "size" 1 which don't match the beginning of any other token
    const std::string chars = "(){}[]+*/^:,;";
    if (chars.find(code[i]) != chars.npos) {
      next_char();
      return Token(code[i - 1], 0, cl, cc);
    }
  }
  // nothing that should be recognized.
  skip_invalid();
  return Token(INVALID, 0, cl, cc);
}
