.global _start

.section .text
_start:
  movq $0, %rax # array1 index
  movq $0, %rbx # array2 index
  movq $0, %rcx # mergedArray index
  movl $0, %edi # last number in mergedArray


sort_HW1:
  movl array1(%rax), %r8d
  movl array2(%rbx), %r9d

  # check if both arrays ended
  testl %r8d, %r8d
  jnz array1_not_ended_HW1
  testl %r9d, %r9d
  jz end_HW1

array1_not_ended_HW1:
  cmpl %r8d, %r9d # r9d - r8d
  jae array2_bigger_HW1 # jump if array1[i] > array2[j]

  # otherwise
  cmpl %r8d, %edi
  jz array1_index_HW1
  movl %r8d, mergedArray(%rcx) # insert from array1
  movl %r8d, %edi
  addq $4, %rcx

    array1_index_HW1:
      addq $4, %rax
      jmp sort_HW1

  array2_bigger_HW1:
    cmpl %r9d, %edi
    jz array2_index_HW1
    movl %r9d, mergedArray(%rcx) # insert from array2
    movl %r9d, %edi
    addq $4, %rcx

      array2_index_HW1:
        addq $4, %rbx
        jmp sort_HW1

end_HW1:
  # insert 0 at end of mergedArray
  movl $0, mergedArray(%rcx)
