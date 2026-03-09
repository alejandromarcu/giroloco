#include "multitec.hpp"
#include <dos.h>
#include <conio.h>
#include <stdio.h>

char Presionada [127];
unsigned char TPuls;

void interrupt (*viejoProc)(...);

int TeclaPulsada ()
{
	int Aux = TPuls;
     TPuls = 0;
     return Aux;
}

unsigned char LeeTecla ()
{
	unsigned char Aux;
     TPuls =0;
     do {
     	Aux = TeclaPulsada ();
     } while (!Aux);
	while (kbhit ()) getch ();
     return Aux;
}


void interrupt teclado (...)
{
//    asm cli;
    unsigned aux;
    unsigned char p60 = inportb (0x60);
    Presionada [p60 & 127] = (p60 & 128) != 128;
    if (p60 < 128) TPuls = p60;
//     if (kbhit ()) getch ();
/*    asm {
        		mov ah, 01
               int 0x16
			jnz s
               mov ah, 00
               int 0x16
    } */
//    asm sti;
    asm pushf;
    //anular Pause Y Ctrl
    if ((p60 ==225)|| (p60 ==29)) outportb (0x60,0);
    viejoProc ();

}

void InstalarMT ()
{
      viejoProc = getvect(9);
      setvect (9, teclado);
}

void DesInstalarMT ()
{
     setvect (9, viejoProc);
}


