.globl my_ili_handler
.extern what_to_do, old_ili_handler

.text
.align 4, 0x90
my_ili_handler:
  # back up used registers
  pushq %rbx # opcode
  pushq %r12 # opcode 2 bytes flag
  # restart r12
  movq $0, %r12
  # get rip
  movq 16(%rsp), %rbx # instruction pointer
  movq (%rbx), %rbx # get opcode from pointer
  
  # check if opcode is 1 byte(not starts with 0F)
  pushq %rax
  pushq %rdi
  xorq %rdi, %rdi
  xorq %rax, %rax
  cmpb $0x0F, %bl
  jne last_byte
  # two byte opcode
  incq %r12
  movb %bh, %al
  jmp what_to_do_label
  
  last_byte:
  movb %bl, %al
  what_to_do_label:
  movq %rax, %rdi # prepare parameter for what_to_do
  call what_to_do
  popq %rdi
  # check return value
  testq %rax, %rax
  jnz return_back
  # check if rax = 0
  popq %rax
  popq %r12
  popq %rbx
  jmp *old_ili_handler

  return_back:
    movq %rax, %rdi
    incq %r12
    addq %r12, 24(%rsp) # 24(rsp) = rip

  popq %rax
  popq %r12
  popq %rbx
  iretq
