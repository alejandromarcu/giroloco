/*
  Soporte de SB, SB 2.0, SB Pro y SB 16.
  Por Luis Crespo, FidoNet 2:343/108.21, Internet d8089110@est.fib.upc.es
*/

#include <dos.h>
#include "hardware.hpp"

#define FALSE 0
#define TRUE  1

char DSPVerMajor,
     DSPVerMinor,           /* Versi¢n del DSP */
     SBIRQ,                 /* IRQ de la SB    */
     SB8DMA,                /* DMA de 8 bits   */
     SB16DMA,               /* DMA de 16 bits  */
     TimeConst,             /* Constante de tiempo del DSP */
     SBVector;              /* N£mero de vector de interrupci¢n de la SB */
void interrupt (*OldISR)(...); /* Puntero a la antigua ISR */
char SBReadError,           /* Error de lectura en el DSP */
     SBWriteError;          /* Error de escritura en el DSP */

/* Puertos de la SB */
unsigned SBBase    = 0x210, /* Puerto base                         */
         SBMixAddr = 0x214, /* Registro de direcciones del mixer   */
         SBMixData = 0x215, /* Registro de datos del mixer         */
         SBReset   = 0x216, /* Puerto de reset                     */
         SBReadp   = 0x21A, /* Puerto de lectura del DSP           */
         SBWritep  = 0x21C, /* Puerto de escritura del DSP         */
         SBRStat   = 0x21E; /* Puerto de estado de lectura del DSP */

/* Comandos del DSP */
unsigned char cmDirectDAC  = 0x10,  /* Modo directo, 8 bits */
              cmDirectADC  = 0x20,
              cmSingleDAC  = 0x14,  /* DMA de ciclo £nico, 8 bits */
              cmSingleADC  = 0x24,
              cmAutoDAC    = 0x1C,  /* DMA auto inicializado, 8 bits */
              cmAutoADC    = 0x2C,
              cmSetTimeCt  = 0x40,  /* Constante de tiempo */
              cmSetSize    = 0x48,  /* Tama¤o del buffer */
              cmHiSpdDAC   = 0x90,  /* DMA auto inicializado, modo high-speed, 8 bits */
              cmHiSpdADC   = 0x98,
              cmSB16DAC16  = 0xB6,  /* SB 16, DMA de 16 bits */
              cmSB16ADC16  = 0xBE,
              cmSB16DAC8   = 0xC6,  /* SB 16, DMA de 8 bits */
              cmSB16ADC8   = 0xCE,
              cmStopDMA    = 0xD0,  /* Para el DMA */
              cmSpeakerOn  = 0xD1,  /* Control de la salida del DAC */
              cmSpeakerOff = 0xD3,
              cmGetDSPVer  = 0xE1,  /* Consulta de versi¢n del DSP */

/* Modos de DMA de la SB 16 */
              mdUnsigned   = 0x00,
              mdSigned     = 0x10,
              mdMono       = 0x00,
              mdStereo     = 0x20,

              Ready        = 0xAA;  /* Indicaci¢n de reset correcto */


unsigned RWCount = 0xFFFF;      /* N£mero m ximo de consultas a los puertos de estado */


static unsigned BufSize;
unsigned char Ack;
static void (*NextBuf)(...);



/*-------------------- Operaciones b sicas de I/O -------------------------*/

unsigned char SBRead()
{
  unsigned Ct;

  SBReadError = TRUE;
  Ct = RWCount;
  while (Ct>0)
  {
    if ((inportb(SBRStat) & 0x80)!=0)
    /* El Turbo C++ 1.01 compila la l¡nea de arriba sin ning£n warning.
    El Borland C++ 3.1, en cambio, da un FATAL ERROR y aborta la compilaci¢n
    de inmediato. Para que luego digan que el C es portable, cuando ni
    siquiera lo es entre dos versiones del mismo compilador del mismo
    fabricante en la misma plataforma!!! (¨Podr¡a alguien explicarme c¢mo
    usar inportb() con el BC++ 3.1?) */
    {
      /* Cuando el bit 7 est  a 1, se puede leer */
      SBReadError = FALSE;
      return(inportb(SBReadp));
    }
    Ct--;
  }
  return(0); /* S¢lo para evitar el warning */
}

