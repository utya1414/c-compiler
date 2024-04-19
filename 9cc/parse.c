#include"9cc.h"

//
// Parser
// 

void debug_token() {
    fprintf(stderr, "token->kind: %d\n", token->kind);
    fprintf(stderr, "token->str: %s\n", token->str);
    fprintf(stderr, "token->len: %d\n", token->len);
    return;
}

bool consume(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) 
        return false;
    token = token->next;
    return true;
}

Token *consume_ident() {
    if (token->kind != TK_IDENT)
        return NULL;
    Token *tok = token;
    token = token->next;
    return tok;
}

bool consume_keyword(char *kw) {
    if (token->kind != TK_KEYWORD ||
        strlen(kw) != token->len ||
        memcmp(token->str, kw, token->len))
        return false;
    token = token->next;
    return true;
}

char *get_ident(Token *tok) {
    if (tok->kind != TK_IDENT) {
        error_at(tok->str, "expected an identifier");
    }
    return strndup(tok->str, tok->len);
}

Node *new_node(NodeKind kind) {
    Node *node = (Node*)calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// ポインタの加算を区別して計算する
Node *new_add(Node *lhs, Node *rhs) {
    add_type(lhs);
    add_type(rhs);
    if (is_integer(lhs->ty) && is_integer(rhs->ty)) {
        return new_binary(ND_ADD, lhs, rhs);
    }

    if (lhs->ty->base && rhs->ty->base) {
        error("invalid pointer arithmetic");
    }
    // num + ptr
    if (!lhs->ty->base && rhs->ty->base) {
        Node *tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }
    // ptr + num int なら4倍する
    rhs = new_binary(ND_MUL, rhs, new_num(lhs->ty->base->size));
    return new_binary(ND_ADD, lhs, rhs);
}

Node *new_sub(Node *lhs, Node *rhs) {
    add_type(lhs);
    add_type(rhs);
    if (is_integer(lhs->ty) && is_integer(rhs->ty)) {
        return new_binary(ND_SUB, lhs, rhs);
    }

    // ptr - ptr, 二つのポインタの間に何個の要素があるか計算できる
    if (lhs->ty->base && rhs->ty->base) {
        Node *node = new_binary(ND_SUB, lhs, rhs);
        node->ty = int_type();
        return new_binary(ND_DIV, node, new_num(lhs->ty->base->size));
    }

    // ptr - num 新しい変数が生成される
    if (lhs->ty->base && !rhs->ty->base) {
        rhs = new_binary(ND_MUL, rhs, new_num(lhs->ty->base->size));
        add_type(rhs);
        Node *node = new_binary(ND_SUB, lhs, rhs);
        node->ty = lhs->ty;
        return node;
    }

    error("invalid operands");
}

Node *new_unary(NodeKind kind, Node *lhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    return node;
}

Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}


Node *new_var_node(LVar *lvar) {
    Node *node = new_node(ND_LVAR);
    node->lvar = lvar;
    return node;
}

LVar *new_lvar(char *name, int len, Type *ty) {
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->name = name;
    lvar->len = len;
    lvar->ty = ty;
    lvar->next = locals;
    locals = lvar;
    return lvar;
}

LVar *find_lvar(Token *tok) {
    for (LVar *lvar = locals; lvar; lvar = lvar->next) {
        if (lvar->len == tok->len && !memcmp(tok->str, lvar->name, lvar->len)) {
            return lvar;
        }
    }
    return NULL;
}

// declspec = "int"
static Type *declspec() {
    // int型のみ対応
    if (strncmp(token->str, "int", token->len)) {
        error_at(token->str, "expected int");
    }
    token = token->next;
    return int_type();
}

// func-params = param ("," param)"
// param = declspec declarator
Type *func_params(Type *ty) {
    Type head = {};
    Type *cur = &head;
    while (!consume(")")) {
        if (cur != &head) {
            expect(",");
        }
        Type *basety = declspec();
        Type *ty = declarator(basety);
        cur = cur->next = copy_type(ty);
    }
    ty = func_type(ty);
    ty->params = head.next;
    return ty;
}

// type-suffix = ("[" num "]")?
//               | ("(" func-params? ")")?
static Type *type_suffix(Type *ty) {
    if (consume("(")) {
        return func_params(ty);
    }
    if (consume("[")) {
        int sz = expect_number();
        ty = array_type(ty, sz);
        expect("]");
        return ty;
    }
    return ty;
}

// 整数型や関数型、ポインタ型の宣言をパースする 
// declarator = "*" ident type-suffix?
static Type *declarator(Type *ty) {
    while (consume("*")) {
        ty = pointer_to(ty);
    }

    if (token->kind != TK_IDENT) {
        error_at(token->str, "expected an variable name");
    }
    Token *tok = token;
    token = token->next;
    ty = type_suffix(ty);
    ty->name = tok;
    return ty;
}

// declaration = declspec (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
static Node *declaration() {
    Type *basety = declspec();
    Node head = {};
    Node *cur = &head;
    while(!consume(";")) {
        if (cur != &head) {
            expect(",");
        }
        Type *ty = declarator(basety);
        LVar *lvar = new_lvar(get_ident(ty->name), ty->name->len, ty);
        if (!consume("=")) {
           continue;
        }
        Node *node = new_binary(ND_ASSIGN, new_var_node(lvar), assign());
        cur = cur->next = new_unary(ND_EXPR_STMT, node);
    }

    Node *node = new_node(ND_BLOCK);
    node->body = head.next;
    return node;
}

