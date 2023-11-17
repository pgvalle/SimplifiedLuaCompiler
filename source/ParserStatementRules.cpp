#include "Parser.h"

void Parser::do_statement() {
  block();
  if (tk.name == KW_end) {
    fetch_next_token();
  } else {
    panic("<end>", { TkName(';') }); // follow(statement)
  }
}

void Parser::while_statement() {
  expression();
  if (tk.name == KW_do) {
    fetch_next_token();
  } else {
    panic("<do>", {
      // first(block) - { & }
      KW_do, KW_while, KW_if, KW_return, KW_break,
      KW_for, KW_local, KW_function, ID, TkName('('),
      // follow(block)
      KW_end, KW_elseif, KW_else, EOTS
    });
  }
  do_statement();
}

void Parser::if_statement() {
  auto then = [=]() {
    if (tk.name == KW_then) {
      fetch_next_token();    
    } else {
      panic("<then>", {
        // first(block) - { & }
        KW_do, KW_while, KW_if, KW_return, KW_break,
        KW_for, KW_local, KW_function, ID, TkName('('),
        // follow(block)
        KW_end, KW_elseif, KW_else, EOTS
      });
    }
    block();
  };

  expression();
  then();
  while (tk.name == KW_elseif) {
    fetch_next_token();
    expression();
    then();
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
    panic("<end>", { TkName(';') } ); // follow(statement)
  }
}

void Parser::return_statement() {
  // First(expressions)
  const TkNameList first_expressions = {
    KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
    NUMBER, STRING, TkName('-'), TkName('{'), TkName('(')
  };
  // if found, then there's an expression
  if (token_in(first_expressions)) {
    expressions();
  }
}

void Parser::for_statement() {
  if (tk.name == ID) {
    fetch_next_token();
  } else {
    panic("<id>", {
      // first(forids) - { & } U follow(forids)
      TkName('='), TkName(','), KW_in,
      // first(expression)
      KW_not, KW_nil, KW_true, KW_false,
      KW_function, ID, NUMBER, STRING,
      TkName('-'), TkName('{'), TkName('(')
    });
  }

  switch (tk.name) {
  case ',':
    do {
      fetch_next_token();
      if (tk.name == ID) {
        fetch_next_token();
      } else {
        // panic("<=>", {});
      }
    } while (tk.name == ',');
  case KW_in:
    expressions();
    do_statement();
    break;
  case '=':
    fetch_next_token();
    expression();
    if (tk.name == ',') {
      fetch_next_token();
    } else {
      // panic("<=>", {});
    }
    expression();
    // for_expressions();
    do_statement();
    break;
  default:
    panic("<=>", {});
  }
}

void Parser::decl_statement() {
  switch (tk.name) {
  case ID:
    identifiers();
    if (tk.name == '=') {
      fetch_next_token();
    } else {
      panic("<=>", {
        // first(expression)
        KW_not, KW_nil, KW_true, KW_false,
        KW_function, ID, NUMBER, STRING,
        TkName('-'), TkName('{'), TkName('(')
      });
    }
    expressions();
    break;
  case KW_function:
    fetch_next_token();
    function();
    break;
  default:
    // Follow(declaraction)
    panic("<id> or <function>", { TkName(';') });
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
    break;
  case KW_local:
    fetch_next_token();
    decl_statement();
    break;
  case KW_function:
    fetch_next_token();
    function();
    break;
  case ID:
  case '(':
    variables();
    break;
  default:
    panic(
      "<do>, <while>, <if>, <return>, \
<break>, <local>, <function>, <id>, <(>",
      { TkName(';') }
    );
  }
}
