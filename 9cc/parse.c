#include"9cc.h"

//
// Parser
// 

void debug_token() {
    fprintf(stderr, "token->kind: %d\n", token->kind);
    fprintf(stderr, "token->str: %s\n", token->str);
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

Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *new_unary(NodeKind kind, Node *lhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
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
    for (LVar *var = locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}

static Type *declspec() {
    token = token->next;
    return ty_int;
}

// declarator = "*" ident
static Type *declarator(Type *ty) {
    while (consume("*")) {
        ty = pointer_to(ty);
    }

    if (token->kind != TK_IDENT) {
        error_at(token->str, "expected an variable name");
    }

    ty->name = token;
    token = token->next;
    return ty;
}

// declaration = declspec (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
static Node *declaration() {
    Type *ty = declspec();
    Node head = {};
    Node *cur = &head;
    while(!consume(";")) {
        if (cur != &head) {
            expect(",");
        }
        Type *ty = declarator(ty);
        LVar *var = new_lvar(get_ident(ty->name), ty->name->len, ty);
        fprintf(stderr, "token->str: %s\n", token->str);
        if (!consume("=")) {
           continue;
        }
        Node *node = new_binary(ND_ASSIGN, new_var_node(var), assign());
        cur = cur->next = new_unary(ND_EXPR_STMT, node);
    }

    Node *node = new_node(ND_BLOCK);
    node->body = head.next;
    return node;
}

// function   = ident "(" (assign ("," assign)*)? ")" { compound_stmt
Function *function() {
    Type *ty = declspec();
    ty = declarator(ty);
    locals = NULL;
    Function *fn = calloc(1, sizeof(Function));
    fn->name = get_ident(ty->name);
    if (consume("(")) {
        LVar head = {};
        LVar *cur = &head;
        while (!consume(")")) {
            if (cur != & head) {
                expect(",");
            }
            Token *tok = consume_ident();
            LVar *lvar = calloc(1, sizeof(LVar));
            lvar->name = get_ident(tok);
            lvar->len = tok->len;
            lvar->offset = 8;
            cur = cur->next = lvar;            
        }
        fn->params = head.next;
    }

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
        if (consume_keyword("int")) {
            cur = cur->next = declaration();
        } else {
            cur = cur->next = stmt();
        }
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
            node = new_binary(ND_ADD, node, mul());
        } else if (consume("-")) {
            node = new_binary(ND_SUB, node, mul());
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

// unary      = ("+" | "-")? unary
//              | "*" unary
//              | "&" unary
Node *unary() {
    if (consume("+")) {
        return unary();
    }
    if (consume("-")) {
        return new_binary(ND_SUB, new_num(0), unary());
    }
    if (consume("*")) {
        return new_binary(ND_DEREF, unary(), NULL);
    }
    if (consume("&")) {
        return new_binary(ND_ADDR, unary(), NULL);
    }
    return primary();
}

// primary    = num 
//              | ident ("(" (num ("," num)*)? ")")?
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
        
        Node *node = new_node(ND_LVAR);

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = get_ident(tok);
            lvar->len = tok->len;
            if (locals) {
                lvar->offset = locals->offset + 8;
            } else {
                lvar->offset = 8;
            }
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_num(expect_number());
}