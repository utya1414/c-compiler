#include"9cc.h"

char *user_input;
Token *token;
LVar *locals;

int main(int argc, char **argv) {
    if (argc != 2) {
        error("引数の個数が正しくありません\n");
        return 1;
    }

    user_input = argv[1];
    token = tokenize();
    Function *fns = program();

    codegen(fns);
    return 0;
}