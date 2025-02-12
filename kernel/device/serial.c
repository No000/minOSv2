#include <stdint.h>
#include "../util.h"
#include "serial.h"

void init_pic(void)
{
    io_cli();
    io_out8(PIC0_ICW1, 0x11);
    io_out8(PIC0_ICW2,   32);   // IRQ0-7をINT 0x20-0x27にマップ
    io_out8(PIC0_ICW3,    4);
    io_out8(PIC0_ICW4, 0x01);
    io_out8(PIC0_IMR,  0xff);

    io_out8(PIC1_ICW1, 0x11);
    io_out8(PIC1_ICW2,   40);   // IRQ8-15をINT 0x28-0x2fにマップ
    io_out8(PIC1_ICW3,    2);
    io_out8(PIC1_ICW4, 0x01);
    io_out8(PIC1_IMR,  0xff);

    io_out8(PIC0_IMR,  0xe6);   // タイマとCOM1〜4以外はMask
    io_out8(PIC1_IMR,  0xff);
    io_sti();
}

extern int gpd;
uint16_t comport;
void init_serial(void)
{
    if (gpd) {
        comport = PORT2;
    } else {
        comport = PORT1;
    }

    io_cli();
    io_out8(comport + 1, 0x00);
    io_out8(comport + 3, 0x80);
    io_out8(comport + 0, 0x01);    // 115200 baud rete
    io_out8(comport + 1, 0x00);
    io_out8(comport + 3, 0x03);
    io_out8(comport + 2, 0xC7);
    io_out8(comport + 4, 0x0B);
    io_out8(comport + 1, 0x0d);    // 割り込み許可
    io_sti();
}

static uint8_t serial_thr_empty(void)
{
    return io_in8(comport + 5) & 0x20;
}

static void write_serial(uint8_t c)
{
    while (!serial_thr_empty());
    io_out8(comport, c);
}

void puts_serial(const char *s)
{
    io_cli();
    for (int i = 0; s[i] != '\0'; i++) {
        write_serial(s[i]);
    }
    io_sti();
}

void nputs_serial(const char *s, int n)
{
    io_cli();
    for (int i = 0; i < n; i++) {
        write_serial(s[i]);
    }
    //write_serial('\r');
    //write_serial('\n');
    io_sti();
}
void putn_serial(uint64_t n)
{
    static const char hex[] = "0123456789abcdef";
    char buf[19] = "0x";
    buf[18] = 0;
    for (int i = 0; i < 16; i++) {
        buf[17 - i] = hex[n & 15];
        n >>= 4;
    }
    puts_serial(buf);
}

void putsn_serial(const char *s, uint64_t n)
{
    puts_serial(s);
    putn_serial(n);
    puts_serial("\r\n");
}

void putsp_serial(const char *s, const void *p)
{
    putsn_serial(s, (uint64_t)p);
}

