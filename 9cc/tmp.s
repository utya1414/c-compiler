.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 1
  push 2
  push 3
  push 4
  push 5
  push 6
  pop r9
  pop r8
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  call add6
  push rax
  push 2
  push 3
  push 4
  push 5
  push 6
  pop r9
  pop r8
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  call add6
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  pop rax
  mov rsp, rbp
  pop rbp
  ret
