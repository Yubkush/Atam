#include <asm/desc.h>

// TO_DO: store the address of the IDT at the parameter given
void my_store_idt(struct desc_ptr *idtr) {
    asm ("sidt %0;"
        :   // output
        :"=m"(idtr)
        :   // clobbered registers
        );
}

// TO_DO: load the address of the IDT from the parameter given
void my_load_idt(struct desc_ptr *idtr) {
    asm ("lidt %0;"
        :   // output
        :"+m"(idtr)
        :   // clobbered registers
        );
}

// TO_DO: set the address in the right place at the parameter gate
// try to remember - how this information is stored?
void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
    asm ("movw %%bx, (%1);
          sar $16, %%rbx;
          movw %%bx, 6(%1);
          sar $16, %%rbx;
          movl %%ebx, 8(%1);"
        :   // output
        :"+b"(addr),
         "m"(gate)
        :"rbx"   // clobbered registers
        );
}

// TO_DO: return val is the address stored at the parameter gate_desc gate
// try to remember - how this information is stored?
unsigned long my_get_gate_offset(gate_desc *gate) {
    unsigned long addr;
    asm ("movl 8(%1), %%ebx;
          shl $16, %%rbx
          movw 6(%1), %%bx;
          shl $16, %%rbx
          movw (%1), %%bx;
          "
        :"+b"(addr)   // output
        :"m"(gate) // input
        :"rbx"   // clobbered registers
        );
    
    return addr;
}
