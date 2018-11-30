#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <DOS.H>
#include <BIOS.H>

#define    IRQ            5
#define    IRQMask        0x20
#define    VECTOR        (IRQ + 8)

void interrupt read_event(void);

void init_irq(void);

void cleanup(void);

void interrupt ( *last_function)();

#define LINE_write(l, v) outport((l), (v))
#define LINE_set_bit(l, i) outport((l), inport((l)) | (1 << (i)))
#define LINE_unset_bit(l, i) outport((l), inport((l)) & ~(1 << (i)))

#define LINE_A     (BA + 0)
#define LINE_B     (BA + 1)
#define LINE_C     (BA + 2)
#define LINE_CTRL  (BA + 3)

#define CTRL_LINE_A      4
#define CTRL_LINE_B      1
#define CTRL_LINE_C_HIGH 3
#define CTRL_LINE_C_LOW  0
#define CTRL_LINE_set_input(l)  LINE_set_bit(LINE_CTRL, (l))
#define CTRL_LINE_set_output(l) LINE_unset_bit(LINE_CTRL, (l))

#define EVENT_LINE_A     (BA + 6)
#define EVENT_LINE_B     (BA + 7)
#define EVENT_LINE_C     (BA + 8)
#define EVENT_RESET      0
#define EVENT_SET        ~(EVENT_RESET)


unsigned int Event;
unsigned int BA;        // Базовый адрес UNIO (ch.0 -23)
unsigned long Count[24];
unsigned char Set[24];
unsigned long Pass;
unsigned int Td = 3, Fr = 1;

union Line {
    struct Bits {
        unsigned int x0: 1;
        unsigned int x1: 1;
        unsigned int x2: 1;
        unsigned int x3: 1;
        unsigned int x4: 1;
        unsigned int x5: 1;
        unsigned int x6: 1;
        unsigned int x7: 1;
    } bits;
    unsigned char data;
};

static void print_line(int line) {
    union Line c;

    c.data = (unsigned char) inportb(line);
    printf("Line: %d\n", line);
    printf("X7 X6 X5 X4 X3 X2 X1 X0\n");
    printf("%2d %2d %2d %2d %2d %2d %2d %2d\n",
           c.bits.x7, c.bits.x6, c.bits.x5, c.bits.x4, c.bits.x3, c.bits.x2,
           c.bits.x1, c.bits.x0);
}




void main(int arg, char **av) {
    unsigned int i, k;

    if (arg > 1 && (*av[1] == '/' || *av[1] == '?')) {
        printf(
                "ct_p55i Fr Td\n"
                "         |  |\n"
                "         |  +--- time freq (code 0-3)\n"
                "         +------ front (code 0-3)\n"
        );
        return;
    }

    if (arg > 1) {
        sscanf(av[1], "%d", &Fr);
        if (Fr > 3) {
            printf("Front must be 0 to 3");
            return;
        }
    }
    if (arg > 2) {
        sscanf(av[2], "%d", &Td);
        if (Td > 3) {
            printf("Time freq must be 0 to 3");
            return;
        }
    }
#if defined(FASTWEL_EMULATE)
    BA = 0x10;
    LINE_write(LINE_CTRL, 0x1B);
    print_line(LINE_CTRL);
#endif
    printf("UNIOxx-5 schema:\"p55\"  Fastwel,(c)2000\n");
    // -- Определить Базовый адрес ----
    for (BA = 0x100; BA < 0x400; BA += 0x10)
        if ((inportb(BA + 0xA00E) == 'p') && (inportb(BA + 0xA00F) == 55))
            break;
    if (BA == 0x400) {
        printf("Scheme \"p55\" not loaded!");
        return;
    } else
        printf("Base address UNIOxx-5 available:%Xh\n", BA);

    BA = BA + 0xA000;

    CTRL_LINE_set_input(CTRL_LINE_A);
    CTRL_LINE_set_input(CTRL_LINE_B);
    CTRL_LINE_set_input(CTRL_LINE_C_HIGH);
    CTRL_LINE_set_input(CTRL_LINE_C_LOW);
    memset(Set, EVENT_RESET, sizeof(Set));
//      outportb(BA+3, 0x1B); // All channel - input


    init_irq();

    for (;; Pass++) {
        do {// Wait event
            if (bioskey(1) != 0) {
                bioskey(0);
                cleanup();
                return;
            }

        } while (!Event);

        printf("\n");
        for (k = 0; k < sizeof(Count) / sizeof(Count[0]); k++) {
            printf("C%-2d:%-8lu  ", k, Count[k]);
            if (Set[k] == EVENT_SET) {
                LINE_set_bit(LINE_A, 3);
            } else {
                LINE_unset_bit(LINE_A, 3);
            }
            printf("\n");
        }

        Event = 0;
        printf("\n\n");    // Goto xy (SmartLink)
        // Enable interrupt on all input channels
        outportb (BA + 5, 0x7);

    }
}

void interrupt read_event(void) {
    union {
        unsigned long lng;
        unsigned int w[2];
    } dw;
    unsigned long msk;
    char k;

    outportb (BA + 5, 0);    // Disable interrupts

    // ----- Channels 0-23 -------------
    dw.w[0] = inport(BA + 6);    // Read event register EV[15:0]
    dw.w[1] = inport(BA + 8);    // Read event register EV[23:16]

    if (dw.lng) {        // If set event
        outport(BA + 6, dw.w[0]);    // Reset event EV[15:0]
        outport(BA + 8, dw.w[1]);    // Reset event EV[23:16]

        // Search input with event
        for (k = 0, msk = 1; k < 24; k++, msk <<= 1)
            if (msk & dw.lng) {
                Count[k]++; // +1 event count
                Set[k] = ~Set[k];
            }
    }
    Event = 1;
    // End of interrupt
    outportb (0x20, 0x20);
}

void init_irq(void) {
    if (atexit(cleanup) != 0) {
        perror("Exit function can't be registered");
        exit(1);
    }
    last_function = getvect(VECTOR);
    setvect(VECTOR, read_event);
    // Unmask PC-interrupt
    outportb(0x21, inp(0x21) & ~IRQMask);

    // Фронт события и время антидребезга входов 0- 23
    outportb(BA + 4, (Fr << 6) | (Fr << 4) | (Fr << 2) | Td);
    // Сброс событий EV[23:0]
    outport (BA + 6, 0xFFFF);
    outportb(BA + 8, 0xFF);

    outportb(BA + 13, 0x5);        // Линия  IRQ5

    // Разрешить генерацию прерываний от всех входов
    outportb (BA + 5, 0x7);

}

void cleanup(void) {
    // Запретить генерацию прерываний
    outportb(BA + 5, 0);
    outportb(BA + 13, 0);
    outportb(0x21, inp(0x21) | IRQMask);
    setvect(VECTOR, last_function);
}



