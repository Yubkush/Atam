.global _start

.section .text
_start:
  movq (root), %rax
  movq $new_node, %rbx
  movq (new_node), %r8
  testq %rax, %rax
  jz empty_tree_HW1

  loop_search:
    # check if new_node value is in rax
    movq (%rax), %r9
    cmpq %r8, %r9
    je end_HW1
    jl go_right_HW1
    # check if left son is null
    movq 8(%rax), %rcx
    testq %rcx, %rcx
    jz insert_left_HW1
    # go left
    movq %rcx, %rax
    jmp loop_search

    go_right_HW1:
      # check if right son is null. if it is we can insert
      movq 16(%rax), %rdx
      testq %rdx, %rdx
      jz insert_right_HW1
      movq %rdx, %rax
      jmp loop_search


  empty_tree_HW1:
    movq $new_node, (root)
    jmp end_HW1

  insert_left_HW1:
    movq %rbx, 8(%rax)
    jmp end_HW1

  insert_right_HW1:
    movq %rbx ,16(%rax)

end_HW1:
