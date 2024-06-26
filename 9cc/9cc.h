#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include<stdint.h>

typedef struct Type Type;
//
//  Tokenizer
//

typedef enum {
    TK_RESERVED,
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

// node kind
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ, // == 
    ND_NE, // != :5
    ND_LT, // <
    ND_LE, // <=
    ND_ASSIGN, // =
    ND_ADDR, // & 
    ND_DEREF, // * :10
    ND_IF, // if
    ND_WHILE, // while
    ND_FOR, // for
    ND_BLOCK, // block 
    ND_FUNCALL, // function call :15
    ND_LVAR, // local variable
    ND_EXPR_STMT, // expression statement
    ND_RETURN, // return
    ND_NUM, // Integer :19
} NodeKind;

typedef struct Node Node;

// variable or function
typedef struct Obj Obj;

struct Obj {
    char *name;
    Type *ty;    
    bool is_local;
    int offset; // Local variable
    bool is_function; // Grobal or function

    Obj *next;
    Obj *params;
    Node *body;
    Obj *locals;
    int stack_size;
};

extern Obj *locals;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Type *ty;

    // if, else, while, statement
    Node *cond;
    Node *then;
    Node *els;
    
    // for statement
    Node *init;
    Node *inc;
    
    // block
    Node *next;
    Node *body;

    // function call
    char *funcname;
    Node *args;

    // local variable
    Obj *lvar;
    int val;
    int offset;
};

typedef enum {
    TY_INT,
    TY_PTR,
    TY_FUNC,
    TY_ARRAY,
} TypeKind;

struct Type {
    TypeKind kind;
    int size; // size of type

    Type *base; // pointer
    Token *name;


    Type *return_ty; 
    Type *params; 

    Type *next; 

    int array_len; // array length
};

extern char *user_input;
extern Token *token;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
bool consume_keyword(char *kw);
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
// Type 
// 

bool is_integer(Type *ty);
bool is_pointer(Type *ty);
Type *pointer_to(Type *base);
Type *copy_type(Type *ty);
Type *func_type(Type *return_ty);
Type *int_type();
Type *array_type(Type *base, int size);
void add_type(Node *node);

//
// Parser
// 

Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);
Obj *find_lvar(Token *tok);

static Type *type_suffix(Type *ty);
static Type *declspec();
static Node *declaration();
static Type *declarator(Type *ty);
Node *expr_stmt();
Node *compound_stmt();
Obj *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *postfix();
Node *primary();

// 
// generator
// 

void gen(Node *node);
void gen_lval(Node *node);
void codegen(Obj *fns);

int foo();