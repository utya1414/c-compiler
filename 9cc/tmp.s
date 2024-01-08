.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  mov rax, rbp
  sub rax, 8
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.L.begin.1:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 9
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je  .L.else.2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .L.end.2
.L.else.2:
.L.end.2:
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .L.begin.1
.L.end.1:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
