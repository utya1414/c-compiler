.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  mov rax, rbp
  sub rax, 8
  push rax
  push 3
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  push 1
  pop rax
  cmp rax, 0
  je  .L.else.1
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .L.end.1
.L.else.1:
.L.end.1:
  pop rax
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
  pop rax
  mov rsp, rbp
  pop rbp
  ret
