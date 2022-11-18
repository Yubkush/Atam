.global _start

.section .text
_start:

  movl $0x0, %edi # dest index
  movl $0x0, %esi # src index
  movl (num), %eax # RAX = n
  # check if num is negative
  cdq
  testl %edx, %edx
  jnz NEG_HW1

pos_loop_HW1:
  # move source+offset to destination+offset
  movb source(%esi), %cl
  movb %cl, destination(%edi)
  # offset++
  inc %esi
  inc %edi

  dec %eax # counter --
  # jump to loop if counter>0 else jump to end
  testl %eax, %eax
  jnz pos_loop_HW1
  jmp end_HW1

NEG_HW1:
  notl %eax
  movl %eax, %edi
  inc %eax
  neg_loop_HW1:
    # move source+offset to destination+offset
    movb source(%esi), %cl
    movb %cl, destination(%edi)
    # offset changes
    inc %esi
    dec %edi

    dec %eax # counter --
    # jump to loop if counter>0 else jump to end
    testl %eax, %eax
    jnz neg_loop_HW1
  
end_HW1:
