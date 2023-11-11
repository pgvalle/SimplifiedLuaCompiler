#include "Parser.h"
#include <algorithm>

Parser::Parser(const char* filepath) : lex(filepath) {
  tk = lex.next_token();
}

void Parser::log_token_name(TokenName name) const {
  switch (name) {
  case IDENTIFIER:
    printf("<id>");
    break;
  case RELOP:
    printf("<relop>");
    break;
  case NUMBER:
    printf("<num>");
    break;
  case STRING:
    printf("<str>");
    break;
  case CONCAT:
    printf("<..>");
    break;
  case KW_ENUM_BEGIN ... KW_ENUM_END:
    printf("<kw>" );
    break;
  case EOTS:
    printf("<EOTS>");
    break;
  default:
    printf("<%c>", name);
  }
}

void Parser::log_and_skip_error(std::vector<TokenName>&& syncs) {
  // print error here
  printf("Got "); tk.print(); printf(".\n");

  // skip tokens until found one that is inside sync set
  const auto begin = syncs.begin(), end = syncs.end();
  do {
    tk = lex.next_token();
  } while (std::find(begin, end, tk.name) == end);
}

void Parser::arguments() {
  
}

void Parser::identifiers() {
  
}

void Parser::parse() {

}