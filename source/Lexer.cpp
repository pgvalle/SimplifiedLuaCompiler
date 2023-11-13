#include "Lexer.h"
#include <string.h>

#define log printf

Lexer::Lexer(const char* filepath) {
  FILE* file = fopen(filepath, "r");
  if (file == nullptr) {
    throw "error opening " + std::string(filepath);
  }

  // get file size
  fseek(file, 0, SEEK_END);
  const size_t file_len = ftell(file);
  fseek(file, 0, SEEK_SET);
  // read all content
  code = new char[file_len]();
  fread(code, 1, file_len - 1, file);
  fclose(file);

  i = 0;
  line = 1;
  column = 1;
}

Lexer::~Lexer() {
  delete code;
}

char Lexer::next_char() {
  const bool eol = (code[++i] == '\n');
  // update line and column accordingly
  line += size_t(eol);
  column = (eol ? 0 : (column + 1));
  return code[i];
}

void Lexer::skip_invalid() {
  const std::string str = " \n\t\0\"_(){}[]+*/^:,;-~=<>";
  char c;
  do {
    c = next_char();
  } while (!isalnum(c) && str.find(c) == str.npos);
}

bool is_spacer(char c) {
  return (c == ' ' || (iscntrl(c) && c != '\0'));
}

void Lexer::skip_spacers() {
  char c = code[i];
  while (is_spacer(c)) {
    c = next_char();
  }
}

void Lexer::skip_comment() {
  uint8_t comment_nesting = 0, state = 0;
  for (char c = next_char(); c; c = next_char()) {
    switch (state) {
    case 0: // start
      if (c == '[') { // may be a big comment
        state = 1;
      } else if (c == '\n') { // end of comment
        next_char();
        return;
      } else { // one line comment
        state = 2;
      }
      break;
    case 1: // check if big comment
      if (c == '[') { // it's a big comment
        comment_nesting = 1;
        state = 3;
      } else if (c == '\n') { // end of comment
        next_char();
        return;
      } else { // one line comment
        state = 2;
      }
      break;
    case 2:  // skip one line comment
      if (c == '\n') {
        next_char();
        return;
      }
      break;
    case 3: // big comment. Nesting may increase or decrease
      if (c == '[') { // nesting may increase
        state = 4;
      } else if (c == ']') { // nesting may decrease
        state = 5;
      }
      break;
    case 4: // Nesting may increase
      if (c == '[') { // nesting increased
        comment_nesting++;
        state = 3;
      } else if (c == ']') { // nesting may decrease
        state = 5;
      } else {
        state = 3;
      }
      break;
    case 5: // Nesting may decrease
      if (c == ']') { // nesting decreased
        if (--comment_nesting == 0) { // end of big comment
          next_char();
          return;
        }
        state = 3;
      } else if (c == '[') { // nesting may increase
        state = 4;
      } else {
        state = 3;
      }
      break;
    }
  }
}

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
    if (strcmp(id.c_str(), KEYWORDS[j]) == 0) {
      return Token(j + KW_ENUM_BEGIN + 1, j, cl, cc);
    }
  }
  // put id in symbol table if not there
  static size_t symbol_table_line = 1;
  if (symbols.find(id) == symbols.end()) {
    symbols.insert({ id, symbol_table_line++ });
  }
  return Token(IDENTIFIER, symbols[id], cl, cc);
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
    const std::string str = "(){}[]+*/^:,;";
    const size_t pos = str.find(code[i]);
    if (pos != str.npos) {
      next_char();
      return Token(str[pos], 0, cl, cc);
    }
  }
  // nothing that should be recognized.
  skip_invalid();
  return Token(INVALID, 0, cl, cc);
}
