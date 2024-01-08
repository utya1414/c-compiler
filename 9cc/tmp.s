.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
.L.begin.1:
  push 10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .L.begin.1
.L.end.1:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
