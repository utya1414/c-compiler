### Parser

program    = function*
function   = declspec declarator "{" compound_stmt
    declarator = "*" ident type-suffix
        type-suffix = ("(" func-params? ")")?
        func-params = param ("," param)"
        param = declspec declarator
            declspec = "int"
compound_stmt = (declaration | stmt)* "}"
    declaration = declspec (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
stmt    = expr-stmt ";"
        | "{" compound_stmt 
        | "if" "(" expr ")" stmt ("else" stmt)?
        | "while" "(" expr ")" stmt
        | "for" "(" expr-stmt expr? ";" expr? ")" stmt
        | "return" expr ";"
expr-stmt = expr? ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? unary
             | "*" unary
             | "&" unary
primary    = num 
             | ident func-args?
             | "(" expr ")"

'int fib(int n) { if (n <= 1) { return n; } return fib(n - 1) + fib(n - 2); } int main() { return fib(10); }'


program -> int fib(int n) { if (n <= 1) { return n; } return fib(n - 1) + fib(n - 2); } int main() { return fib(10); }
    function -> int fib(int n) { if (n <= 1) { return n; } return fib(n - 1) + fib(n - 2); }
        declspec -> int
        declarator -> fib (int n)
            ident typesuffix -> fib (int n)
                "(" func-params ")" -> (int n)
                    param -> int n
                        declspec -> int
                        declarator -> n
                            ident -> n
        "{"
        compound_stmt
            
        
    function

ADDの前のレジスタ二つにpのアドレスと8(2のときのintが4)