#include <stdlib.h>
#include <stdio.h>
#include <DOS.H>
#include <BIOS.H>

#define    IRQ            5
#define    IRQMask        0x20
#define    VECTOR        (IRQ + 8)

void interrupt ReadEvent(void);

void InitIRQ(void);

void CleanUp(void);

void interrupt ( *oldhandler)();


unsigned int Event;
unsigned int BA;        // Базовый адрес UNIO (ch.0 -23)
unsigned long Count[24];
unsigned long Pass;
unsigned int Td = 3, Fr = 1;


void main(int arg, char **av) {
    unsigned int i, k;

    if ( arg > 1 && (*av[1] == '/' || *av[1] == '?')) {
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

//      outportb(BA+3, 0x1B); // All channel - input


    InitIRQ();

    for (;; Pass++) {
        do {// Wait event
            if (bioskey(1) != 0) {
                bioskey(0);
                CleanUp();
                return;
            }

        } while (!Event);

        printf("\n");
        for (k = 0; k < 6; k++) {
            for (i = 0; i < 4; i++)
                printf("C%-2d:%-8lu  ", k * 4 + i, Count[k * 4 + i]);
            printf("\n");
        }

        Event = 0;
        printf("\n\n");    // Goto xy (SmartLink)
        // Enable interrupt on all input channels
        outportb (BA + 5, 0x7);

    }
}

void interrupt ReadEvent(void) {
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
            if (msk & dw.lng)
                Count[k]++; // +1 event count
    }
    Event = 1;
    // End of interrupt
    outportb (0x20, 0x20);
}

void InitIRQ(void) {
    if (atexit(CleanUp) != 0) {
        perror("Exit function can't be registered");
        exit(1);
    }
    oldhandler = getvect(VECTOR);
    setvect(VECTOR, ReadEvent);
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

void CleanUp(void) {
    // Запретить генерацию прерываний
    outportb(BA + 5, 0);
    outportb(BA + 13, 0);
    outportb(0x21, inp(0x21) | IRQMask);
    setvect(VECTOR, oldhandler);
}



