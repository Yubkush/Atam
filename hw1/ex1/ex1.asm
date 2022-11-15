.global _start

.section .text
_start:

  movq (num), %rax
  movb $0x1, (Bool)

  sarq $1, %rax
  jb carry_HW1 # if carry is 1 change bool and finish 
  sarq $1, %rax
  jb carry_HW1
  jmp end_HW1

carry_HW1:
  movb $0x0, (Bool)

end_HW1:
