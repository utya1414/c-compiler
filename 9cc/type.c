#include"9cc.h"

Type *ty_int = &(Type){TY_INT, 4};
Type *ty_pointer = &(Type){TY_PTR, 8};

bool is_integer(Type *ty) {
    return ty->kind == TY_INT;
}

bool is_pointer(Type *ty) {
    return ty->kind == TY_PTR;
}

Type *pointer_to(Type *base) {
    Type *ty = calloc(1, sizeof(Type));
    ty = ty_pointer;
    ty->base = base;
    return ty;
}

Type *copy_type(Type *ty) {
    Type *ret = calloc(1, sizeof(Type));
    *ret = *ty;
    return ret;
}

Type *func_type(Type *return_ty) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_FUNC;
    ty->return_ty = return_ty;
    return ty;
}

Type *int_type() {
    Type *ty = calloc(1, sizeof(Type));
    ty = ty_int;
    return ty;
}

Type *array_type(Type *base, int len) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_ARRAY;
    ty->size = base->size * len;
    ty->base = base;
    ty->array_len = len;
    return ty;
}

void add_type(Node *node) {
    if (!node || node->ty) {
        return;
    }

    add_type(node->lhs);
    add_type(node->rhs);
    add_type(node->cond);
    add_type(node->then);
    add_type(node->els);

    for (Node *n = node->body; n; n = n->next) {
        add_type(n);
    }

    switch (node->kind) {
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
        node->ty = node->lhs->ty;
        return;
    case ND_ASSIGN:
        if (node->lhs->ty->kind == TY_ARRAY) {
            error("not an lvalue");
        }
        node->ty = node->lhs->ty;
        return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_NUM:
    case ND_FUNCALL:
        node->ty = int_type();
        return;
    case ND_LVAR:
        node->ty = node->lvar->ty;
        return;
    case ND_ADDR:
        if (node->lhs->ty->kind == TY_ARRAY) {
            node->ty = pointer_to(node->lhs->ty->base);
        } else {
            node->ty = pointer_to(node->lhs->ty);
        }
        return ;
    case ND_DEREF:
        if (!node->lhs->ty->base) {
            error("invalid pointer dereference");
        }
        node->ty = node->lhs->ty->base;
        return;
    }
}