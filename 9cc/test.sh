#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
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
assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 "12 + 34 - 5;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 10 '- -10;'
assert 10 '- - +10;'

# test of == and !=
assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

#test of < and <=
assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

# test of > and >=
assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

# test of single-letter variable
assert 3 "a = 3;
a;"
assert 22 "b = 5 * 6 - 8;
b;"
assert 3 "a = 3;
a;"
assert 14 "a = 3;
b = 5 * 6 - 8;
a + b / 2;"

# test of multi-letter variable
assert 6 'foo = 1;
bar = 2 + 3;
foo + bar;'

# test of return
assert 3 'return 3;'
assert 14 'a = 3;
b = 5 * 6 - 8;
return a + b / 2;'

# test of if statement and nested if statement
assert 3 'if (1) return 3;'
assert 3 'if (0) return 2; return 3;'
assert 3 'a = 3;
if (a == 3) return a;'
assert 3 'a = 3;
if (a != 3) return 2; return a;'
assert 3 'if (1) if (1) return 3;'
assert 3 'if (1) if (0) return 2; return 3;'

# test of if-else statement and nested if-else statement
assert 3 'if (1) return 3; else return 2;'
assert 2 'if (0) return 3; else return 2;'
assert 3 'if (1) if (0) return 2; else return 3;'
# assert 2 'if (0) if (1) return 3; else return 2;'

echo OK