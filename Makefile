C_SOURCES = ${wildcard kernel/*.c}
HEADERS = ${wildcard kernel/*.h}
CC = /usr/local/bin/i386-elf-gcc
GDB = /usr/local/Cellar/i386-elf-gdb/8.2/bin/i386-elf-gdb
CFLAGS = -g -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		 -Wall -Wextra -Werror

OBJ = ${C_SOURCES:.c=.o} 

all: os-image.bin


os-image.bin: Boot1.bin Boot2.lbin
	cat  $^ > $@

os-test-image.bin: Boot1.bin test.bin
	cat  $^ > $@

%.lbin: ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 Boot2.o ${OBJ} --oformat binary

%.bin: bootloader/%.asm
	nasm $< -f bin -o $@

%.o: bootloader/%.asm
	nasm $< -f elf -o $@

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@


