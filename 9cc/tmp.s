.intel_syntax noprefix
.global fib
fib:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je  .L.else.1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  jmp .L.return.fib
  jmp .L.end.1
.L.else.1:
.L.end.1:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  mov rax, 0
  call fib
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 2
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  mov rax, 0
  call fib
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  jmp .L.return.fib
.L.return.fib:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  push 10
  pop rdi
  mov rax, 0
  call fib
  push rax
  jmp .L.return.main
.L.return.main:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
