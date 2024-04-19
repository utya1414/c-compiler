.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 5
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 5
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  push 1
  push 12
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 2
  push 4
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  jmp .L.return.main
.L.return.main:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
