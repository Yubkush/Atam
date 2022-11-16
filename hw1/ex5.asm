.global _start

.section .text
_start:

movq head, %rax # rax is node*
cmpq %rax, $0
je end_HW1
movl value, %r8d
movq $0, %rcx # rcx is counter of nodes with value

# while loop to find two nodes with value
loop1_HW1:
  movl (%rax), %r9d # tmp value
  cmpl %r8d, %r9d
  



end_HW1:
