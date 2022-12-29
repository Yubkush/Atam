.section .data
index: .int 1

.section .bss
.lcomm final, 4

.global _hw3_dance
.section .text
_hw3_dance:
    mov $60, %rax
    mov $0, %rdi
    syscall
