#include "Parser.h"

void Parser::do_statement() {
  block();
  if (tk.name == KW_end) {
    fetch_next_token();
  } else {
    printf("Error: Expected <kw, end>");
    log_and_skip_error({ TkName(';') }); // Follow(Stmt)
  }
}

void Parser::while_statement() {
  expression();
  if (tk.name == KW_do) {
    fetch_next_token();
  } else {
    printf("Error: Expected <kw, do>");
    log_and_skip_error({ // First(Block)
      KW_do, KW_while, KW_if, KW_return, KW_break,
      KW_for, KW_local, KW_function, ID, TkName('(')
    });
  }
  do_statement();
}

void Parser::if_statement() {
  expression();
  if (tk.name == KW_then) {
    fetch_next_token();    
  } else {
    printf("Error: Expected <kw, then>");
    log_and_skip_error({ // First(Block)
      KW_do, KW_while, KW_if, KW_return, KW_break,
      KW_for, KW_local, KW_function, ID, TkName('(')
    });
  }
  block();
  while (tk.name == KW_elseif) {
    fetch_next_token();
    expression();
    if (tk.name == KW_then) {
      fetch_next_token();
    } else {
      printf("Error: Expected <kw, then>");
      log_and_skip_error({ // First(block)
        KW_do, KW_while, KW_if, KW_return, KW_for,
        KW_local, KW_function, ID, TkName('(')
      });
    }
    block();
  }
  // else (optional)
  if (tk.name == KW_else) {
    fetch_next_token();
    block();
  }
  // end
  if (tk.name == KW_end) {
    fetch_next_token();
  } else {
    printf("Error: Expected <kw, end>");
    log_and_skip_error({ TkName(';') }); // Follow(Stmt)
  }
}

void Parser::return_statement() {
  // First(expressions)
  const TkNameList names = {
    KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
    NUMBER, STRING, TkName('-'), TkName('{'), TkName('(')
  };
  // if found, then there's an expression
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
    log_and_skip_error({  });
  }
}

void Parser::decl_statement() {
  switch (tk.name) {
  case ID:
    identifiers();
    if (tk.name == '=') {
      fetch_next_token();
    } else {
      printf("Error: Expected <=>");
      log_and_skip_error({ // First(expressions)
        KW_not, KW_nil, KW_true, KW_function,
        ID, TkName('-'), TkName('{'), TkName('(')
      });
    }
    expressions();
    break;
  case KW_function:
    fetch_next_token();
    function();
    break;
  default:
    printf("Error: Expected <id> or <function>");
    log_and_skip_error({ TkName(';') }); // Follow(declaraction)
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
  case ID:
  case '(':
    variables();
    break;
  default:
    printf("Error: Expected <;>");
    log_and_skip_error({ TkName(';') }); // Follow(statement)
  }
}
