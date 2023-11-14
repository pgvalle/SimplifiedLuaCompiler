#include "Parser.h"

void Parser::block() {
  const TkNameList firsts = {
    KW_do, KW_while, KW_if, KW_return, KW_break,
    KW_for, KW_local, KW_function, ID, TkName('(')
  };
  if (token_in(firsts)) {
    statement();
    if (tk.name == ';') {
      fetch_next_token();
    } else {
      printf("Error: Expected <;>");
      log_and_skip_error(firsts);
    }
    block();
  }
}

void Parser::field() {
  switch (tk.name) {
  case '[':
    fetch_next_token();
    expression();
    
    expression();
    break;
  case ID:
    fetch_next_token();
    if (tk.name == '=') {
      fetch_next_token();
    } else {
      printf("Error: Expected <=>");
      log_and_skip_error({ // First(expression)
        KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
        NUMBER, STRING, TkName('-'), TkName('{'), TkName('(')
      });
    }
    expression();
  }
}

void Parser::function() {
  // // id ( Idsopt ) Block end
  // if (tk.name == ID) {
  //   fetch_next_token();
  // } else {
  //   printf("Error: Expected <id>");
  //   log_and_skip_error({});
  // }

  // if (tk.name == '(') {
  //   fetch_next_token();
  // } else {
  //   printf("Error: Expected <=>");
  //   log_and_skip_error({});
  // }

  // if (tk.name == ID) {
  //   fetch_next_token();
  //   identifiers();
  // }

  // if (tk.name == ')') {
  //   fetch_next_token();
  // } else {
  //   log_and_skip_error({ // First(expression)
  //     KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
  //     NUMBER, STRING, TkName('-'), TkName('{'), TkName('(')
  //   });
  // }
  // block();
  // if (tk.name == KW_end) {
  //   fetch_next_token();
  // } else {
  //   log_and_skip_error({ // First(expression)
  //     KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
  //     NUMBER, STRING, TkName('-'), TkName('{'), TkName('(')
  //   });
  // }
}




void Parser::expression() {
  
}

void Parser::expressions() {
  do {
    expression();
  } while (tk.name == ',');
}

void Parser::variable() {

}

void Parser::variables() {
  do {
    variable();
  } while (tk.name == ',');
}

void Parser::fields() {
  do {
    field();
  } while (tk.name == ',');
}

void Parser::identifiers() {
  if (tk.name == ID) {
    fetch_next_token();
  } else {
    printf("Error: Expected <id>");
    log_and_skip_error({ TkName('=') }); // Follow(identifiers)
  }
  while (tk.name == ',') {
    fetch_next_token();
    identifiers();
  }
}
