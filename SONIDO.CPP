#include <dos.h>
#include <stdio.h>
#include <graphics.h>
#include <process.h>
#include "vocplay.hpp"
#include "sonido.hpp"
#include "menu.hpp"

extern SConfig config;

unsigned      DSP_RESET,
     	    DSP_READ_DATA,
    		    DSP_WRITE_DATA,
  		    DSP_WRITE_STATUS,
    		    DSP_DATA_AVAIL;

WAV Sonidos [CANT_SONIDOS];

void SpeakerOn () { WriteDAC (0xD1); };
void SpeakerOff () { WriteDAC (0xD3); };
void DMAStop () { WriteDAC (0xD4); };
void DMAContinue () { WriteDAC (0xD0); };

void Sonido (int nson) { if (config.SonAct) Sonidos [nson].Tocar (); }

void WriteDSP (int value)
{
	while (inportb (DSP_WRITE_STATUS) & 0x80);
     outportb (DSP_WRITE_DATA, value);
}

void WriteDAC (int level)
{
	WriteDSP (0x10);
  	WriteDSP (level);
}


void CargarSonidos ()
{
     int cont;
     char narch [14];
	for (cont=0; cont < CANT_SONIDOS; cont++)
     {
          sprintf (narch,"SPC%d.SON",cont);
		Sonidos [cont].Cargar (narch);
     }
}


void Playback (void far *punt, unsigned tam, unsigned frec)
{
	PlaySample ((char *) punt,tam,frec);
}


void WAV::Cargar (char *narch)
{
     FILE *arch;
     unsigned int cab [20];
     arch = fopen (narch,"rb");
     if (!arch)
     {
     	closegraph ();
          printf ("Error leyendo el archivo %s.\n\n", narch);
          exit (1);
     }

	fseek (arch, 0L, SEEK_SET);
     fread (cab, 2, 20, arch);
     tam = cab [2] - 32;
     frec = cab [12];

     sonido = farmalloc (tam);
     if (sonido) fread (sonido, 1, tam, arch);
     fclose (arch);
}


