#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <alloc.h>
#include <mem.h>

#define TAM_BUFFER 8192

#define WriteBitPlane(x) (outport (0x3C4,(1<<(x+8))+2))
#define ReadBitPlane(x) (outport (0x3CE,x * 256+4))

unsigned char BUFFER [TAM_BUFFER];

typedef struct IBP_Encab {
               	int ancho, alto;
                    char p2 [124];
			};


void GrabarIBP (int x1, int y1, int x2, int y2, char *narch)
{
	FILE *arch;
	int cx, cy, cont = 1, color, colant, escribir, BytesLinea, OfsInic,bp;
	unsigned char aux;
	arch = fopen (narch, "wb");
	setbuf (arch, BUFFER);
     IBP_Encab encab;
     x1 = (x1 / 8) * 8;
     x2 = (x2 / 8) * 8;
     encab.ancho = x2 - x1;
     encab.alto  = y2 - y1;
	fwrite (&encab, sizeof (encab),1,arch);
	cont   = 0;
     BytesLinea =encab.ancho / 8;
     OfsInic = x1 / 8 + y1 * (640 / 8);
     for (bp = 0; bp < 4; bp++)
     {
          ReadBitPlane (bp);
     	for (cy = 0; cy < encab.alto; cy++)
               for (cx = 0; cx < BytesLinea; cx++)
               {
               	aux = peekb (0xA000,OfsInic + cy * 80 + cx);
				fwrite (&aux,1,1,arch);
               }

     }

	fwrite (&aux,1,1,arch);
	fclose (arch);
}

void CargarIBP (int x1, int y1, int pagina, char *narch)
{
	FILE *arch;
	int cy, cont = 0, color, BytesLinea,bp;
	unsigned int TamBP,segmento, OfsInic;
     char *ImBP, *punt;
     if (pagina == 0) segmento = 0xA000;
     else segmento = 0xA800;
	arch = fopen (narch, "rb");
	setbuf (arch, BUFFER);

     IBP_Encab encab;
	fread (&encab, sizeof (encab),1,arch);
     BytesLinea =encab.ancho / 8;
     OfsInic = x1 / 8 + y1 * 80;
     TamBP = BytesLinea * encab.alto;
     ImBP = (char far *) farmalloc (TamBP);
if (ImBP ==NULL) exit (0);
     for (bp = 0; bp < 4; bp++)
     {
     	fread (ImBP,TamBP,1,arch);
     	punt  = ImBP;
          WriteBitPlane (bp);
     	for (cy = 0; cy < encab.alto; cy++)
          {
               memcpy (MK_FP (segmento,cy*80+OfsInic) ,punt,BytesLinea);
               punt+=BytesLinea;
          }
     }
     bar (0,0,0,0); // LA PRIMERA VEZ QUE SE USA ANDA MAL!!!!
	farfree (ImBP);
	fclose (arch);
}

