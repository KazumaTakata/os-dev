[extern irq_handler]

global irq10

irq10:
	cli
    push byte 10
	push byte 42
	jmp irq_common_stub

    

irq_common_stub:
    pusha 
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq_handler ; Different than the ISR code
    pop eax  ; Different than the ISR code
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8
    sti
    iret 
