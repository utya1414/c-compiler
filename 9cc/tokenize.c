#include "9cc.h"

// 
// Tokenizer
// 

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void expect(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "expected \"%s\"", op);
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "expected a number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

bool isidentfirst(char p) {
    return ('a' <= p && p <= 'z') || ('A' <= p && p <= 'Z') || p == '_';
}

bool isidentrest(char p) {
    return isidentfirst(p) || ('0' <= p && p <= '9');
}

bool is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") ||
            startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>=;{},&", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (startswith(p, "return") && !is_alnum(p[6])) {
            cur = new_token(TK_KEYWORD, cur, p, 6);
            p += 6;
            continue;
        }

        if (startswith(p, "if")) {
            cur = new_token(TK_KEYWORD, cur, p, 2);
            p += 2;
            continue;
        }

        if (startswith(p, "else")) {
            cur = new_token(TK_KEYWORD, cur, p, 4);
            p += 4;
            continue;
        }
        
        if (startswith(p, "while")) {
            cur = new_token(TK_KEYWORD, cur, p, 5);
            p += 5;
            continue;
        }
        
        if (startswith(p, "for")) {
            cur = new_token(TK_KEYWORD, cur, p, 3);
            p += 3;
            continue;
        }

        if (startswith(p, "int")) {
            cur = new_token(TK_KEYWORD, cur, p, 3);
            p += 3;
            continue;
        }

        if (isidentfirst(*p)) {
            char *start = p;
            do {
                p++;
            } while(isidentrest(*p));
            cur = new_token(TK_IDENT, cur, start, p - start);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "invalid token");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
