#include "Parser.h"
#include <algorithm>

Parser::Parser(const char* filepath) : lex(filepath) {
  tk = lex.next_token(); // first token ready
}

void Parser::log_token_name(TokenName name) const {
  switch (name) {
  case IDENTIFIER:
    printf("<id>");
    break;
  case RELOP:
    printf("<relop>");
    break;
  case NUMBER:
    printf("<num>");
    break;
  case STRING:
    printf("<str>");
    break;
  case CONCAT:
    printf("<..>");
    break;
  case KW_ENUM_BEGIN ... KW_ENUM_END:
    printf("<kw>" );
    break;
  case EOTS:
    printf("<EOTS>");
    break;
  default:
    printf("<%c>", name);
  }
}

void Parser::log_and_skip_error(TokenNameList&& syncs) {
  // print error here
  printf("\n  Got "); tk.print(); printf(".\n");

  // skip tokens until found one that is inside sync set
  const auto begin = syncs.begin(), end = syncs.end();
  do {
    tk = lex.next_token();
  } while (std::find(begin, end, tk.name) == end);
}

void Parser::block() {
  while (!tk.is_last()) {
    statement();
    if (tk.name == ';') {
      tk = lex.next_token();
    } else {
      printf("Error: Expected <;>.");
      log_and_skip_error({});
    }
  }
}

void Parser::statement() {

}

void Parser::expressions() {
  // expression();
  while (tk.name == ',') {
    // expression();
  }
}

void Parser::arguments() {
  switch (tk.name) {
  case '(':
    tk = lex.next_token();
    if (tk.name == ')') {
      tk = lex.next_token();
    } else {
      expressions();
      if (tk.name == ')') {
        tk = lex.next_token();
      } else {
        printf("Error: expected <)>.");
        log_and_skip_error({ TokenName(')') });
      }
    }
    break;
  case '{':

    break;
  default:
    printf("Error: expected <{> or <(>.");
    log_and_skip_error({ TokenName('{'), TokenName('(') });
  }
}

void Parser::identifiers() {
  do {
    // next id in list
    if (tk.name == IDENTIFIER) {
      tk = lex.next_token();
    } else {
      printf("Error: Expected <id>.");
      log_and_skip_error({ IDENTIFIER }); // First (Ids2) = { ',', & }
    }
  } while (tk.name == ',');
}

void Parser::parse() {

}
