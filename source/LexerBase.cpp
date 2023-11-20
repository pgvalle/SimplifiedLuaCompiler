#include "Lexer.h"

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
  const std::string valid = " \n\t\0\"_(){}[]+*/^:,;-~=<>";
  char c;
  do {
    c = next_char();
  } while (!isalnum(c) && valid.find(c) == valid.npos);
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
