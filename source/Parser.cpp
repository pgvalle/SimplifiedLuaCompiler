#include "Parser.h"
#include <algorithm>

Parser::Parser(const char* filepath) : lex(filepath) {}

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

bool Parser::find_token(const TokenNameList& syncs) {
  const auto it = std::find(
    syncs.begin(), syncs.end(), tk.name
  );
  return (it != syncs.end() || tk.name == EOTS);
}

void Parser::log_and_skip_error(TokenNameList&& syncs) {
  // print what token we actually got
  printf("\n       Got "); tk.print();
  // skip tokens until found one that is inside sync set
  do {
    tk = lex.next_token();
  } while (!find_token(syncs));
  // show where we're at
  printf("       Continuing from "); tk.print();
}

void Parser::block() {
  while (tk.name != EOTS) {
    statement();
    if (tk.name == ';') {
      tk = lex.next_token();
    } else {
      printf("Error: Expected <;>.");
      log_and_skip_error({
        // first(Function) U First(FunctionCall)
        KW_function, IDENTIFIER, TokenName('('),
        // remaining
        KW_do, KW_while, KW_if, KW_return, KW_for, KW_local
      });
    }
  }
}

void Parser::statement() {
  switch (tk.name) {
  case KW_do:
    tk = lex.next_token();
    block();
    if (tk.name == KW_end) {
      tk = lex.next_token();
    } else {
      printf("Error: Expected <end>.");
      log_and_skip_error({ TokenName(';') });
    }
    break;
  case KW_break:
    tk = lex.next_token();
    break;
  case KW_for:
    tk = lex.next_token();
    if (tk.name == IDENTIFIER) {
      tk = lex.next_token();
    } else {
      printf("Error: Expected <id>.");
      log_and_skip_error({
        TokenName(','), TokenName('='), KW_in
      });
    }
    // fors();
  default:
    if (tk.name == KW_function) {
      tk = lex.next_token();
      // function();
    } else {
      variable_or_call();
    }
  }
}

void Parser::variable_or_call() {

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
  tk = lex.next_token(); // first token ready
  block();
}
