#ifndef FASTWEL_DOS_H
#define FASTWEL_DOS_H

#if defined(__cplusplus)
extern "C" {
#endif

#define interrupt
#define far

#define inportb(cmd) __inportb__((cmd))
#define inport(cmd) __inportw__((cmd))
#define inp(cmd) __inportw__((cmd))
#define outportb(port, value) __outportb__((port), (value))
#define outportw(port, value) __outportw__((port), (value))
#define outport(port, value) __outportw__((port), (value))

int __inportb__(int cmd);

int __inportw__(int cmd);

int __outportb__(int __portid, int __value);

int __outportw__(int __portid, int __value);

void setvect(int n, void *call);

void *getvect(int n);

#if defined(__cplusplus)
}
#endif

#endif //FASTWEL_DOS_H
