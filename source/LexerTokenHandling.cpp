#include "Lexer.h"
#include <string.h>

Token Lexer::next_id_or_kw() {
  const size_t cl = line, cc = column;
  // read and store id
  std::string id;
  for (char c = code[i]; isalnum(c); c = next_char()) {
    id += c;
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
  for (char c = code[i]; isdigit(c); c = next_char()) {
    number = 10 * number + int(c - '0');
  }
  return Token(NUMBER, number, cl, cc);
}

Token Lexer::next_string() {
  const size_t cl = line, cc = column;
  uint8_t state = 0;
  for (char c = next_char(); c; c = next_char()) {
    switch (state) {
    case 0: // reading string
      if (c == '\\') { // control character code comming
        state = 1;
      } else if (c  == '"') {
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
  start:
  skip_spacers();

  const size_t cl = line, cc = column; // current line/column -> cl/cc
  switch (code[i]) {
  case '_':
  case 'a' ... 'z':
  case 'A' ... 'Z':
    return next_id_or_kw();
    break;
  case '0' ... '9':
    return next_number();
  case '"':
    return next_string();
  case '-':
    if (next_char() == '-') { // it's a comment
      skip_comment();
      goto start; // go back to start
    }
    // it's the - sign
    return Token('-', 0, cl, cc);
  case '.':
    if (next_char() == '.') { // ..
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
    return Token(EOTS, 0, cl, cc);
  default:
    // tokens of "size" 1 which don't match the beginning of any other token
    for (char c : "(){}[]+*/^:,;") {
      if (c == code[i]) {
        next_char();
        return Token(c, 0, cl, cc);
      }
    }
  }
  // nothing that should be recognized.
  skip_invalid();
  return Token(INVALID, 0, cl, cc);
}
