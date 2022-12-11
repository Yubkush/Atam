.globl my_ili_handler
.extern what_to_do, old_ili_handler

.text
.align 4, 0x90
my_ili_handler:
  # back up used registers
  pushq %rbx # opcode last byte
  pushq %r12 # opcode 2 bytes flag
  pushq %r13 # opcode byte extension
  # restart r12, r13
  movq $0, %r12
  movq $0, %r13
  # get rip
  movq 24(%rsp), %rbx # instruction pointer
  movq (%rbx), %rbx # get opcode from pointer
  movq %rbx, %r13 # put opcode in r13
  shrq $8, %r13 # get rid of last byte, left with 00 or 0F if two byte opcode
  andq $0xFF, %rbx # get only the last byte
  
  # check if opcode is 1 byte(not starts with 0F)
  cmpw $0x0F, %r13w
  jne last_byte
  incq %r12
  
  last_byte:
  pushq %rax
  pushq %rdi
  movq %rbx, %rdi # prepare parameter for what_to_do
  call what_to_do
  popq %rdi
  # check return value
  testq %rax, %rax
  jnz return_back
  # check if rax = 0
  popq %rax
  popq %r13
  popq %r12
  popq %rbx
  jmp *old_ili_handler

  return_back:
    movq %rax, %rdi
    incq %r12
    addq %r12, 32(%rsp) # 32(rsp) = rip

  popq %rax
  popq %r13
  popq %r12
  popq %rbx
  iretq
