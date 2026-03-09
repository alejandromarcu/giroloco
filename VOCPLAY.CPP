#include <stdio.h>
#include <alloc.h>
#include <dos.h>
#include <mem.h>

#include "sb.hpp"
#include "speaker.hpp"

#define FALSE 0
#define TRUE  1


unsigned Tamanyo;       /* Tama¤o del VOC en memoria */
char     *pVoc,         /* Puntero al VOC */
	    *pBufDMA,      /* Puntero al buffer */
	    *pBufOrig,     /* Idem original, para repeticiones */
	    *pBufTotDMA;   /* Puntero al buffer total */
unsigned VocPos;        /* Posici¢n actual del VOC */
char     FinVoz,
	    PrimeraMitad,
	    HaySB;
unsigned BufSize = 512; /* Tama¤o del buffer de DMA */



void ReservaBufferDMA(unsigned Size)
/* Reserva un buffer para transferencia de DMA, controlando que no haya
solapamiento de p ginas. */
{
  unsigned char PagIni,PagDest;
  unsigned long DirFisIni,DirFisDest;
  /* Direcci¢n f¡sica de inicio y final del buffer de DMA, respectivamente */

  pBufTotDMA =(char *) malloc(Size*2);
  pBufOrig = pBufTotDMA;
  DirFisIni = (unsigned long)FP_SEG(pBufOrig) << 4 + FP_OFF(pBufOrig);
  DirFisDest = DirFisIni+Size;
  PagIni = DirFisIni >> 16;
  PagDest = DirFisDest >> 16;
  if (PagIni!=PagDest) pBufOrig = pBufOrig+Size;
  /* Si hay solapamiento en la primera mitad, se toma la segunda */
}

void NextBuffer()
/* Esta rutina es llamada cada vez que se recorre medio buffer, y se
encarga de actualizar el buffer de DMA */
{
  memcpy(pBufDMA,pVoc+VocPos,BufSize);
  VocPos = VocPos+BufSize;
  if (PrimeraMitad) pBufDMA=pBufDMA+BufSize; else pBufDMA=pBufDMA-BufSize;
  PrimeraMitad=!PrimeraMitad;
  if (VocPos>Tamanyo)
  {
	FinVoz=TRUE;
	SpkStopBuf();
   }
}

void InitVocPlay()
{
  //HaySB =  SBDetect();
  FinVoz = TRUE;
  ReservaBufferDMA(BufSize*2);
}

void DoneVocPlay()
{
	 free(pBufTotDMA);
}

void PlaySample(char* pSample, int Len, unsigned Freq)
{
  if (!FinVoz) SpkStopBuf ();
  FinVoz = FALSE;
  VocPos = 0;
  PrimeraMitad = TRUE;
  pBufDMA = pBufOrig;
  memset(pBufDMA,128,BufSize*2);
  pVoc=pSample;
  Tamanyo=Len;
  SpkPlayBuf(pBufDMA,BufSize,&Freq,NextBuffer);
}
