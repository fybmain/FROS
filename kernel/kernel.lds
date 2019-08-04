ENTRY(kernel_entry)

SECTIONS {
    . = 0x10000;

    . = ALIGN(4);
    .text : {
        *(.text)
    }

    . = ALIGN(4);
    .data : {
        *(.data)
    }

    . = ALIGN(4);
    .bss : {
        *(.bss)
    }

    . = ALIGN(4);
    .rodata : {
        *(.rodata)
    }
}
