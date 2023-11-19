#include "Parser.h"

void Parser::prefix_expression() {
  switch (tk.name) {
  case '(':
    fetch_next_token();
    expression();
    if (tk.name == ')') {
      fetch_next_token();
    } else {
      // TODO: implement error handling here
    }
    if (tk.name == '[') {
      fetch_next_token();
      expression();
      if (tk.name == ']') {
        fetch_next_token();
      } else {
        // TODO: implement error handling here
      }
      variable2();
    }
    break;
  case ID:
    fetch_next_token();
    variable2();
    break;
  }
}

void Parser::expression() {
  switch (tk.name) {
  case KW_not:
  case '-':
    fetch_next_token();
    expression();
    expression2();
    break;
  case '(':
  case ID:
    prefix_expression();
    expression2();
    break;
  case KW_function:
    fetch_next_token();
    function();
    expression2();
    break;
  case '{':
    fetch_next_token();
    // fields is optional so we check for first(fields) before calling it
    if (tk.name == ID || tk.name == '[') {
      fields();
    }
    if (tk.name == '}') {
      fetch_next_token();
    } else {
      panic("<}>", {
        // first(expression2) - { & }
        TkName('+'), TkName('-'), TkName('*'), TkName('/'),
        TkName('^'), KW_or, KW_and, RELOP, CONCAT,
        // follow(expression2) (includes binops but they're above already)
        TkName('}'), TkName(')'), TkName(']'), TkName(';'),
        TkName(','), KW_do, KW_then
      });
    }
    expression2();
    break;
  case KW_true:
  case KW_false:
  case KW_nil:
  case NUMBER:
  case STRING:
    fetch_next_token();
    expression2();
    break;
  default:
    panic("expression", {
      // follow(expression)
      TkName('+'), TkName('-'), TkName('*'), TkName('/'),
      TkName('^'), KW_or, KW_and, RELOP, CONCAT,
      TkName('}'), TkName(')'), TkName(']'), TkName(';'),
      TkName(','), KW_do, KW_then
    });
  }
}

void Parser::expression2() {
  const TkNames firsts = {
    TkName('+'), TkName('-'), TkName('*'), TkName('/'),
    TkName('^'), KW_or, KW_and, RELOP, CONCAT
  };
  if (TkName_in(firsts)) {
    fetch_next_token();
    expression();
    expression2();
  }
}

void Parser::expressions() {
  expression();
  while (tk.name == ',') {
    fetch_next_token();
    expression();
  }
}
