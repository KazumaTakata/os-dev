#ifndef __IDT_H
#define __IDT_H

#include "../include/stdint.h"
#include "string.h"
#include "print.h"

#define I86_IDT_DESC_BIT16 0x06   //00000110
#define I86_IDT_DESC_BIT32 0x0E   //00001110
#define I86_IDT_DESC_RING1 0x40   //01000000
#define I86_IDT_DESC_RING2 0x20   //00100000
#define I86_IDT_DESC_RING3 0x60   //01100000
#define I86_IDT_DESC_PRESENT 0x80 //10000000

struct __attribute__((__packed__)) idtr
{
    uint16_t limit;
    uint32_t base;
};

struct __attribute__((__packed__)) ind
{
    uint16_t offset_low;
    uint16_t Segment;
    uint8_t reserved;
    uint8_t flag;
    uint16_t offset_high;
};

void set_interrupt(int index, uint32_t address);
void init_idt();

#endif
