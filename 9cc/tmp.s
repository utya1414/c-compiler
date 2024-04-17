.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  push 8
  jmp .L.return.main
.L.return.main:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