void SBWrite(unsigned char b)
{
  unsigned Ct;

  SBWriteError = TRUE;
  Ct = RWCount;
  while (Ct>0 && SBWriteError)
  {
    if ((inportb(SBWritep) & 0x80)==0)
    {
      /* Cuando el bit 7 est  a 0, se puede leer */
      SBWriteError = FALSE;
      outportb(SBWritep,b);
    }
    Ct--;
  }
}

void SBSendByte(unsigned char b)
{
  SBWrite(cmDirectDAC);
  SBWrite(b);
}

unsigned char SBReadMixer(unsigned char Addr)
{
  outportb(SBMixAddr,Addr);
  return(inportb(SBMixData));
}

void SBWriteMixer(unsigned char Addr, unsigned char b)
{
  outportb(SBMixAddr,Addr);
  outportb(SBMixData,b);
}



/*-------------------- Detecci¢n e inicializaci¢n -------------------------*/

char ReadSB16IRQ()
{
  unsigned char IRQSetup;

  IRQSetup = SBReadMixer(0x80);    /* Interrupt setup register */
  if ((IRQSetup & 1)==1) return(2);
  else if ((IRQSetup & 2)==2) return(5);
  else if ((IRQSetup & 4)==4) return(7);
  else if ((IRQSetup & 8)==8) return(10);
  else return(0);
}

void ReadSB16DMA(char *DMA8, char *DMA16)
{
  unsigned char DMASetup;

  *DMA8 = 0;
  *DMA16 = 0;
  DMASetup = SBReadMixer(0x81);    /* DMA Setup register */
  if ((DMASetup & 1)==1) *DMA8=0;
  else if ((DMASetup & 2)==2) *DMA8=1;
  else if ((DMASetup & 8)==8) *DMA8=3;
  if ((DMASetup & 0x20)==0x20) *DMA16=5;
  else if ((DMASetup & 0x40)==0x40) *DMA16=6;
  else if ((DMASetup & 0x80)==0x80) *DMA16=7;
}

void interrupt PruebaIRQ2(...)
{
  SBIRQ = 2;
  EOI();
}

void interrupt PruebaIRQ3(...)
{
  SBIRQ = 3;
  EOI();
}

void interrupt PruebaIRQ5(...)
{
  SBIRQ = 5;
  EOI();
}

void interrupt PruebaIRQ7(...)
{
  SBIRQ = 7;
  EOI();
}

void interrupt PruebaIRQ10(...)
{
  SBIRQ = 10;
  EOISlave();
  EOI();
}


void DeterminaIRQyDMA()
{
  char TablaDMA[3] = {0,1,3};
  unsigned char IMR,IMRSlave;
  void interrupt (*Old2)(...);
  void interrupt (*Old3)(...);
  void interrupt (*Old5)(...);
  void interrupt (*Old7)(...);
  void interrupt (*Old10)(...);
  unsigned char Algo,DMACt;

  if (DSPVerMajor>=4)
  { /* La detecci¢n en la SB 16 es mucho m s sencilla */
    SBIRQ = ReadSB16IRQ();
    ReadSB16DMA(&SB8DMA,&SB16DMA);
  }
  else {
    /* Detecci¢n por fuerza bruta: arriesgada pero efectiva */
    SB16DMA = 0;
    SBIRQ = 0;
    Old2 = getvect(8+2); setvect(8+2,PruebaIRQ2);
    Old3 = getvect(8+3); setvect(8+3,PruebaIRQ3);
    Old5 = getvect(8+5); setvect(8+5,PruebaIRQ5);
    Old7 = getvect(8+7); setvect(8+7,PruebaIRQ7);
    Old10 = getvect(104+10); setvect(104+10,PruebaIRQ10);
    asm CLI
    IMR = inportb(0x21); /* Guarda m scaras de interrupciones */
    IMRSlave = inportb(0xA1);
    outportb(0x21,IMR & 0x53); /* Habilita todas posible IRQ's de la SB */
    outportb(0xA1,IMRSlave & 0xFB);
    asm STI
    Algo = 128;
    DMACt = 0;
    SBWrite(cmSetTimeCt);
    SBWrite(206); /* 20 KHz */
    SBWrite(cmSingleDAC);
    SBWrite(0); SBWrite(0); /* 1 byte */
    do
    {
      SB8DMA = TablaDMA[DMACt];
      ProgKDMA(SB8DMA,&Algo,1,FALSE,TRUE);
      delay(1);             /* Espera a que termine la mini-transferencia */
      StopDMA(SB8DMA);
      DMACt++;
    } while ((SBIRQ==0) && (DMACt<=3));
    Algo = SBRead(); /* Ack a la SB */
    asm CLI
    outportb(0x21,IMR);
    outportb(0xA1,IMRSlave);
    asm STI
    setvect(8+2,Old2);
    setvect(8+3,Old3);
    setvect(8+5,Old5);
    setvect(8+7,Old7);
    setvect(104+10,Old10);
  }
}

