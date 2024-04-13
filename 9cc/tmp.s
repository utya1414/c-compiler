.intel_syntax noprefix
.global ret4
ret4:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  push 4
  jmp .L.return.ret4
.L.return.ret4:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  mov rax, 0
  call ret4
  push rax
  jmp .L.return.main
.L.return.main:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
