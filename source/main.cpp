#include "Parser.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("You must pass the filename!\n");
    return -1;
  }

  try {
    Parser parser(argv[1]);
    parser.parse();
  } catch (const std::string& str) {
    printf("Fatal error: %s\n", str.c_str());
  }

  return 0;
}