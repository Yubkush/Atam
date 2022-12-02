.global count_above

.section bss
.lcomm buffer 20

.section .text
count_above:
  pushq %rbp
  movq %rsp, %rbp
  # save callee-saved registers
  pushq %rbx # %rbx is char separator
  pushq %r12 # EOF flag
  pushq %r13 # %r13 is long limit
  pushq %r14 # %r14 is the counter for the result
  pushq %r15 # %r15 negative flag
  
  movq %rdi, %rbx
  movq $1, %r12
  movq %rsi, %r13
  movq $0, %r14
  movq $0, %r15
  
  call get_path
  # open file
  movq %rax, %rdi # path to file
  movq $0x000, %rsi # read only
  movq $0444, %rdx # file permissions
  movq $2, %rax # open syscall
  syscall # rax has FD

  # prepare to read
  movq %rax, %rdi # file descriptor

  read_numbers:
    test %r12, %r12
    jz finished_file 
    movq $0, %r15
    movq $1, %rdx # read one char at a time
    leaq buffer(%rip), %rsi # string address
    read_single_number:
      movq $0, %rax # for read syscall
      syscall
      testq %rax, %rax # reached EOF
      jz last_number
      cmpb (%rsi), %bl # check if the last char was the seperator
      je not_last
      cmpb $45, (%rsi) # if we get '-' (negative num)
      jne positive
      movq $1, %r15
      jmp read_single_number
      
      positive:
        incq %rsi
        jmp read_single_number

    last_number:
      movq $0, %r12
      incq %rsi
    not_last:
    movq $0, (%rsi) # convert to null terminated string
    pushq %rdi # prepare for atam_atol
    leaq buffer(%rip), %rdi
    call atam_atol
    popq %rdi # get file descriptor back
    
    # iff we got negative number change it with 2 complement
    here:
    cmpq $1, %r15
    jne compare
    notq %rax
    incq %rax

    compare:
      cmpq %r13, %rax # compare if we read number bigger then limit
      jle read_numbers

    incq %r14
    jmp read_numbers
  
  
  finished_file:
    movq %r14, %rax
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    leave
    ret
