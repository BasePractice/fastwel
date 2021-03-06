// �ਬ�� �ணࠬ��஢���� UNIO ��� ��ਠ�� p55.
// ������뢠���� ᮡ��� �� �室�� 0-23 (�஭� ᮡ��� ��������
// � ��ࠬ����).
// �ᯮ������ ���뢠��� IRQ5.


#include <bios.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

#define	IRQ		5
#define	IRQMask		0x20
#define	VECTOR		IRQ+8

void interrupt ReadEvent(void);
void 	       InitIRQ(void);
void 	       CleanUp(void);
void interrupt( *oldhandler)();



unsigned int  Event;
unsigned int  BA;		// ������ ���� UNIO (ch.0 -23)
unsigned long Count[24];
unsigned long Pass;
unsigned int   Td=3,Fr=1;


      void main(int arg,char **av)
  {

      unsigned int   i,k;


      printf("\x1b[2J");	// Clear Screen (SmartLink)

      if (*av[1]=='/'|| *av[1]=='?'){
	    printf (
	    "ct_p55i Fr Td\n"
	    "         |  |\n"
	    "         |  +--- ��� �६��� ��⨤ॡ���� (���� 0-3)\n"
	    "         +------ ��� �஭� ᮡ���        (���� 0-3)\n"
	    );
	    return;
      }

      if (arg>1) {
	sscanf(av[1],"%d",&Fr);
	if (Fr >3) {printf("��� �஭� �.�. �� 0 �� 3"); return;}
      }
      if (arg>2) {
	sscanf(av[2],"%d",&Td);
	if (Td >3) {printf("��� ����� �.�. �� 0 �� 3"); return;}
      }

      printf ("UNIOxx-5 ��� �奬�:\"p55\"  Fastwel,(c)2000\n");
      // -- ��।����� ������ ���� ----
      for (BA=0x100;BA<0x400;BA+=0x10)
	if ((inportb(BA+0xA00E)=='p')&&(inportb(BA+0xA00F)==55)) break;
      if (BA==0x400) {printf("��� �奬�\"p55\"�� ����㦥� !");return;}
      else 	    printf("������ ���� ����� UNIOxx-5 ��।����:%Xh\n",BA);

      BA=BA+0xA000;

//      outportb(BA+3, 0x1B); // �� ������ - �室�


      InitIRQ();

      for (;;Pass++){

	do{				// ���� ᮡ���

	   if (bioskey(1)!=0) {
	     bioskey(0);
	     CleanUp();
	     return;
	   }

	} while(!Event);

	printf("\n");
	for(k=0;k<6;k++){
	  for(i=0;i<4;i++)  printf("C%-2d:%-8lu  ",k*4+i, Count[k*4+i]);
	  printf("\n");
	}

	Event=0;
	printf("\x1B\x3D\x21\x20");	// Goto xy (SmartLink)
	// ������� ������� ���뢠��� �� ��� �室��
	outportb (BA+5,0x7);

      }
  }

   void interrupt ReadEvent(void)

 {

  union {unsigned long lng; unsigned int w[2];} dw;
  unsigned long msk;
  char k;

    outportb (BA+5,0);	// ����� ���뢠���

    // ----- ������ 0-23 -------------
    dw.w[0] =inport(BA+6);    // �⥭�� ॣ���� ᮡ�⨩ EV[15:0]
    dw.w[1] =inport(BA+8);    // �⥭�� ॣ���� ᮡ�⨩ EV[23:16]

    if(dw.lng) {		// �᫨ ᮡ�⨥ �ந��諮

      outport(BA+6,dw.w[0]);	// ���� ᮡ�⨩ EV[15:0]
      outport(BA+8,dw.w[1]);	// ���� ᮡ�⨩ EV[23:16]

      // ��।��塞 �室�, ��� �ந��諮 ᮡ�⨥ � ���뢠�� ���
      for(k=0,msk=1;k<24;k++,msk<<=1)
	  if (msk&dw.lng) Count[k]++; // +1 � ���稪� ᮡ�⨩
    }
    Event=1;

    // End of interrupt
    outportb (0x20,0x20);

 }

   void InitIRQ(void)
 {

   if(atexit(CleanUp) != 0) {
	perror("Exit function can't be registered"); exit(1);
   }
   oldhandler = getvect(VECTOR);
   setvect(VECTOR, ReadEvent);
   // Unmask PC-interrupt
   outportb(0x21,inp(0x21) & ~IRQMask);

   // �஭� ᮡ��� � �६� ��⨤ॡ���� �室�� 0- 23
   outportb(BA+4,(Fr<<6)|(Fr<<4)|(Fr<<2)|Td);
   // ���� ᮡ�⨩ EV[23:0]
   outport (BA+6,0xFFFF);
   outportb(BA+8,0xFF);

   outportb(BA+13,0x5);	    // �����  IRQ5

   // ������� ������� ���뢠��� �� ��� �室��
   outportb (BA+5,0x7);

 }

   void CleanUp(void)
 {
   // ������� ������� ���뢠���
   outportb(BA+5 ,0);
   outportb(BA+13 ,0);
   outportb(0x21,inp(0x21) | IRQMask);
   setvect(VECTOR, oldhandler);
 }



