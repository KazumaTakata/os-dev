#include "print.h"
#include "string.h"
#include "port.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "../include/bootinfo.h"
#include "phys_mem.h"

#define GetInInterrupt(arg) __asm__("int %0\n"   \
                                    :            \
                                    : "N"((arg)) \
                                    : "cc", "memory")

void enable()
{
    asm("sti");
}

//! format of a memory region
struct memory_region
{

    uint32_t startLo;
    uint32_t startHi;
    uint32_t sizeLo;
    uint32_t sizeHi;
    uint32_t type;
    uint32_t acpi_3_0;
};

void main(struct multiboot_info *bootinfo)
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

    uint32_t memSize = 1024 + bootinfo->m_memoryLo + bootinfo->m_memoryHi * 64;

    pmmngr_init(memSize, 0x20000);

    struct memory_region *region = (struct memory_region *)0x2000;

    for (int i = 0; i < 6; ++i)
    {

        //! sanity check; if type is > 4 mark it reserved
        if (region[i].type > 4)
            break;

        //! if start address is 0, there is no more entries, break out
        if (i > 0 && region[i].startLo == 0)
            break;

        //! display entry
        // DebugPrintf("region %i: start: 0x%x%x length (bytes): 0x%x%x type: %i (%s)\n", i,
        //             region[i].startHi, region[i].startLo,
        //             region[i].sizeHi, region[i].sizeLo,
        //             region[i].type, strMemoryTypes[region[i].type - 1]);

        //! if region is avilable memory, initialize the region for use
        if (region[i].type == 1)
            if (i != 0)
            {
                pmmngr_init_region(region[i].startLo, region[i].sizeLo);
            }
    }

    for (;;)
    {
    }
}