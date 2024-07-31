/* Based from https://github.com/five-embeddev/riscv-scratchpad/blob/master/baremetal-startup-c/src/linker.lds */

OUTPUT_ARCH( "riscv" )

/* ENTRY(_start) */

MEMORY
{
  ROM (rx)  : ORIGIN = 0x00100000, LENGTH = 0x000FFFFF
  RAM (!rx) : ORIGIN = 0x00600000, LENGTH = 0x003FFFFF
}

_entry_point = _vectors_start + 0x80;
ENTRY(_entry_point)

SECTIONS
{

  __stack_size = DEFINED(__stack_size) ? __stack_size : 0x400;
  PROVIDE(__stack_size = __stack_size);

  __heap_size = DEFINED(__heap_size) ? __heap_size : 0x800;
  
  .vectors :
    {
        . = ALIGN(4);
		_vectors_start = .;
        KEEP(*(.vectors))
        . = 0x80;
		_vectors_end = .;
    } > ROM =0xFF

  .text : {
    _ftext = .;
    PROVIDE( eprol = . );
    PROVIDE(_entry_point$ = . + 0x80);
    . = ALIGN(4);
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
    *(.rodata.*)
  } > ROM

  .data : {
    . = ALIGN(4);
    *(.data) 
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.rodata.*)
    _edata = .;
    PROVIDE( __global_pointer$ = . + 0x800 );
  } > RAM
  
  .bss : {
  . = ALIGN(4);
   *(.bss.*)
   __bss_start = .;
  } > RAM
  
  .heap (NOLOAD) : ALIGN(8) {
    PROVIDE( _end = . );
    PROVIDE( end = .);
    PROVIDE( __heap_start = . );
    PROVIDE( metal_segment_heap_target_start = . );
    /* If __heap_max is defined, grow the heap to use the rest of RAM,
      * otherwise set the heap size to __heap_size */
    . = DEFINED(__heap_max) ? MIN( LENGTH(RAM) - ( . - ORIGIN(RAM)) , 0x00400000) : __heap_size;
    PROVIDE( metal_segment_heap_target_end = . );
    PROVIDE( _heap_end = . );
    PROVIDE( __heap_end = . );
    } >RAM 

  /*__global_pointer$ = . + 0x800; */
  /*__global_pointer$ = . + 0x10000;*/
  /*end = .;
  _end = .;*/
}
