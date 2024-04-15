### Parser

program    = function*
function   = declarator "{" compound_stmt
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
