// Пример программирования UNIO24 для варианта p55.
// Подсчитываются события по входам (фронт события задается в параметрах).


#include <bios.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

unsigned char Event(void);

unsigned int  BA;		// Базовый адрес UNIO (ch.0 -23)
unsigned long Count[24];
unsigned long Pass;
unsigned int   Td=3,Fr=1;


      void main(int arg,char **av)
  {

      unsigned int   i,k;


      printf("\x1b[2J");	// Clear Screen (SmartLink)

      if (*av[1]=='/'|| *av[1]=='?'){
	    printf (
	    "ct_p55 Fr Td\n"
	    "         |  |\n"
	    "         |  +--- Код времени антидребезга (коды 0-3)\n"
	    "         +------ Код фронт события        (коды 0-3)\n"
	    );
	    return;
      }

      if (arg>1) {
	sscanf(av[1],"%d",&Fr);
	if (Fr >3) {printf("Код фронта д.б. от 0 до 3"); return;}
      }
      if (arg>2) {
	sscanf(av[2],"%d",&Td);
	if (Td >3) {printf("Код частоты д.б. от 0 до 3"); return;}
      }

  printf ("UNIOxx-5 Код Схемы:\"p55\"  Fastwel,(c)2000\n");
  // -- Определить Базовый адрес ----
  for (BA=0x100;BA<0x400;BA+=0x10)
    if ((inportb(BA+0xA00E)=='p')&&(inportb(BA+0xA00F)==55)) break;
  if (BA==0x400) {printf("Код Схемы\"p55\"не загружен !");return;}
  else 	    printf("Базовый адрес модуля UNIOxx-5 определен:%Xh\n",BA);

  BA=BA+0xA000;

//      outportb(BA+3, 0x1B); // Все каналы - входы

      // Фронт события и время антидребезга входов 0- 23
      outportb(BA+4,(Fr<<6)|(Fr<<4)|(Fr<<2)|Td);
      // Сброс событий EV[23:0]
      outport (BA+6,0xFFFF);
      outportb(BA+8,0xFF);

      for (;;Pass++){

	if (bioskey(1)!=0) {
	  bioskey(0);
	  printf("\x1b[2J");	// Clear Screen (SmartLink)
	  return;
	}

	if(Event()){	//Если произошло событие - индицируем счетчики

	  printf("\n");
	  for(k=0;k<6;k++){
	    for(i=0;i<4;i++)  printf("C%-2d:%-8lu  ",k*4+i, Count[k*4+i]);
	    printf("\n");
	  }
	}
	printf("\x1B\x3D\x21\x20");	// Goto xy (SmartLink)

      }
  }

    unsigned char Event(void)
 {

    union {unsigned long lng; unsigned int w[2];} dw;
    unsigned long msk;
    char ev=0,k;


    // ----- Каналы 0-23 -------------
    dw.w[0] =inport(BA+6);    // Чтение регистра событий EV[15:0]
    dw.w[1] =0xFF&inport(BA+8);// Чтение регистра событий EV[23:16]

    if(dw.lng) {		// Если событие произошло

      ev=1;
      outport(BA+6,dw.w[0]);	// Сброс событий EV[15:0]
      outport(BA+8,dw.w[1]);	// Сброс событий EV[23:16]

      // Определяем входы, где произошло событие и сбрасываем его
      for(k=0,msk=1;k<24;k++,msk<<=1)
	  if (msk&dw.lng) Count[k]++; // +1 в счетчике событий

    }
    return ev;

 }




