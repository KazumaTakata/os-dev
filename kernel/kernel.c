#include "print.h"
#include "string.h"
#include "port.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"

#define GetInInterrupt(arg) __asm__("int %0\n"   \
                                    :            \
                                    : "N"((arg)) \
                                    : "cc", "memory")

void enable()
{
    asm("sti");
}

void main()
{
    char *hel = "hello";
    print_str(hel, 0, 4);
    print_str("boys!", 1, 4);

    port_byte_out(0x03D4, 0x0f);
    port_byte_out(0x03D5, 80);

    port_byte_out(0x03D4, 0x0e);
    port_byte_out(0x03D5, 0);

    init_idt();

    // // GetInInterrupt(100);
    i86_pic_initialize(0x20, 0x28);

    enable();

    i86_pit_initialize();

    i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

    for (;;)
    {
    }
}