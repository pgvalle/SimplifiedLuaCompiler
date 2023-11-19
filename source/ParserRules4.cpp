#include "Parser.h"


void Parser::variable() {
  switch (tk.name) {
  case '(':
    fetch_next_token();
    expression();
    if (tk.name == ')') {
      fetch_next_token();
    } else {
      panic("<(>", {
        // first(expression)
        KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
        NUMBER, STRING, TkName('-'), TkName('{'), TkName('('),
        // the next expected token may be there
        TkName('[')
      });
    }
    if (tk.name == '[') {
      fetch_next_token();
    } else {
      panic("<[>", {
        // first(expression)
        KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
        NUMBER, STRING, TkName('-'), TkName('{'), TkName('(')
      });
    }
    expression();
    variable2();
    break;
  case ID:
    fetch_next_token();
    variable2();
    break;
  default:
    // follow(variable)
    panic("<id> or <(>", {
      TkName('='), TkName(',')
    });
  }
}

void Parser::variable2() {
  if (tk.name == '[') {
    fetch_next_token();
    expression();
    if (tk.name == ']') {
      fetch_next_token();
    } else {
      // follow(variable2)
      panic("<]>", { TkName('='), TkName(',') });
    }
    variable2();
  }
}

void Parser::variables() {
  variable();
  while (tk.name == ',') {
    fetch_next_token();
    variable();
  }
}

void Parser::field() {
  switch (tk.name) {
  case '[':
    fetch_next_token();
    expression();
    if (tk.name == ']') {
      fetch_next_token();
    } else {
      panic("<]>", {
        // first(expression)
        KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
        NUMBER, STRING, TkName('-'), TkName('{'), TkName('('),
        // the next expected token may be there
        TkName('=')
      });
    }
    if (tk.name == '=') {
      fetch_next_token();
    } else {
      panic("<=>", {
        // first(expression)
        KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
        NUMBER, STRING, TkName('-'), TkName('{'), TkName('(')
      });
    }
    expression();
    break;
  case ID:
    fetch_next_token();
    if (tk.name == '=') {
      fetch_next_token();
    } else {
      panic("<=>", {
        // first(expression)
        KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
        NUMBER, STRING, TkName('-'), TkName('{'), TkName('(')
      });
    }
    expression();
    break;
  default:
    panic("<id> or <[>", {
      // follow(field)
      TkName('}'), TkName(')'), TkName(',')
    });
  }
}

void Parser::fields() {
  field();
  while (tk.name == ',') {
    fetch_next_token();
    field();
  }
}

void Parser::identifiers() {
  while (tk.name == ',') {
    fetch_next_token();
    if (tk.name == ID) {
      fetch_next_token();
    } else {
      panic("<id>", {
        // follow(ids)
        TkName('='), TkName(')')
      });
    }
  }
}