// localsに関数の引数を渡す
static void create_param_lvars(Type *param) {
    if (param) {
        create_param_lvars(param->next);
        LVar *lvar = new_lvar(get_ident(param->name), param->name->len, param);
    }
}

// function   = declspec declarator "{" compound_stmt
Function *function() {
    Type *ty = declspec();
    ty = declarator(ty);
    locals = NULL;
    Function *fn = calloc(1, sizeof(Function));
    fn->name = get_ident(ty->name);
    create_param_lvars(ty->params);
    fn->params = locals;
    consume("{");
    fn->body = compound_stmt();
    fn->locals = locals;
    return fn;
}

// compound_stmt = (declaration | stmt)* "}"
Node *compound_stmt() {
    Node *node = new_node(ND_BLOCK);
    Node head = {};
    Node *cur = &head;
    while (!consume("}")) {
        if (!strncmp(token->str, "int", token->len)) {
            cur = cur->next = declaration();
        } else {
            cur = cur->next = stmt();
        }
        add_type(cur);
    }
    node->body = head.next;
    return node;
}

// プログラムは関数のリスト
// program    = function*
Function *program() {
    Function head = {};
    Function *cur = &head;
    while (!at_eof()) {
        cur = cur->next = function();
    }
    return head.next;
}

// stmt    = expr-stmt ";"
//         | "{" compound_stmt 
//         | "if" "(" expr ")" stmt ("else" stmt)?
//         | "while" "(" expr ")" stmt
//         | "for" "(" expr-stmt expr? ";" expr? ")" stmt
//         | "return" expr ";"
Node *stmt() {
    Node *node;

    if (consume("{")) {
        return compound_stmt();
    }

    if (consume_keyword("if")) {
        node = new_node(ND_IF);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume_keyword("else")) {
            node->els = stmt();
        }
        return node;
    } 

    if (consume_keyword("while")) {
        node = new_node(ND_WHILE);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    }

    if (consume_keyword("for")) {
        node = new_node(ND_FOR);
        expect("(");

        node->init = expr_stmt();

        if (!consume(";")) {
            node->cond = expr();
            expect(";");
        }
        if (!consume(")")) {
            node->inc = expr();
            expect(")");
        }
        node->then = stmt();
        return node;
    }

    if (consume_keyword("return")) {
        node = new_node(ND_RETURN);
        node->lhs = expr();
        expect(";");
        return node;
    }

    return expr_stmt();
}

// expr-stmt = expr? ";"
Node *expr_stmt() {
    if (consume(";")) {
        return new_node(ND_BLOCK);
    }
    Node *node = new_node(ND_EXPR_STMT);
    node->lhs = expr();
    expect(";");
    return node;
}

// expr       = assign
Node *expr() {
    return assign();
}

// assign     = equality ("=" assign)?
Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node = new_binary(ND_ASSIGN, node, assign());
    }
    return node;
}

// equality   = relational ("==" relational | "!=" relational)*
Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("==")) {
            node = new_binary(ND_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_binary(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<")) {
            node = new_binary(ND_LT, node, add());
        } else if (consume("<=")) {
            node = new_binary(ND_LE, node, add());
        } else if (consume(">")) {
            node = new_binary(ND_LT, add(), node);
        } else if (consume(">=")) {
            node = new_binary(ND_LE, add(), node);
        } else {
            return node;
        }
    }
}

// add        = mul ("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+")) {
            node = new_add(node, mul());
        } else if (consume("-")) {
            node = new_sub(node, mul());
        } else {
            return node;
        }
    }
}

// mul        = unary ("*" unary | "/" unary)*F
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*")) {
            node = new_binary(ND_MUL, node, unary());
        } else if (consume("/")) {
            node = new_binary(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}

// unary      = "sizeof" unary
//              ("+" | "-")? unary
//              | "*" unary
//              | "&" unary
//              | postfix
Node *unary() {
    if (consume_keyword("sizeof")) {
        Node *node = unary();
        add_type(node);
        return new_num(node->ty->size);
    }
    if (consume("+")) {
        return unary();
    }
    if (consume("-")) {
        return new_binary(ND_SUB, new_num(0), unary());
    }
    if (consume("*")) {
        return new_unary(ND_DEREF, unary());
    }
    if (consume("&")) {
        return new_unary(ND_ADDR, unary());
    }
    return postfix();
}

// postfix = primary ("[ expr "]")*
Node *postfix() {
    Node *node = primary();


    // array access
    if (consume("[")) {
        Node *idx = expr();
        expect("]");
        node = new_unary(ND_DEREF, new_add(node, idx));
    }
    return node;
}

// primary    = num 
//              | ident func-args?
//              | ident "[" primary "]"
//              | "(" expr ")"
Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        // function call
        if (consume("(")) {
            Node *node = new_node(ND_FUNCALL);
            node->funcname = strndup(tok->str, tok->len);
            if (!consume(")")) {
                node->args = assign();
                Node *cur = node->args;
                while (consume(",")) {
                    cur = cur->next = assign();
                }
                expect(")");
            }
            return node;
        }

        LVar *lvar = find_lvar(tok);
       if (!lvar) {
            error("undefined variable");
        }
        return new_var_node(lvar);
    }

    return new_num(expect_number());
}