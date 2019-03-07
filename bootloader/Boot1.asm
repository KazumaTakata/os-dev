org 0x7c00

bits	16					; We are still in 16 bit Real Mode
 
Start: jmp loader
msg  db  "Welcome to Operating System!", 0

Print: 
    lodsb
    or   al, al
    jz   PrintDone
    mov  ah, 0eh
    int  10h
    jmp  Print
PrintDone:
    ret




Reset:
	mov		ah, 0					; reset floppy disk function
	mov		dl, 0					; drive 0 is floppy drive
	int		0x13					; call BIOS
	jc		Reset					; If Carry Flag (CF) is set, there was an error. Try resetting again
 
	mov		bx, 0x1000				; we are going to read sector to into address 0x1000:0
	
    ret
 
Read:
	mov		ah, 0x02				; function 2
	mov		al, 10h					; read 1 sector
	mov		ch, 0h					; we are reading the second sector past us, so its still on track 1
	mov		cl, 2h					; sector to read (The second sector)
	mov		dh, 0h					; head number
	mov		dl, 0h					; drive number. Remember Drive 0 is floppy drive.
	int		0x13					; call BIOS - Read the sector
	jc		Read					; Error, so try again
	jmp		0x1000
    ret	

ResetRead:
    call Reset
    call Read
    ret

loader: 
    xor ax, ax
    mov ds, ax
    mov es, ax
    

    mov si, msg
    call Print

    ; call ResetRead
    mov bx, 0x1000 
    mov dh, 2 ; read 2 sectors
    ; the bios sets 'dl' for our boot disk number
    ; if you have trouble, use the '-fda' flag: 'qemu -fda file.bin'
    ; call disk_load
    call ResetRead

    ; mov si, 0x1000
    ; call Print


   
   

    xor ax, ax
    
    cli 
    hlt

%include "/Users/kazumatakata/c/os-dev/bootloader/disk.asm"
	
times 510 - ($-$$) db 0				; We have to be 512 bytes. Clear the rest of the bytes with 0
 
dw 0xAA55		