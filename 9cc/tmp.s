.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 0
  pop rax
  cmp rax, 0
  je  .Lelse
  push 3
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend
.Lelse:
  push 2
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
