.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  push 2
  push 3
  pop rdi
  jmp .L.return.main
.L.return.main:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
