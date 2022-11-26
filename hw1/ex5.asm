.global _start

.section .text
_start:

movq (head), %rax # rax is node* tmp
movq $head, %rbx # father of tmp
cmpq $0, %rax 
je end_HW1
movslq val, %r8
movb $0, %cl # bool if we need to swap head node

# while loop to find two nodes with value
movq (%rax), %r9 # tmp value
cmpq %r8, %r9
je swap_root_HW1
loop1_HW1:
  testq %rax, %rax # check if tmp is null
  jz end_HW1
  movq (%rax), %r9 # tmp value
  cmpq %r8, %r9
  je find1_HW1
  movq %rax, %rbx # father = father.next
  movq 8(%rax), %rax # tmp = tmp.next
  jmp loop1_HW1
  
# r10 is node1 father, r11 is node1
swap_root_HW1:
  movb $1, %cl
find1_HW1:
  movq %rbx, %r10
  movq %rax, %r11

loop2_HW1:
  movq %rax, %rbx # father = father.next
  movq 8(%rax), %rax # tmp = tmp.next
  testq %rax, %rax # check if tmp is null
  jz end_HW1
  movq (%rax), %r9 # tmp value
  cmpq %r8, %r9
  je find2_HW1
  jmp loop2_HW1

# r12 is node1 father, r13 is node1
find2_HW1:
  movq %rbx, %r12
  movq %rax, %r13

loop3_HW1:
  movq 8(%rax), %rax # tmp = tmp.next
  testq %rax, %rax # check if tmp is null
  jz swap_HW1
  movq (%rax), %r9 # tmp value
  cmpq %r8, %r9
  je end_HW1
  jmp loop3_HW1
  

swap_HW1:
  testb %cl, %cl
  jz regular_swap_HW1
  # swap head
  # swap fathers pointers
  movq (head), %r14
  movq 8(%r12), %r15
  movq %r14, 8(%r12)
  movq %r15, (head)
  # swap child pointers
  movq 8(%r11), %r14
  movq 8(%r13), %r15
  movq %r14, 8(%r13)
  movq %r15, 8(%r11)
  jmp end_HW1

regular_swap_HW1:
  # swap fathers pointers
  movq 8(%r10), %r14
  movq 8(%r12), %r15
  movq %r14, 8(%r12)
  movq %r15, 8(%r10)
  # swap child pointers
  movq 8(%r11), %r14
  movq 8(%r13), %r15
  movq %r14, 8(%r13)
  movq %r15, 8(%r11)

end_HW1:
