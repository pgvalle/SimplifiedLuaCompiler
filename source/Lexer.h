#pragma once

#include <cstdint>
#include <unordered_map>

class Lexer {
public:
  char* code;
  size_t i, line;
  unordered_map<string, size_t> symbols;
    
  Token next_id_or_kw();
  Token next_number();
  Token next_string();

  void skip_spacers();
  void skip_comment();

public:
    Lexer(const char* filepath);

    Token next_token() {
        begin:
        skip_spacers();

        switch (code[i]) { /* identifier/keyword */
        case 'A' ... 'Z':
        case 'a' ... 'z':
        case '_':
            return next_id_or_kw();
        /* number */
        case '0' ... '9':
            return next_number();
        /* string */
        case '"':
            return next_string();
        /* specific characters that don't match the beginning of any other pattern */
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case '+':
        case '*':
        case '/':
        case '^':
        case ':':
        case ',':
        case ';':
            return Token(code[i++], 0);
        /* comment or - operator */
        case '-':
            if (code[++i] == '-') { // -- it's a comment
                skip_comment();
                log("comment\n");
                goto begin; // go back to initial state
            }
            // it's the - sign
            return Token('-', 0);
        /* concat or scope operator */
        case '.':
            if (code[++i] == '.') { // ..
                i++;
                return Token(CONCAT, 0);
            }
            return Token('.', 0);
        /* logical operators */
        case '=':
            if (code[++i] == '=') {
                i++;
                return Token(RELOP, EQ);
            }
            return Token('=', 0);
        case '~':
            if (code[++i] == '=') {
                i++;
                return Token(RELOP, NE);
            }
            break; // '~' is unknown to the lexer
        case '<':
            if (code[++i] == '=') {
                i++;
                return Token(RELOP, LE);
            }
            return Token(RELOP, LT);
        case '>':
            if (code[++i] == '=') {
                i++;
                return Token(RELOP, GE);
            }
            return Token(RELOP, GT);
        }

        // stream may be over
        if (code[i] == '\0') {
            return Token(EOTS, 0);
        }
        // its nothing that was expected or end of code
        return Token(INVALID, 0);
    }
};