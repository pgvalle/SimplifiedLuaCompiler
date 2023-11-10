#include <cstdio>
#include <string>
#include <array>
#include <unordered_map>

using namespace std;

// for debugging purposes
#define log printf
//#define log(args) ;

static const string KEYWORDS[] = {
    "and", "break", "do", "else", "elseif", "end", "false",
    "for", "function", "if", "in", "local", "nil", "not",
    "or", "repeat", "return", "then", "true", "until", "while"
};

// the remaining token names that are not here are ascii characters
// and don't match any other token beginning 
enum TokenName {
    // basic???
    INVALID = 0, IDENTIFIER, RELOP, NUMBER, STRING,
    
    // ???
    CONCAT,

    KW_LIST_BEGIN = 200,
        AND, BREAK, DO, ELSE, ELSEIF, END, FALSE,
        FOR, FUNCTION, IF, IN, LOCAL, NIL, NOT,
        OR, REPEAT, RETURN, THEN, TRUE, UNTIL, WHILE,
    KW_LIST_END,

    EOTS // End Of Token Stream
};

static const string RELOPS[] = {
    "==", "~=", "<", ">", "<=", ">="
};

// types of relop tokens
enum RelOp {
    RELOP_BEGIN = 128,
    EQ, NE, LT, GT, LE, GE
};

struct Token {
    int name;
    int attrib;

    Token() {
        name = INVALID;
        attrib = 0;
    }

    Token(int name_, int attrib_) {
        name = name_;
        attrib = attrib_;
    }

    void print() const {
        switch (name) {
        case INVALID:
            printf("<ERROR>");
            break;
        case IDENTIFIER:
            printf("<id, %d>", attrib);
            break;
        case RELOP:
            printf("<relop, %s>", RELOPS[attrib - RELOP_BEGIN - 1].c_str());
            break;
        case NUMBER:
            printf("<num, %d>", attrib);
            break;
        case STRING:
            printf("<str, %d>", attrib);
            break;
        case CONCAT:
            printf("<..>");
            break;
        case KW_LIST_BEGIN + 1 ... KW_LIST_END - 1:
            printf("<kw, %s>", KEYWORDS[attrib].c_str());
            break;
        case EOTS:
            printf("<EOTS>");
            break;
        default: // tokens that are a single ascii character
            printf("<%c>", name);
        }
    }
};

class Lexer {
public:
    string code;
    size_t i, line;
    unordered_map<string, size_t> symbols;
    
    Token next_id_or_kw() {
        // read and store id
        string id;
        while (isalnum(code[i])) {
            id += code[i++];
        }
        // check if it's a keyword. It might be.
        const size_t kw_list_len = KW_LIST_END - KW_LIST_BEGIN - 1;
        for (size_t j = 0; j < kw_list_len; j++) {
            if (id == KEYWORDS[j]) {
                return Token(j + KW_LIST_BEGIN + 1, j);
            }
        }
        // put id in symbol table if not there
        static size_t symbol_table_line = 1;
        if (symbols.find(id) == symbols.end()) {
            symbols.insert({ id, symbol_table_line++ });
        }
        return Token(IDENTIFIER, symbols[id]);
    }

    Token next_number() {
        int number = 0;
        while (isdigit(code[i])) {
            number = 10 * number + int(code[i++] - '0');
        }
        return Token(NUMBER, number);
    }

    Token next_string() {
        i++; // skip '"'
        int state = 0;
        while (code[++i]) {
            switch (state) {
            case 0: // reading string
                if (code[i] == '\\') { // control character code comming
                    state = 1;
                } else if (code[i]  == '"') {
                    i++;
                    return Token(STRING, line);
                }
                break;
            case 1: // skip control character
                state = 0;
                break;
            }
            i++;
        }

        return Token(INVALID, 0);
    }

    void skip_spacers() {
        // one state only
        while (code[i] == ' ' || code[i] == '\n') {
            line += size_t(code[i++] == '\n');
        }
    }

    void skip_comment() {
        i++; // skip '-'
        int comment_nesting = 0, state = 0;
        while (code[i]) {
            switch (state) {
            case 0: // start
                if (code[i] == '[') { // may be a big comment
                    state = 1;
                } else { // one line comment
                    state = 2;
                    continue; // current character might be \n already
                }
                break;
            case 1: // check if big comment.
                if (code[i] == '[') { // it is a big comment
                    log("big ");
                    comment_nesting++;
                    state = 3;
                } else { // one line comment
                    state = 2;
                    continue; // current character might be \n already
                }
                break;
            case 2:  // one line comment
                if (code[i] == '\n') {
                    i++;
                    return;
                }
                break;
            case 3: // big comment. Nesting may increase or decrease
                // comment closed.
                if (comment_nesting == 0) { 
                    i++;
                    return;
                }
                // still opened
                if (code[i] == '[') {
                    state = 4;
                } else if (code[i] == ']') {
                    state = 5;
                }
                break;
            case 4: // Nesting may increase
                if (code[i] == '[') {
                    comment_nesting++;
                    state = 3;
                } else if (code[i] == ']') {
                    state = 5;
                }
                break;
            case 5: // Nesting may decrease
                if (code[i] == ']') {
                    comment_nesting--;
                    state = 3;
                } else if (code[i] == '[') {
                    state = 4;
                }
                break;
            }
            i++;
        }
    }

public:
    Lexer(const char* filepath) {
        FILE* file = fopen(filepath, "r");
        if (file == nullptr) {
            throw "error opening " + string(filepath);
        }

        // get file size
        fseek(file, 0, SEEK_END);
        const size_t file_len = ftell(file);
        fseek(file, 0, SEEK_SET);
        // read all content
        char* tmp_buf = new char[file_len + 1];
        fread(tmp_buf, 1, file_len, file);
        fclose(file);
        // copy content to code
        code = tmp_buf;
        delete tmp_buf;

        i = 0;
        line = 1;
    }

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

class Parser {
private:
    Lexer lex;
    Token tk;

    void print_and_skip_error(std::initializer_list<TokenName>&& sync_set) {
        // print error here
        printf("Got "); tk.print(); printf(".");
        printf("But expected one of the following: ");
        for (auto name : sync_set) {
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
            case KW_LIST_BEGIN + 1 ... KW_LIST_END - 1:
                printf("<kw>" );
                break;
            case EOTS:
                printf("<EOTS>");
                break;
            default: // tokens that are a single ascii character
                printf("<%c>", name);
        }

        // skip while didn't find a token in sync_set and file not over
        do {
            tk = lex.next_token();
        } while (sync_set.count(tk.name) == 0 &&
            tk.name != EOTS && tk.name != INVALID);
    }

    void ids() {
        if (tk.name == IDENTIFIER) {
            
        } else {
            print_and_skip_error();
        }
    }

public:
    void parse() {
        tk = lex.next_token();
    }
};

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("You should pass the file name as argument!\n");
        return -1;
    }
    
    try {
        Lexer lex(argv[1]);
        Token tk;
        do {
            tk = lex.next_token();
        } while (tk.name != EOTS && tk.name != INVALID);
    } catch (const string& s) {
        printf("%s\n", s.c_str());
    }

    return 0;
}
