#!/bin/bash
cat <<EOF | gcc -xc -c -o tmp2.o -
    #include<stdlib.h>
    void alloc4(int **tmp, int a, int b, int c, int d) {
        *tmp = malloc(4 * sizeof(int));
        (*tmp)[0] = a;
        (*tmp)[1] = b;
        (*tmp)[2] = c;
        (*tmp)[3] = d;
    }
EOF

assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s || exit
    cc -static -g -o tmp tmp.s tmp2.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$iput => $expected expected, but got $actual"
        exit 1
    fi
}

# test of + and - and * and / and () and unary operator
assert 0 "int main() { 0; }"
assert 42 "int main() { 42; }"

assert 21 "int main() { 5+20-4; }"
assert 41 "int main() { 12 + 34 - 5; }"
assert 47 'int main() { 5+6*7; }'
assert 15 'int main() { 5*(9-6); }'
assert 4 'int main() { (3+5)/2; }'
assert 10 'int main() { -10+20; }'
assert 10 'int main() { - -10; }'
assert 10 'int main() { - - +10; }'

# test of == and !=
assert 0 'int main() { 0==1; }'
assert 1 'int main() { 42==42; }'
assert 1 'int main() { 0!=1; }'
assert 0 'int main() { 42!=42; }'

# test of < and <=
assert 1 'int main() { 0<1; }'
assert 0 'int main() { 1<1; }'
assert 0 'int main() { 2<1; }'
assert 1 'int main() { 0<=1; }'
assert 1 'int main() { 1<=1; }'
assert 0 'int main() { 2<=1; }'
# test of > and >=
assert 1 'int main() { 1>0; }'
assert 0 'int main() { 1>1; }'
assert 0 'int main() { 1>2; }'
assert 1 'int main() { 1>=0; }'
assert 1 'int main() { 1>=1; }'
assert 0 'int main() { 1>=2; }'

# test of single-letter variable
assert 3 "int main() { int a; a = 3; return a; }"
assert 22 "int main() { int b; b = 5 * 6 - 8; return b; }"
assert 9 "int main() { int a; int b; a = 3; b = 6; return a + b; }"
assert 14 "int main() { int a; int b; a = 3; b = 5 * 6 - 8; return a + b / 2; }"

# test of multi-letter variable
assert 6 'int main() { int foo; int bar; foo = 1; bar = 2 + 3; return foo + bar; }'

# test of return
assert 3 'int main() { return 3; }'
assert 14 'int main() { int a; int b; a = 3; b = 5 * 6 - 8; return a + b / 2; }'

# test of if statement and nested if statement
assert 3 'int main() { if (1) return 3; }'
assert 3 'int main() { if (0) return 2; return 3; }'
assert 3 'int main() { int a; a = 3; if (a == 3) return a; }'
assert 3 'int main() { int a; a = 3; if (a != 3) return 2; return a; }'
assert 3 'int main() { if (1) if (1) return 3; }'
assert 3 'int main() { if (1) if (0) return 2; return 3; }'

# test of if-else statement and nested if-else statement
assert 3 'int main() { if (1) return 3; else return 2; }'
assert 2 'int main() { if (0) return 3; else return 2; }'
assert 3 'int main() { if (1) { if (0) return 2; else return 3; } }'
assert 2 'int main() { if (0) { if (1) return 3; } else return 2; }'

# test of while statement
assert 10 'int main() { int a; a = 0; while(a < 10) a = a + 1; return a; }'

# test of for statement
assert 10 'int main() { int a; for (a = 0; a < 10; a = a + 1) a = a + 1; return a; }'
assert 10 'int main() { for (;;) return 10; }'
assert 9 'int main() { int a; for (a = 0;; a = a + 1) if (a == 9) return a; }'

# test of block
assert 3 'int main() { 1; {2;} return 3; }'
assert 4 'int main() { int a; a = 3; if (1) {a = 4;} return a; }'

# test of function call with no arguments
assert 3 'int ret3() { return 3; } int main() { return ret3(); }'
assert 5 'int ret5() { return 5; } int main() { return ret5(); }'

# test of function call with arguments
assert 8 'int add(int a, int b) { return a + b; } int main() { return add(3, 5); }'
assert 2 'int sub(int a, int b) { return a - b; } int main() { return sub(5, 3); }'
assert 6 'int add3(int a, int b, int c) { return a + b + c; } int main() { return add3(1, 2, 3); }'
assert 21 'int add6(int a, int b, int c, int d, int e, int f) { return a + b + c + d + e + f; } int main() { return add6(1, 2, 3, 4, 5, 6); }'
assert 41 'int add6(int a, int b, int c, int d, int e, int f) { return a + b + c + d + e + f; } int main() { return add6(add6(1, 2, 3, 4, 5, 6), 2, 3, 4, 5, 6); }'

# test of function definition
assert 4 'int ret4() { return 4; } int main() { return ret4(); }'
assert 4 'int retarg(int a) { return a; } int main() { return retarg(4); }'
assert 55 'int fib(int n) { if (n <= 1) { return n; } return fib(n - 1) + fib(n - 2); } int main() { return fib(10); }'

# # test of & and *
assert 3 'int main() { int a; int *y; a = 3; y = &a; return *y; }'
assert 3 'int main() { int x=3; return *&x; }'
assert 3 'int main() { int x; int *y; y = &x; *y = 3; return x;}'

# # # test of sizeof
assert 8 'int main() { return sizeof(1); }'
assert 8 'int main() { int x; return sizeof(x); }'
assert 8 'int main() { int *x; return sizeof(x); }'

# test of arrary
assert 1 'int main() { int x[2]; *x = 1; return *x; }'
assert 2 'int main() { int x[2]; *(x+1) = 2; return *(x+1); }'
assert 3 'int main() { int x[2]; *x = 1; *(x+1) = 2; int *p; p =x; return *p + *(p+1); }'

# # test of multi-dimentional array
assert 0 'int main() { int x[2][3]; int *y=x; *y=0; return **x; }'
assert 1 'int main() { int x[2][3]; int *y=x; *(y+1)=1; return *(*x+1); }'
assert 2 'int main() { int x[2][3]; int *y=x; *(y+2)=2; return *(*x+2); }'
assert 3 'int main() { int x[2][3]; int *y=x; *(y+3)=3; return **(x+1); }'
assert 4 'int main() { int x[2][3]; int *y=x; *(y+4)=4; return *(*(x+1)+1); }'
assert 5 'int main() { int x[2][3]; int *y=x; *(y+5)=5; return *(*(x+1)+2); }'

# test of index access
assert 3 'int main() { int x[2]; x[0] = 1; x[1] = 2; return x[0] + x[1]; }'
assert 3 'int main() { int x[2]; x[0] = 1; x[1] = 2; int *p; p = x; return *p + *(p+1); }'
assert 0 'int main() { int x[2][3]; int *y=x; y[0]=0; return x[0][0]; }'
assert 1 'int main() { int x[2][3]; int *y=x; y[1]=1; return x[0][1]; }'
assert 2 'int main() { int x[2][3]; int *y=x; y[2]=2; return x[0][2]; }'
assert 3 'int main() { int x[2][3]; int *y=x; y[3]=3; return x[1][0]; }'
assert 4 'int main() { int x[2][3]; int *y=x; y[4]=4; return x[1][1]; }'
assert 5 'int main() { int x[2][3]; int *y=x; y[5]=5; return x[1][2]; }'


echo OK