char SBDetect()
{
  unsigned char Status;
return (FALSE);
  EOISlave(); /* Se limpian posibles interrupciones pendientes */
  EOI();
  SBReadError = FALSE;
  SBWriteError = FALSE;
  do
  {
    outportb(SBReset,1);
    delay(1);                 /* Espera un tiempo prudencial para el reset */
    outportb(SBReset,0);
    Status = SBRead();
    if (SBReadError) Status = 0;
    if (Status!=Ready)
    {
      SBBase = SBBase+0x10;        /* Si no detecta... */
      SBMixAddr = SBMixAddr+0x10;  /* ...prueba con otro puerto base */
      SBMixData = SBMixData+0x10;
      SBReset = SBReset+0x10;
      SBReadp = SBReadp+0x10;
      SBWritep = SBWritep+0x10;
      SBRStat = SBRStat+0x10;
    }
  } while ((Status!=Ready) && (SBBase!=0x290));
  if (SBBase!=0x290)
  {
    SBWrite(cmGetDSPVer);    /* Averigua la versi¢n del DSP */
    DSPVerMajor = SBRead();
    DSPVerMinor = SBRead();
    SBWrite(cmSpeakerOff);
    DeterminaIRQyDMA();
    return(TRUE);
  }
  else return(FALSE);
}


/*------------------ Generaci¢n de sonido via DMA -----------------------*/

void interrupt DMAISR(...)
{
  if (DSPVerMajor==1)
  {
    SBWrite(cmSingleDAC);
    SBWrite(BufSize & 0xFF);
    SBWrite(BufSize>>8);
  }
  Ack = inportb(SBRStat);
  if (SBIRQ>7) EOISlave();
  EOI();
  asm STI
  NextBuf();
}

void ProgDSP()
{
  switch(DSPVerMajor)
  {
    case 1:
      SBWrite(cmSingleDAC);
      SBWrite(BufSize & 0xFF);
      SBWrite(BufSize>>8);
      break;
    case 2:
      SBWrite(cmSetSize);
      SBWrite(BufSize & 0xFF);
      SBWrite(BufSize>>8);
      if (DSPVerMinor==0) SBWrite(cmAutoDAC);
      else SBWrite(cmHiSpdDAC);
      break;
    case 3 :
      SBWrite(cmSetSize);
      SBWrite(BufSize & 0xFF);
      SBWrite(BufSize>>8);
      SBWrite(cmHiSpdDAC);
      break;
    default :   /* SB 16 o superior */
      SBWrite(cmSB16DAC8);
      SBWrite(mdMono+mdUnsigned);
      SBWrite(BufSize & 0xFF);
      SBWrite(BufSize>>8);
  }
}

void SBStopBuf()
{
  SBWrite(cmStopDMA);
  DeshabilitaIRQ(SBIRQ);
  setvect(SBVector,OldISR);
  StopDMA(SB8DMA);
  SBWrite(cmSpeakerOff);    /* Deshabilita la salida digital */
}         

void SBPlayBuf(void *pBuffer, unsigned BufferSize, unsigned *Frec, void (*BufProc)(...))
{
  BufSize = BufferSize-1;
  NextBuf = BufProc;
  TimeConst = 256-(1000000 / *Frec);
  *Frec = 1000000/(256-TimeConst);
  SBWrite(cmSetTimeCt);
  SBWrite(TimeConst);
  if (SBIRQ<8) SBVector=8+SBIRQ; else SBVector=104+SBIRQ;
  OldISR = getvect(SBVector);
  setvect(SBVector,DMAISR);
  HabilitaIRQ(SBIRQ);
  ProgKDMA(SB8DMA,pBuffer,BufferSize*2,TRUE,TRUE);
  SBWrite(cmSpeakerOn);    /* Habilita la salida digital */
  ProgDSP();
}


