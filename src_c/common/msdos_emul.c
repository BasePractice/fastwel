
static int port[1024];
#define PORT_SIZE (sizeof(port)/sizeof(port[0]))

int bioskey(int cmd) {
    return cmd;
}

int __inportb__(int cmd) {
    if (cmd > PORT_SIZE)
        return 0;
    return port[cmd];
}

int __inportw__(int cmd) {
    if (cmd > PORT_SIZE)
        return 0;
    return port[cmd];
}

int __outportb__(int __portid, int __value) {
    if (__portid > PORT_SIZE)
        return 0;
    port[__portid] = __value;
}

int __outportw__(int __portid, int __value) {
    if (__portid > PORT_SIZE)
        return 0;
    port[__portid] = __value;
}

void setvect(int n, void *call) {

}

void *getvect(int n) {
    return 0;
}

