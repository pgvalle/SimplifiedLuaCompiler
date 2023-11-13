#include "Parser.h"

// done (not sure)
void Parser::do_statement() {
  block();
  if (tk.name == KW_end) {
    fetch_next_token();
  } else {
    printf("Error: Expected <kw, end>");
    log_and_skip_error({ TkName(';') });
  }
}

// almost done
void Parser::while_statement() {
  expression();
  if (tk.name == KW_do) {
    fetch_next_token();
    block();
  } else {
    printf("Error: Expected <kw, do>");
    log_and_skip_error({
      KW_do, KW_while, KW_if, KW_return, KW_for,
      KW_local, KW_function, ID, TkName('('), EOTS
    });
  }
  block();
  if (tk.name == KW_end) {
    fetch_next_token();
  } else {
    printf("Error: Expected <kw, end>");
    log_and_skip_error({});
  }
}

// not done
void Parser::if_statement() {
  expression();
  if (tk.name == KW_then) {
    fetch_next_token();    
  } else {
    printf("Error: Expected <kw, then>");
    log_and_skip_error({
      KW_do, KW_while, KW_if, KW_return, KW_for,
      KW_local, KW_function, ID, TkName('('), EOTS
    });
  }
  block();

  // TODO: elseif e else faltam

  if (tk.name == KW_end) {
    fetch_next_token();
  } else {
    printf("Error: Expected <kw, end>");
    log_and_skip_error({});
  }
}

// done
void Parser::return_statement() {
  fetch_next_token();
  // first(expressions)
  const TkNameList names = {
    KW_not, KW_nil, KW_true, KW_function,
    ID, TkName('-'), TkName('{'), TkName('(')
  };
  // if found. Then there's an expression
  for (TkName name : names) {
    if (name == tk.name) {
      expressions();
      break;
    }
  }
}

void Parser::for_statement() {
  if (tk.name == ID) {
    fetch_next_token();
  } else {
    printf("Error: Expected <id>");
    log_and_skip_error({ TkName(','), TkName('='), KW_in });
  }
  // fors();
}

// almost done
void Parser::decl_statement() {
  if (tk.name == ID) {
    identifiers();
    if (tk.name == '=') {
      fetch_next_token();
    } else {
      printf("Error: Expected <=>");
      log_and_skip_error({
        KW_not, KW_nil, KW_true, KW_function,
        ID, TkName('-'), TkName('{'), TkName('(')
      });
    }
    expressions();
  } else if (tk.name == KW_function) {
    fetch_next_token();
    function();
  } else {
    printf("Error: Expected <id> or <function>");
    log_and_skip_error({});
  }
}

void Parser::statement() {
  switch (tk.name) {
  case KW_do:
    fetch_next_token();
    do_statement();
    break;
  case KW_while:
    fetch_next_token();
    while_statement();
    break;
  case KW_if:
    fetch_next_token();
    if_statement();
    break;
  case KW_return:
    fetch_next_token();
    return_statement();
    break;
  case KW_break:
    fetch_next_token();
    break;
  case KW_for:
    fetch_next_token();
    for_statement();
  case KW_local:
    fetch_next_token();
    decl_statement();
  case KW_function:
    fetch_next_token();
    function();
    break;
  default:
    // vars ou functioncall. Muito difícil
    break;
  }
}
