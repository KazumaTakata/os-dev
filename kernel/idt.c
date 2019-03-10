#include "idt.h"

struct ind idt[256];
struct idtr _idtr;

void default_hander()
{
    print_str("interrupt hander is called", 0, 0);
}

void load_idt()
{
    __asm__ __volatile__("lidtl (%0)"
                         :
                         : "r"(&_idtr));
}

void init_idt()
{
    uint32_t address = (uint32_t)default_hander;
    memset((char *)&idt[0], 0, sizeof(idt));

    for (int i = 0; i < 256; i++)
    {
        set_interrupt(i, address);
    }

    _idtr.base = (uint32_t)idt;
    _idtr.limit = (uint16_t)sizeof(struct ind) * 256 - 1;

    load_idt();
}

void set_interrupt(int index, uint32_t address)
{

    uint16_t high_add = (uint16_t)(address >> 16) & 0xffff;
    uint16_t low_add = (uint16_t)address & 0xffff;

    idt[index].offset_high = high_add;
    idt[index].offset_low = low_add;
    idt[index].Segment = 0x8;
    idt[index].flag = I86_IDT_DESC_BIT32 | I86_IDT_DESC_PRESENT;
    idt[index].reserved = 0;
}
