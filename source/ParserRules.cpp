#include "Parser.h"

void Parser::block() {
  const TkNameList first_block = {
    // first(block) - { & }
    KW_do, KW_while, KW_if, KW_return, KW_break,
    KW_for, KW_local, KW_function, ID, TkName('(')
  };
  while (token_in(first_block)) {
    statement();
    if (tk.name == ';') {
      fetch_next_token();
    } else {
      panic("<;>", {
        // first(block) - { & }
        KW_do, KW_while, KW_if, KW_return, KW_break,
        KW_for, KW_local, KW_function, ID, TkName('('),
        // follow(block)
        KW_end, KW_elseif, KW_else, EOTS
      });
    }
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
      panic("<]>", {
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
  default:
    panic("<id> or <[>", {
      // follow(field)
      TkName('}'), TkName(')')
    });
  }
}

void Parser::function() {
  if (tk.name == ID) {
    fetch_next_token();
  } else if (tk.name != '(') {
    // first(Ids) e follow porque pode não ter id
    panic("<id> or <(>", { TkName('('), TkName(')'), ID });
  }
  if (tk.name == '(') {
    fetch_next_token();
  } else {
    // first(Ids) e follow porque pode não ter id
    panic("<id> or <(>", { TkName('('), TkName(')'), ID });
  }
  if (tk.name == ID) {
    identifiers();
  }
  if (tk.name == ')') {
    fetch_next_token();
  } else {
    panic("<)>", {
      // First(block) - { & }
      KW_do, KW_while, KW_if, KW_return, KW_break,
      KW_for, KW_local, KW_function, ID, TkName('('),
      // follow(block)
      KW_end, KW_elseif, KW_else, EOTS
    });
  }
  block();
  if (tk.name == KW_end) {
    fetch_next_token();
  } else {
    panic("<end>", {
      // follow(function)
      TkName('+'), TkName('-'), TkName('*'), TkName('/'),
      TkName('^'), KW_or, KW_and, RELOP, CONCAT,
      TkName('}'), TkName(')'), TkName(']'), TkName(';'),
      TkName(','), KW_do, KW_then
    });
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
  case ID:
  case '(':
    // prefix_expression();
    expression2();
    break;
  case KW_function:
    fetch_next_token();
    function();
    expression2();
    break;
  case '{':
    fetch_next_token();
    if (tk.name == ID || tk.name == '[') {
      fields();
    }
    if (tk.name == '}') {
      fetch_next_token();
    } else {
      // first e follow de expression2 (epsilon no first)
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
  const TkNameList firsts = {
    TkName('+'), TkName('-'), TkName('*'), TkName('/'),
    TkName('^'), KW_or, KW_and, RELOP, CONCAT
  };
  if (token_in(firsts)) {
    fetch_next_token();
    expression();
    expression2();
  }
}

void Parser::expressions() {
  do {
    expression();
  } while (tk.name == ',');
}

void Parser::variable() {
  printf("Variable not implemented! Aborting...\n");
  exit(0);
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
    // follow(identifiers)
    panic("<id>", {
      // follow(ids)
      TkName('='), TkName(')')
    });
  }
  if (tk.name == ',') {
    fetch_next_token();
    identifiers();
  }
}
