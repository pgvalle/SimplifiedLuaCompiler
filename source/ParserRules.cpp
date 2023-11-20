#include "Parser.h"

namespace First {
  const Parser::TkNames block = {
    KW_do, KW_while, KW_if, KW_return, KW_break,
    KW_for, KW_local, KW_function, ID, TkName('('),
    // epsilon
  };

  const Parser::TkNames function = {
    KW_function
  };

  const Parser::TkNames expression = {
    KW_not, KW_nil, KW_true, KW_false, KW_function, ID,
    NUMBER, STRING, TkName('-'), TkName('{'), TkName('(')
  };

  const Parser::TkNames& expressions = expression;

  const Parser::TkNames expression2 = {
    TkName('+'), TkName('-'), TkName('*'), TkName('/'),
    TkName('^'), KW_or, KW_and, RELOP, CONCAT
  };
}

namespace Follow {
  const Parser::TkNames block = {
    KW_end, KW_elseif, KW_else, EOTS
  };

  const Parser::TkNames function = {
    TkName('+'), TkName('-'), TkName('*'), TkName('/'),
    TkName('^'), KW_or, KW_and, RELOP, CONCAT,
    TkName('}'), TkName(')'), TkName(']'), TkName(';'),
    TkName(','), KW_do, KW_then
  };

  const Parser::TkNames statement = {
    TkName(';')
  };

  const Parser::TkNames expression = {
    TkName('+'), TkName('-'), TkName('*'), TkName('/'),
    TkName('^'), KW_or, KW_and, RELOP, CONCAT,
    TkName('}'), TkName(')'), TkName(']'), TkName(';'),
    TkName(','), KW_do, KW_then
  };

  const Parser::TkNames expression2 = {
    TkName('}'), TkName(')'), TkName(']'),
    TkName(';'), TkName(','), KW_do, KW_then
  };
}

void Parser::block() {
  while (TkName_in(First::block)) {
    statement();
    if (tk.name == ';') {
      next_token();
    } else {
      panic("<;>", { First::block, Follow::block });
    }
  }
}

void Parser::function() {
  if (tk.name == ID) {
    next_token();
  } else if (tk.name != '(') {
    // first(Ids) e follow porque pode não ter id
    panic("<id> or <(>", { { TkName('('), ID } });
  }
  if (tk.name == '(') {
    next_token();
  } else {
    // first(Ids) e follow porque pode não ter id
    panic("<id> or <(>", { { TkName(')'), ID } });
  }
  if (tk.name == ID) {
    next_token();
    identifiers();
  }
  if (tk.name == ')') {
    next_token();
  } else {
    panic("<)>", { First::block, Follow::block });
  }
  block();
  if (tk.name == KW_end) {
    next_token();
  } else {
    panic("<end>", { Follow::function });
  }
}

void Parser::do_statement() {
  block();
  if (tk.name == KW_end) {
    next_token();
  } else {
    panic("<end>", { Follow::statement });
  }
}

void Parser::if_statement() {
  goto then;
  while (tk.name == KW_elseif) {
    next_token();
    then:
    expression();
    if (tk.name == KW_then) {
      next_token();    
    } else {
      panic("<then>", { First::block, Follow::block });
    }
    block();
  } 
  if (tk.name == KW_else) {
    next_token();
    block();
  }
  if (tk.name == KW_end) {
    next_token();
  } else {
    panic("<end>", { Follow::statement } );
  }
}

void Parser::for_statement() {
  const auto do_ = [=]() {
    if (tk.name == KW_do) {
      next_token();
    } else {
      panic("<do>", { First::block, Follow::block });
    }
    do_statement();
  };

  if (tk.name == ID) {
    next_token();
  } else {
    // first(forids) - { & } U follow(forids)
    panic("<id>", { { TkName('='), TkName(','), KW_in } });
  }
  switch (tk.name) {
  case ',':
    do {
      next_token();
      if (tk.name == ID) {
        next_token();
      } else {
        panic("<id>", { { KW_in } });
      }
    } while (tk.name == ',');
  case KW_in:
    next_token();
    expressions();
    do_();
    break;
  case '=':
    next_token();
    expression();
    if (tk.name == ',') {
      next_token();
    } else {
      panic("<,>", { First::expression });
    }
    expression();
    if (tk.name == ',') {
      next_token();
      expression();
    }
    do_();
    break;
  default:
    panic("<=>", { Follow::statement });
  }
}

