#include "Lexer.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("You must pass the filename!\n");
        return -1;
    }

    try {
        Lexer lex(argv[1]);
        Token tk;
        do {
            tk = lex.next_token();
            tk.print();
        } while (tk.name != INVALID && tk.name != EOTS);
    } catch (const std::string& str) {
        printf("Error: %s\n", str.c_str());
    }

    return 0;
}