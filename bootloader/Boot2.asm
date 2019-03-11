[bits	16]	

Start: jmp boot2

%include "/Users/kazumatakata/c/os-dev/bootloader/Gdt.asm"
%include "/Users/kazumatakata/c/os-dev/bootloader/bootinfo.inc"
%include "/Users/kazumatakata/c/os-dev/bootloader/memory.asm"
LoadingMsg db "Preparing to load operating system...", 0x0D, 0x0A, 0x00

Print: 
    lodsb
    or   al, al
    jz   PrintDone
    mov  ah, 0eh
    int  10h
    jmp  Print
PrintDone:
    ret


boot_info:
istruc multiboot_info
	at multiboot_info.flags,			dd 0
	at multiboot_info.memoryLo,			dd 0
	at multiboot_info.memoryHi,			dd 0
	at multiboot_info.bootDevice,		dd 0
	at multiboot_info.cmdLine,			dd 0
	at multiboot_info.mods_count,		dd 0
	at multiboot_info.mods_addr,		dd 0
	at multiboot_info.syms0,			dd 0
	at multiboot_info.syms1,			dd 0
	at multiboot_info.syms2,			dd 0
	at multiboot_info.mmap_length,		dd 0
	at multiboot_info.mmap_addr,		dd 0
	at multiboot_info.drives_length,	dd 0
	at multiboot_info.drives_addr,		dd 0
	at multiboot_info.config_table,		dd 0
	at multiboot_info.bootloader_name,	dd 0
	at multiboot_info.apm_table,		dd 0
	at multiboot_info.vbe_control_info,	dd 0
	at multiboot_info.vbe_mode_info,	dw 0
	at multiboot_info.vbe_interface_seg,dw 0
	at multiboot_info.vbe_interface_off,dw 0
	at multiboot_info.vbe_interface_len,dw 0
iend



boot2: 
    ;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;
 
	cli				; clear interrupts
	xor	ax, ax			; null segments
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x9000		; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
	sti				; enable interrupts
 
	;-------------------------------;
	;   Print loading message	;
	;-------------------------------;
    mov	al, 2	; set bit 2 (enable a20)
    out	0x92, al

    mov al, 0xdd	; command 0xdd: enable a20
    out 0x64, al	; send command to controller


	mov	si, LoadingMsg
	call	Print
 
	;-------------------------------;
	;   Install our GDT		;
	;-------------------------------;
 
	call	InstallGDT		; install our GDT




	xor		eax, eax
	xor		ebx, ebx
	call	BiosGetMemorySize64MB

	mov		word [boot_info+multiboot_info.memoryHi], bx
	mov		word [boot_info+multiboot_info.memoryLo], ax



	mov		eax, 0x0
	mov		ds, ax
	mov		di, 0x2000
	call	BiosGetMemoryMap
 
	;-------------------------------;
	;   Go into pmode		;
	;-------------------------------;
 
	cli				; clear interrupts
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax
 
	jmp	08h:Stage3		; far jump to fix CS. Remember that the code selector is 0x8!


bits 32					; Welcome to the 32 bit world!
 
Stage3:
 
	;-------------------------------;
	;   Set registers		;
	;-------------------------------;
 
	mov	ax, 0x10	
	mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000 ; 6. update the stack right at the top of the free space
    mov esp, ebp


    ; %define	VIDMEM	0xB8000		
 
	; mov	edi, VIDMEM		; get pointer to video memory
	; mov	Byte [edi], 'A'		; print character 'A'
	; mov Byte [edi+1], 0x7		; character attribute

	push	dword boot_info

    [extern main] ; Define calling point. Must have same name as kernel.c 'main' function
    call main ; Calls the C function. The linker will know where it is placed in memory
	jmp $
 
;*******************************************************
;	Stop execution
;*******************************************************
 
STOP:
 
	cli
	hlt