void Parser::decl_statement() {
  switch (tk.name) {
  case ID:
    next_token();
    identifiers();
    if (tk.name == '=') {
      next_token();
    } else {
      panic("<=>", { First::expressions });
    }
    expressions();
    break;
  case KW_function:
    next_token();
    function();
    break;
  default:
    panic("<id> or <function>", { Follow::statement });
  }
}

void Parser::statement() {
  switch (tk.name) {
  case KW_do:
    next_token();
    do_statement();
    break;
  case KW_while:
    next_token();
    expression();
    do_statement();
    break;
  case KW_if:
    next_token();
    if_statement();
    break;
  case KW_return:
    next_token();
    // if found, then 'return' returns something
    if (TkName_in(First::expressions)) {
      expressions();
    }
    break;
  case KW_break:
    next_token();
    break;
  case KW_for:
    next_token();
    for_statement();
    break;
  case KW_local:
    next_token();
    decl_statement();
    break;
  case KW_function:
    next_token();
    function();
    break;
  case ID:
  case '(':
    // variables
    variable();
    while (tk.name == ',') {
      next_token();
      variable();
    }
    if (tk.name == '=') {
      next_token();
    } else {
      panic("<=>", { First::expressions });
    }
    expressions();
    break;
  default:
    panic(
      "<do>, <while>, <if>, <return>, \
<break>, <local>, <function>, <id>, <(>",
      { Follow::statement }
    );
  }
}

void Parser::expression() {
  switch (tk.name) {
  case KW_not:
  case '-':
    next_token();
    expression();
    expression2();
    break;
  case '(': // prefix expression
    next_token();
    expression();
    if (tk.name == ')') {
      next_token();
    } else {
      panic("<)>", {}); // TODO: implement error handling here
    }
    if (tk.name == '[') {
      next_token();
      expression();
      if (tk.name == ']') {
        next_token();
      } else {
        panic("<]>", {}); // TODO: implement error handling here
      }
      variable2();
    }
    break;
  case ID: // prefix expression
    next_token();
    variable2();
    break;
  case KW_function:
    next_token();
    function();
    expression2();
    break;
  case '{':
    next_token();
    // since fields is optional we check for first(fields) before calling it
    if (tk.name == ID || tk.name == '[') {
      // fields
      field();
      while (tk.name == ',') {
        next_token();
        field();
      }
    }
    if (tk.name == '}') {
      next_token();
    } else {
      panic("<}>", { First::expression2, Follow::expression2 });
    }
    expression2();
    break;
  case KW_true:
  case KW_false:
  case KW_nil:
  case NUMBER:
  case STRING:
    next_token();
    expression2();
    break;
  default:
    panic("expression", { Follow::expression });
  }
}

void Parser::expression2() {
  if (TkName_in(First::expression2)) {
    next_token();
    expression();
    expression2();
  }
}

void Parser::expressions() {
  expression();
  while (tk.name == ',') {
    next_token();
    expression();
  }
}

void Parser::variable() {
  switch (tk.name) {
  case '(':
    next_token();
    expression();
    if (tk.name == ')') {
      next_token();
    } else {
      panic("<(>", { First::expression, { TkName('[') } });
    }
    if (tk.name == '[') {
      next_token();
    } else {
      panic("<[>", { First::expression });
    }
    expression();
    variable2();
    break;
  case ID:
    next_token();
    variable2();
    break;
  default:
    // follow(variable)
    panic("<id> or <(>", { { TkName('='), TkName(',') } });
  }
}

void Parser::variable2() {
  if (tk.name == '[') {
    next_token();
    expression();
    if (tk.name == ']') {
      next_token();
    } else {
      // follow(variable2)
      panic("<]>", { { TkName('='), TkName(',') } });
    }
    variable2();
  }
}

void Parser::field() {
  switch (tk.name) {
  case '[':
    next_token();
    expression();
    if (tk.name == ']') {
      next_token();
    } else {
      panic("<]>", { First::expression, { TkName('=') } });
    }
    if (tk.name == '=') {
      next_token();
    } else {
      panic("<=>", { First::expression });
    }
    expression();
    break;
  case ID:
    next_token();
    if (tk.name == '=') {
      next_token();
    } else {
      panic("<=>", { First::expression });
    }
    expression();
    break;
  default:
    // follow(field)
    panic("<id> or <[>", { { TkName('}'), TkName(')'), TkName(',') } });
  }
}

void Parser::identifiers() {
  while (tk.name == ',') {
    next_token();
    if (tk.name == ID) {
      next_token();
    } else {
      // follow(ids)
      panic("<id>", { { TkName('='), TkName(')') } });
    }
  }
}
