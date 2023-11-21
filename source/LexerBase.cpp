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
  const bool eol = code[++i] == '\n';
  // update line and column accordingly
  line += eol ? 1 : 0;
  column = eol ? 0 : (column + 1);
  return code[i];
}

bool is_invalid(char c) {
  const std::string valid = " \n\t\0\"_(){}[]+*/^:,;-~=<>";
  return !isalnum(c) && valid.find(c) == valid.npos;
}

void Lexer::skip_invalid() {
  do {
    next_char();
  } while (is_invalid(code[i]));
}

bool is_spacer(char c) {
  return c == ' ' || (iscntrl(c) && c != '\0');
}

void Lexer::skip_spacers() {
  while (is_spacer(code[i])) {
    next_char();
  }
}

void Lexer::skip_comment() {
  uint8_t comment_nesting = 0, state = 0;
  for (next_char(); code[i]; next_char()) {
    switch (state) {
    case 0: // start
      if (code[i] == '[') { // may be a big comment
        state = 1;
      } else { // one line comment
        i--; // current character might be \n
        state = 2;
      }
      break;
    case 1: // check if big comment
      if (code[i] == '[') { // it's a big comment
        comment_nesting = 1;
        state = 3;
      } else { // one line comment
        i--; // current character might be \n
        state = 2;
      }
      break;
    case 2:  // skip one line comment
      if (code[i] == '\n') {
        next_char();
        return;
      }
      break;
    case 3: // big comment. Nesting may increase or decrease
      if (code[i] == '[') { // nesting may increase
        state = 4;
      } else if (code[i] == ']') { // nesting may decrease
        state = 5;
      }
      break;
    case 4: // Nesting may increase
      if (code[i] == '[') { // nesting increased
        comment_nesting++;
        state = 3;
      } else if (code[i] == ']') { // nesting may decrease
        state = 5;
      } else {
        state = 3;
      }
      break;
    case 5: // Nesting may decrease
      if (code[i] == ']') { // nesting decreased
        if (--comment_nesting == 0) { // end of big comment
          next_char();
          return;
        }
        state = 3;
      } else if (code[i] == '[') { // nesting may increase
        state = 4;
      } else {
        state = 3;
      }
      break;
    }
  }
}
