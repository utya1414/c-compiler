#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

//
//  Tokenizer
//

typedef enum {
    TK_RESERVED,
    TK_RETURN,
    TK_KEYWORD,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

// local variable
typedef struct LVar LVar;

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};

extern LVar *locals;

// node kind
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ, // == 
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_ASSIGN, // =
    ND_IF, // if
    ND_WHILE, // while
    ND_FOR, // for
    ND_LVAR, // local variable
    ND_RETURN, // return
    ND_NUM, // Integer
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;

    // if, else, while, statement
    Node *cond;
    Node *then;
    Node *els;
    
    // for statement
    Node *init;
    Node *inc;
    
    int val;
    int offset;
};

extern char *user_input;
extern Token *token;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
bool consume_return();
void expect(char *op);
int expect_number();
bool at_eof();

Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
bool isidentfirst(char p);
bool isidentrest(char p);
bool is_alnum(char c);
Token *tokenize();

//
// Parser
// 

Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);
LVar *find_lvar(Token *tok);

extern Node *code[];

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// 
// generator
// 

void gen(Node *node);
void gen_lval(Node *node);