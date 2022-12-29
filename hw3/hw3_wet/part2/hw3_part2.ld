/* TODO: Add the other needed fields.*/

ENTRY(_hw3_dance)

SECTIONS
{
  . = 0x400000;
  .text : {
    *(.text)
  } :text

  . = 0x20000;
  .data : {
    *(.data)
  } :data_bss
  .bss : {
    *(.bss)
  } :data_bss

  . = 0x90000000;
  .rodata : {
    *(.rodata)
  } :rodata
}

PHDRS
{
  text PT_LOAD AT(0x400000) FLAGS(3);
  data_bss PT_LOAD AT(0x20000) FLAGS(4);
  rodata PT_LOAD AT(0x90000000) FLAGS(5);
}

EXTERN(greens blacks)