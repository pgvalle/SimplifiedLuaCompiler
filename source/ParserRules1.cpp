#include "Parser.h"

void Parser::block() {
  const TkNames firsts = {
    // first(block) - { & }
    KW_do, KW_while, KW_if, KW_return, KW_break,
    KW_for, KW_local, KW_function, ID, TkName('(')
  };
  while (TkName_in(firsts)) {
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

void Parser::function() {
  if (tk.name == ID) {
    fetch_next_token();
  } else if (tk.name != '(') {
    // first(Ids) e follow porque pode não ter id
    panic("<id> or <(>", { TkName('('), ID });
  }
  if (tk.name == '(') {
    fetch_next_token();
  } else {
    // first(Ids) e follow porque pode não ter id
    panic("<id> or <(>", { TkName(')'), ID });
  }
  if (tk.name == ID) {
    fetch_next_token();
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
