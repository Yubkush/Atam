.global _start

.section .text
_start:

  movq $0x0, %rdi # dest index
  movq $0x0, %rsi # src index
  movl (num), %eax # RAX = n
  testl %eax, %eax
  jz end_HW1
  # check if num is negative
  cdq
  testl %edx, %edx
  jnz NEG_HW1

  # check if src < des <= src+n jump to overlap_pos
  movq $destination, %r15
  cmpq $source, %r15
  jle pos_loop_HW1
  movq $source, %r14
  movslq %eax, %rax
  addq %rax, %r14 # r14 is $src + n
  cmpq %r15, %r14
  jge overlap_pos

pos_loop_HW1:
  # move source+offset to destination+offset
  movb source(%rsi), %cl
  movb %cl, destination(%rsi)
  # offset++
  inc %rsi

  dec %eax # counter --
  # jump to loop if counter>0 else jump to end
  testl %eax, %eax
  jnz pos_loop_HW1
  jmp end_HW1

overlap_pos:
  movq %rax, %rsi # rsi = n
  dec %rsi
  pos_loop_overlap_HW1:
    # move source+offset to destination+offset
    movb source(%rsi), %cl
    movb %cl, destination(%rsi)
    # offset--
    dec %rsi

    dec %eax # counter --
    # jump to loop if counter>0 else jump to end
    testl %eax, %eax
    jnz pos_loop_overlap_HW1
    jmp end_HW1

NEG_HW1:
  notl %eax
  movslq %eax, %rdi
  inc %eax
  neg_loop_HW1:
    # move source+offset to destination+offset
    movb source(%rsi), %cl
    movb %cl, destination(%rdi)
    # offset changes
    inc %rsi
    dec %rdi

    dec %eax # counter --
    # jump to loop if counter>0 else jump to end
    testl %eax, %eax
    jnz neg_loop_HW1
  
end_HW1:
