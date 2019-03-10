C_SOURCES = ${wildcard kernel/*.c}
HEADERS = ${wildcard kernel/*.h}
CC = /usr/local/bin/i386-elf-gcc
GDB = /usr/local/Cellar/i386-elf-gdb/8.2/bin/i386-elf-gdb
CFLAGS = -g -m32 -nostdlib  -mno-80387 -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		#  -Wall -Wextra -Werror

OBJ = ${C_SOURCES:.c=.o} 

all: os-image.bin



os-image.bin: Boot1.bin Boot2.lbin
	cat  $^ > $@

os-test-image.bin: Boot1.bin test.bin
	cat  $^ > $@

%.lbin: ${OBJ} Boot2.o interrupt.o
	i386-elf-ld -o $@ -Ttext 0x1000 Boot2.o ${OBJ} interrupt.o --oformat binary

%.bin: bootloader/%.asm
	nasm $< -f bin -o $@

Boot2.o: bootloader/Boot2.asm
	nasm $< -f elf -o $@

interrupt.o: kernel/interrupt.asm
	nasm $< -f elf -o $@

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

kernel.elf: ${OBJ} Boot2.o interrupt.o
	i386-elf-ld -o $@ -Ttext 0x1000 Boot2.o ${OBJ} interrupt.o

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -fda os-image.bin -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

