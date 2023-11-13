#include "Parser.h"

void Parser::block() {
  while (tk.name != EOTS) {
    statement();
    if (tk.name == ';') {
      fetch_next_token();
    } else {
      printf("Error: Expected <;>.");
      log_and_skip_error({
        ID, TkName('('), KW_do, KW_while,
        KW_if, KW_return, KW_for, KW_local, KW_function
      });
    }
  }
}

void Parser::field() {
  switch (tk.name) {
  case '[':
    fetch_next_token();
    expression();
    // mais coisa
    break;
  case ID:

  }
}

void Parser::function() {
  
}

void Parser::variable_or_call() {

}

void Parser::expressions() {
  expression();
  while (tk.name == ',') {
    expression();
  }
}

void Parser::expression() {
  
}

void Parser::arguments() {
  switch (tk.name) {
  case '(':
    fetch_next_token();
    if (tk.name == ')') {
      fetch_next_token();
    } else {
      expressions();
      if (tk.name == ')') {
        fetch_next_token();
      } else {
        printf("Error: expected <)>.");
        log_and_skip_error({});
      }
    }
    break;
  case '{':
    fetch_next_token();
    if (tk.name == '}') {
      fetch_next_token();
    } else {
      do {
        field();
      } while (tk.name == ',');
    }
    break;
  default:
    printf("Error: expected <{> or <(>.");
    log_and_skip_error({});
  }
}

void Parser::identifiers() {
  do {
    // next id in list
    if (tk.name == ID) {
      fetch_next_token();
    } else {
      printf("Error: Expected <id>.");
      log_and_skip_error({ ID });
    }
  } while (tk.name == ',');
}
