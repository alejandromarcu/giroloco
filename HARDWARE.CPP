/*
  Control del nivel hardware del PC:
  Controlador de interrupciones, Controlador de DMA y Timers.
  Por Luis Crespo, FidoNet 2:343/108.21, Internet d8089110@est.fib.upc.es
*/

#include <dos.h>

/*--------------- Controlador de interrupciones ---------------*/

void EOI()
/* Indica un fin de interrupci¢n al controlador maestro */
{
  outportb(0x20,0x20);
}

void EOISlave()
/* Indica un fin de interrupci¢n al controlador esclavo */
{
  outportb(0xA0,0x20);
}

void HabilitaIRQ(char Num)
/* Programa el PIC (o los PICs) para habilitar un nivel de interrupci¢n */
{
  unsigned char Mask;

  if (Num<=7)
  {
    Mask = !(1<<Num); /* Bit a 0 --> interrupci¢n habilitada */
    asm CLI
    outportb(0x21,inportb(0x21) & Mask);
    asm STI
  }
  else {
    /* Si la IRQ es>7, hay que programar el controlador esclavo */
    Num = Num-8;
    Mask = !(1<<Num);
    asm CLI
    outportb(0xA1,inportb(0xA1) & Mask);
    asm STI
    HabilitaIRQ(2);
    /* Y tambi‚n hay que habilitar la entrada en cascada en el master
    (que probablemente ya estar  habilitada) */
  }
}

void DeshabilitaIRQ(char Num)
/* Programa el PIC (o los PICs) para deshabilitar un nivel de interrupci¢n */
{
  unsigned char Mask;

  if (Num<=7)
  {
    Mask = 1<<Num; /* Bit a 1 --> interrupci¢n inhabilitada */
    asm CLI
    outportb(0x21,inportb(0x21) | Mask);
    asm STI
  }
  else {
    /* Si la IRQ es>7, hay que programar el controlador esclavo */
    Num = Num-8;
    Mask = 1<<Num;
    asm CLI
    outportb(0xA1,inportb(0xA1) | Mask);
    asm STI
  }
}



/*--------------- Controlador de DMA ---------------*/

void StopDMA(char DMANum)
/* Deshabilita un canal de DMA */
{
  if (DMANum<4) outportb(0x0A,DMANum|4);
  else outportb(0xD4,(DMANum-4)|4);
}

void ProgKDMA(char DMANum, void *Buffer, unsigned Tamanyo, char AutoInit, char DesdeMem)
/*Programa el controlador de DMA (de 8 bits) para lanzar un buffer de
 memoria a perif‚rico o para leer de perif‚rico a memoria */
{
  int DMAPageReg[8] = {0x87,0x83,0x81,0x82,0x00,0x8B,0x89,0x8A};
  unsigned char Pagina;
  unsigned Desp;
  unsigned long DirFisica;
  unsigned char DMAMode;

  DirFisica = ((unsigned long)FP_SEG(Buffer) << 4) + (unsigned long)FP_OFF(Buffer);
  Pagina = DirFisica >> 16;
  Desp = DirFisica & 0xFFFF;
  asm CLI
  StopDMA(DMANum);  /* Deshabilito el canal provisionalmente, para programarlo */
  outportb(0x0C,0); /* Inicializo el flip-flop para empezar por byte bajo */
  DMAMode = 0x40 | DMANum;
  if (AutoInit) DMAMode=DMAMode|0x10;
  if (DesdeMem) DMAMode=DMAMode|8; else DMAMode=DMAMode|4;
  outportb(0x0B,DMAMode);
  outportb(DMAPageReg[DMANum],Pagina); /* Pagina del buffer en memoria f¡sica */
  outportb(2*DMANum,Desp & 0xFF); /* Desplazamiento del buffer en memoria f¡sica */
  outportb(2*DMANum,Desp>>8);
  Tamanyo--;
  outportb(2*DMANum+1,Tamanyo & 0xFF); /* Tama¤o del buffer */
  outportb(2*DMANum+1,Tamanyo>>8);
  outportb(0x0A,DMANum);    /* Una vez programado, habilito el canal */
  asm STI
}



/*--------------- Timer 0 ---------------*/

void ProgTimer0(unsigned Divisor)
/* Programa el Timer 0 para que cuente desde Divisor hasta 0 */
{
  asm CLI                         /* Inhabilitamos interrupciones: secci¢n cr¡tica */
  outportb(0x43,0x36);            /* 00111010: Timer 0, acceso secuencial y modo cont¡nuo */
  outportb(0x40,Divisor & 0xFF);  /* Byte bajo del contador */
  outportb(0x40,Divisor>>8);      /* Byte alto del contador */
  asm STI                         /* Fin de secci¢n cr¡tica */
}

void SetTimerFrec(unsigned *Frecuencia)
/* Programa el Timer 0 para que genere "Frecuencia" interrupciones por segundo */
{
  unsigned Divisor;

  Divisor = 1193180 / *Frecuencia;
  *Frecuencia = 1193180 / Divisor;
  ProgTimer0(Divisor);
}
