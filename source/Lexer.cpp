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
  code = new char[file_len + 1]();
  fread(code, 1, file_len, file);
  fclose(file);

  i = 0;
  line = 1;
}

Lexer::~Lexer() {
  delete code;
}

void Lexer::skip_spacers() {
  // one state only
  char c = code[i];
  bool is_spacer = (c == ' ' || c == '\n');
  while (is_spacer) {
    line += size_t(c == '\n');
    // get next character and check again
    c = code[++i];
    is_spacer = (c == ' ' || c == '\n');
  }
}

void Lexer::skip_comment() {
  int comment_nesting = 0, state = 0;
  for (char c = code[++i]; c; c = code[++i]) {
    switch (state) {
    case 0: // start
      if (c == '[') { // may be a big comment
        state = 1;
      } else { // one line comment
        state = 2;
        i--;
        continue; // current character might be \n already
      }
      break;
    case 1: // check if big comment
      if (c == '[') { // it's a big comment
        log("big ");
        comment_nesting++;
        state = 3;
      } else { // one line comment
        state = 2;
        i--;
        continue; // current character might be \n already
      }
      break;
    case 2:  // skip one line comment
      if (c == '\n') {
        i++;
        return;
      }
      break;
    case 3: // big comment. Nesting may increase or decrease
      if (comment_nesting == 0) { // end of big comment
        i++;
        return;
      }
      // still opened
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
      }
      break;
    case 5: // Nesting may decrease
      if (c == ']') { // nesting decreased
        comment_nesting--;
        state = 3;
      } else if (c == '[') { // nesting may increase
        state = 4;
      }
      break;
    }
  }
}

Token Lexer::next_id_or_kw() {
  // read and store id
  std::string id;
  for (char c = code[i]; isalnum(c); c = code[++i]) {
    id += c;
  }
  // check if it's a keyword. It might be.
  const size_t kw_enum_len = KW_ENUM_END - KW_ENUM_BEGIN - 1;
  for (size_t j = 0; j < kw_enum_len; j++) {
    if (strcmp(id.c_str(), KEYWORDS[j]) == 0) {
      return Token(j + KW_ENUM_BEGIN + 1, j);
    }
  }
  // put id in symbol table if not there
  static size_t symbol_table_line = 1;
  if (symbols.find(id) == symbols.end()) {
    symbols.insert({ id, symbol_table_line++ });
  }
  return Token(IDENTIFIER, symbols[id]);
}

Token Lexer::next_number() {
  int number = 0;
  for (char c = code[i]; isdigit(c); c = code[++i]) {
    number = 10 * number + int(c - '0');
  }
  return Token(NUMBER, number);
}

Token Lexer::next_string() {
  int state = 0;
  for (char c = code[++i]; c; c = code[++i]) {
    switch (state) {
    case 0: // reading string
      if (c == '\\') { // control character code comming
        state = 1;
      } else if (c  == '"') {
        i++;
        return Token(STRING, line);
      }
      break;
    case 1: // skip control character
      state = 0;
      break;
    }
  }

  return Token(INVALID, 0);
}

Token Lexer::next_token() {
  start:
  skip_spacers();

  switch (code[i]) { 
  case 'A' ... 'Z':
  case 'a' ... 'z':
  case '_':
    return next_id_or_kw();
  case '0' ... '9':
    return next_number();
  case '"':
    return next_string();
  case '(':
  case ')':
  case '{':
  case '}':
  case '[':
  case ']':
  case '+':
  case '*':
  case '/':
  case '^':
  case ':':
  case ',':
  case ';':
      return Token(code[i++], 0);
  case '-':
    if (code[++i] == '-') { // it's a comment
      skip_comment();
      log("comment\n");
      goto start; // go back to start
    }
    // it's the - sign
    return Token('-', 0);
  case '.': // concat operator or scope operator
    if (code[++i] == '.') { // ..
      i++;
      return Token(CONCAT, 0);
    }
    return Token('.', 0);
  case '=': // logical operators
    if (code[++i] == '=') {
      i++;
      return Token(RELOP, EQ);
    }
    return Token('=', 0);
  case '~':
    if (code[++i] == '=') {
      i++;
      return Token(RELOP, NE);
    }
    break; // '~' is unknown to the lexer
  case '<':
    if (code[++i] == '=') {
      i++;
      return Token(RELOP, LE);
    }
    return Token(RELOP, LT);
  case '>':
    if (code[++i] == '=') {
      i++;
      return Token(RELOP, GE);
    }
    return Token(RELOP, GT);
  }
  // stream may be over
  if (code[i] == '\0') {
    return Token(EOTS, 0);
  }
  // its nothing that was expected or end of code
  return Token(INVALID, 0);
}