.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 0
  pop rax
  cmp rax, 0
  je  .L.else.1
  push 1
  pop rax
  cmp rax, 0
  je  .L.else.2
  push 3
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .L.end.2
.L.else.2:
.L.end.2:
  jmp .L.end.1
.L.else.1:
.L.end.1:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
