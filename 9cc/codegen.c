#include"9cc.h"

static int depth;
static char *argreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
static Obj *current_fn;

static int count(void) {
    static int i = 1;
    return i++;
}

static void push(void) {
    printf("  push rax\n");
    depth++;
}

static void pop(char *arg) {
    printf("  pop %s\n", arg);
    depth--;
}

void load(Type *ty) {
    if (ty->kind == TY_ARRAY) {
        return;
    }
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
}

void store(void) {
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
}

void gen(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        load(node->ty);
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);
        store();
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("  jmp .L.return.%s\n", current_fn->name);
        return;
    case ND_IF: {
        int c = count();
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .L.else.%d\n", c);
        gen(node->then);
        printf("  jmp .L.end.%d\n", c);
        printf(".L.else.%d:\n", c);
        if (node->els) {
            gen(node->els);
        }
        printf(".L.end.%d:\n", c);
        return;
    }
    case ND_WHILE: {
        int c = count();
        printf(".L.begin.%d:\n", c);
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .L.end.%d\n", c);
        gen(node->then);
        printf("  jmp .L.begin.%d\n", c);
        printf(".L.end.%d:\n", c);
        return;
    }
    case ND_FOR: {
        int c = count();
        if (node->init) {
            gen(node->init);
        }
        printf(".L.begin.%d:\n", c);
        if (node->cond) {
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je  .L.end.%d\n", c);
        }
        gen(node->then);
        if (node->inc) {
            gen(node->inc);
        }
        printf("  jmp .L.begin.%d\n", c);
        printf(".L.end.%d:\n", c);
        return;
    }
    case ND_BLOCK:
        for (Node *n = node->body; n; n = n->next) {
            gen(n);
        }
        return;
    case ND_FUNCALL: {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen(arg);
            nargs++;
        }
        for (int i = nargs-1; i >= 0; i--) {
            pop(argreg[i]);
        }
        printf("  mov rax, 0\n"); // 関数呼び出し前にrspを16の倍数にする
        printf("  call %s\n", node->funcname);
        printf("  push rax\n");
        return ;
        }
    case ND_ADDR: 
        // アドレスを返せればよい
        gen_lval(node->lhs);
        return;
    case ND_DEREF: 
        gen(node->lhs);
        load(node->ty);
        return;
    case ND_EXPR_STMT:
        gen(node->lhs);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}

// rbpの上にのっているローカル変数のアドレスを計算してスタックにpushする
void gen_lval(Node *node) {
    switch (node->kind) {
        case ND_LVAR: // ローカル変数のアドレスをスタックにpushする
            printf("  mov rax, rbp\n");
            printf("  sub rax, %d\n", node->lvar->offset);
            printf("  push rax\n");
            return;
        case ND_DEREF: // 右辺値としてポインタの指すアドレスを返す
            gen(node->lhs);
            return;
    }
}

// ローカル変数はあらかじめスタックにpushされる
void assign_lvar_offsets(Obj *fns) {
    for (Obj *fn = fns; fn; fn = fn->next) {
        int offset = 0;
        for (Obj *lvar = fn->locals; lvar; lvar = lvar->next) {
            offset += lvar->ty->size;
            lvar->offset = offset;
        }
        fn->stack_size = offset;
    }
}

void codegen(Obj *fns) {
    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");

    assign_lvar_offsets(fns);
    for (Obj *fn = fns; fn; fn = fn->next) {        
        current_fn = fn;
        printf(".global %s\n", fn->name);
        printf("%s:\n", fn->name);
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");
        printf("  sub rsp, %d\n", fn->stack_size);

        int i = 0;
        for (Obj *lvar = fn->params; lvar; lvar = lvar->next) {
            printf("  mov [rbp-%d], %s\n", lvar->offset, argreg[i++]);
        }
        gen(fn->body);
        printf(".L.return.%s:\n", fn->name);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
    }
}