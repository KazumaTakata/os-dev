
#include "idt.h"
#include "pit.h"
#include "pic.h"
#include "port.h"

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

//-----------------------------------------------
//	Controller Registers
//-----------------------------------------------
extern void irq10();
struct interrupt_frame
{
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
};

__attribute__((interrupt)) void interrupt_handler(struct interrupt_frame *frame);

#define I86_PIT_REG_COUNTER0 0x40
#define I86_PIT_REG_COUNTER1 0x41
#define I86_PIT_REG_COUNTER2 0x42
#define I86_PIT_REG_COMMAND 0x43

//============================================================================
//    IMPLEMENTATION PRIVATE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================

//! Global Tick count
static volatile uint32_t _pit_ticks = 0;

void interruptdone(unsigned int intno)
{

    //! insure its a valid hardware irq
    if (intno > 16)
        return;

    //! test if we need to send end-of-interrupt to second pic
    if (intno >= 8)
        i86_pic_send_command(I86_PIC_OCW2_MASK_EOI, 1);

    //! always send end-of-interrupt to primary pic
    i86_pic_send_command(I86_PIC_OCW2_MASK_EOI, 0);
}

//! Test if pit is initialized
// static bool _pit_bIsInit = false;

//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================

//!	pit timer interrupt handler
void i86_pit_irq()
{
    asm("cli");
    asm("pusha");
    // asm volatile("mov %ds, %ax");
    // asm volatile("push %eax");
    // asm volatile("mov $0x10,  %ax");
    // asm volatile("mov %ax, %ds");
    // asm volatile("mov %ax, %es");
    // asm volatile("mov %ax, %fs");
    // asm volatile("mov %ax, %gs");
    // ! increment tick count
    _pit_ticks++;
    interruptdone(0);

    print_str("PIT interrupt", _pit_ticks, 0);

    // //! tell hal we are done
    // asm volatile("pop %eax");
    // asm volatile("mov %ax, %ds");
    // asm volatile("mov %ax, %es");
    // asm volatile("mov %ax, %fs");
    // asm volatile("mov %ax, %gs");

    asm("popa");
    // asm volatile("add  $8, %esp");
    asm("sti");
    asm("leave");
    asm("iret");
}

//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================

//! Sets new pit tick count and returns prev. value
uint32_t i86_pit_set_tick_count(uint32_t i)
{

    uint32_t ret = _pit_ticks;
    _pit_ticks = i;
    return ret;
}

//! returns current tick count
uint32_t i86_pit_get_tick_count()
{

    return _pit_ticks;
}

//! send command to pic
void i86_pit_send_command(uint8_t cmd)
{

    port_byte_out(I86_PIT_REG_COMMAND, cmd);
}

//! send data to a counter
void i86_pit_send_data(uint16_t data, uint8_t counter)
{

    uint8_t port = (counter == I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 : ((counter == I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

    port_byte_out(port, (uint8_t)data);
}

//! read data from counter
uint8_t i86_pit_read_data(uint16_t counter)
{

    uint8_t port = (counter == I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 : ((counter == I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

    return port_byte_in(port);
}

//! starts a counter
void i86_pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode)
{

    if (freq == 0)
        return;

    uint16_t divisor = (uint16_t)(1193181 / (uint16_t)freq);

    //! send operational command
    uint8_t ocw = 0;
    ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
    ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
    ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
    i86_pit_send_command(ocw);

    //! set frequency rate
    i86_pit_send_data(divisor & 0xff, 0);
    i86_pit_send_data((divisor >> 8) & 0xff, 0);

    //! reset tick count
    _pit_ticks = 0;
}

//! initialize minidriver
void i86_pit_initialize()
{
    set_interrupt(32, (uint32_t)interrupt_handler);
}

//! test if pit interface is initialized
// bool i86_pit_is_initialized()
// {

//     return _pit_bIsInit;
// }

typedef struct
{
    uint32_t ds;                                     /* Data segment selector */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
    // uint32_t int_no, err_code;                       /* Interrupt number and error code (if applicable) */
    uint32_t eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
} registers_t;

__attribute__((interrupt)) void interrupt_handler(struct interrupt_frame *frame)
{
    interruptdone(0);

    _pit_ticks++;

    print_str("PIT interrupt", 0, 0);
}

void irq_handler(registers_t r)
{
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    // if (r.int_no >= 40)
    //     port_byte_out(0xA0, 0x20); /* slave */
    // port_byte_out(0x20, 0x20);     /* master */

    /* Handle the interrupt in a more modular way */
    // if (interrupt_handlers[r.int_no] != 0)
    // {
    //     isr_t handler = interrupt_handlers[r.int_no];
    //     handler(r);
    // }

    interruptdone(0);

    _pit_ticks++;

    print_str("PIT interrupt", _pit_ticks, 0);
}