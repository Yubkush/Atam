#include <asm/desc.h>

// TO_DO: store the address of the IDT at the parameter given
void my_store_idt(struct desc_ptr *idtr) {
    asm volatile("sidt %0;"
        :"=m"(*idtr)  // output
        :
        :   // clobbered registers
        );
}

// TO_DO: load the address of the IDT from the parameter given
void my_load_idt(struct desc_ptr *idtr) {
    asm volatile("lidt %0;"
        :   // output
        :"m"(*idtr)
        :   // clobbered registers
        );
}

// TO_DO: set the address in the right place at the parameter gate
// try to remember - how this information is stored?
void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
    gate->offset_low = addr;
    gate->offset_middle = addr >> 16;
    gate->offset_high = addr >> 32;
    // asm ("movw %%bx, (%1);"
    //       "sar $16, %%rbx;"
    //       "movw %%bx, 6(%1);"
    //       "sar $16, %%rbx;"
    //       "movl %%ebx, 8(%1);"
    //     :"+b"(addr)// output
    //     :"g"(gate)
    //     :"rbx"   // clobbered registers
    //     );
}

// TO_DO: return val is the address stored at the parameter gate_desc gate
// try to remember - how this information is stored?
unsigned long my_get_gate_offset(gate_desc *gate) {
    unsigned long addr = gate->offset_high;
    addr = addr << 16;
    addr += gate->offset_middle;
    addr = addr << 16;
    return addr + gate->offset_low;

    // unsigned long addr = 0;
    // asm ("movl 8(%1), %%ebx;"
    //       "shl $16, %%rbx;"
    //       "movw 6(%1), %%bx;"
    //       "shl $16, %%rbx;"
    //       "movw (%1), %%bx;"
    //       "movq %%rbx, %0"
    //     :"+r"(addr)   // output
    //     :"g"(gate) // input
    //     :"rbx"   // clobbered registers
    //     );
    
    return addr;
}
