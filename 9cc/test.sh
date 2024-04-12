#!/bin/bash
cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
int add(int x, int y) { return x + y; }
int sub(int x, int y) { return x - y; }

int add3(int a, int b, int c) {
    return a+b+c;
}
int add6(int a, int b, int c, int d, int e, int f) {
    return a+b+c+d+e+f;
}
EOF

assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s || exit
    cc -static -o tmp tmp.s tmp2.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$iput => $expected expected, but got $actual"
        exit 1
    fi
}

# # test of + and - and * and / and () and unary operator
# assert 0 "0;"
# assert 42 "42;"
# assert 21 "5+20-4;"
# assert 41 "12 + 34 - 5;"
# assert 47 '5+6*7;'
# assert 15 '5*(9-6);'
# assert 4 '(3+5)/2;'
# assert 10 '-10+20;'
# assert 10 '- -10;'
# assert 10 '- - +10;'

# # test of == and !=
# assert 0 '0==1;'
# assert 1 '42==42;'
# assert 1 '0!=1;'
# assert 0 '42!=42;'

# #test of < and <=
# assert 1 '0<1;'
# assert 0 '1<1;'
# assert 0 '2<1;'
# assert 1 '0<=1;'
# assert 1 '1<=1;'
# assert 0 '2<=1;'

# # test of > and >=
# assert 1 '1>0;'
# assert 0 '1>1;'
# assert 0 '1>2;'
# assert 1 '1>=0;'
# assert 1 '1>=1;'
# assert 0 '1>=2;'

# # test of single-letter variable
# assert 3 "a = 3;
# a;"
# assert 22 "b = 5 * 6 - 8;
# b;"
# assert 3 "a = 3;
# a;"
# assert 14 "a = 3;
# b = 5 * 6 - 8;
# a + b / 2;"

# # test of multi-letter variable
# assert 6 'foo = 1;
# bar = 2 + 3;
# foo + bar;'

# # test of return
# assert 3 'return 3;'
# assert 14 'a = 3;
# b = 5 * 6 - 8;
# return a + b / 2;'

# # test of if statement and nested if statement
# assert 3 'if (1) return 3;'
# assert 3 'if (0) return 2; return 3;'
# assert 3 'a = 3;
# if (a == 3) return a;'
# assert 3 'a = 3;
# if (a != 3) return 2; return a;'
# assert 3 'if (1) if (1) return 3;'
# assert 3 'if (1) if (0) return 2; return 3;'

# # test of if-else statement and nested if-else statement
# assert 3 'if (1) return 3; else return 2;'
# assert 2 'if (0) return 3; else return 2;'
# assert 3 'if (1) if (0) return 2; else return 3;'
# # assert 2 'if (0) if (1) return 3; else return 2;'

# # test of while statement
# assert 10 'while(a < 10) a = a + 1; return a;'

# # test of for statement
# assert 10 'for (a = 0; a < 10; a = a + 1) a = a + 1; return a;'
# assert 10 'for (;;) return 10;'
# assert 9 'for (a = 0;; a = a + 1) if (a == 9) return a;'

# # test of block
# assert 3 '{1; {2;} return 3;}'
# assert 4 'a = 3; if (1) {a = 4;} return a;'

# # test of function call with no argments
assert 3 '{ return ret3(); }'
assert 5 '{ return ret5(); }'

# test of function call with argments
assert 8 '{ return add(3, 5); }'
assert 2 '{ return sub(5, 3); }'
assert 6 '{ return add3(1, 2, 3); }'
assert 21 '{ return add6(1, 2, 3, 4, 5, 6); }'
assert 41 '{ return add6(add6(1, 2, 3, 4, 5, 6), 2, 3, 4, 5, 6); }'

echo OK