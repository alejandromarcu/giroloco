#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

#define TAM_BUFFER 8192

unsigned char BUFFER [TAM_BUFFER];

typedef struct IMG_Encab {
               	int ancho, alto;
                    char p2 [124];
			};

void GrabarIMG (int x1, int y1, int x2, int y2, char *narch)
{
	FILE *arch;
	int cx, cy, cont = 1, color, colant, escribir;
	unsigned char aux;
	arch = fopen (narch, "wb");
	setbuf (arch, BUFFER);
     IMG_Encab encab;
     encab.ancho = x2 - x1 + 1;
     encab.alto  = y2 - y1 + 1;
	fwrite (&encab, sizeof (encab),1,arch);
	cont   = 0;
	colant = getpixel (x1,y1);
	for (cy = y1; cy <= y2; cy++)
		for (cx = x1; cx <= x2; cx++)
		{
			color = getpixel (cx, cy);
/*			if (color == colant)
				if (++cont == 16) escribir = 1;
				else escribir = 0;
			else*/ escribir = 1;
			if (escribir)
			{
cont = 1;
                    aux = ((cont-1) << 4) + colant;
				fwrite (&aux,1,1,arch);
				cont = cont == 16 ? 0 : 1;
			}
			colant = color;
		}
	aux = ((cont-1) << 4) + colant;
	fwrite (&aux,1,1,arch);
	fclose (arch);
}

void CargarIMG (int x1, int y1, char *narch)
{
	FILE *arch;
	int cx, cy, cont = 0, color, escribir, x2, y2;
	unsigned char aux;
	arch = fopen (narch, "rb");
	setbuf (arch, BUFFER);

     IMG_Encab encab;
	fread (&encab, sizeof (encab),1,arch);
     x2 = encab.ancho + x1 - 1;
     y2 = encab.alto +  y1 - 1;

	for (cy = y1; cy <= y2; cy++)
		for (cx = x1; cx <= x2; cx++)
		{
			if (!cont)
			{
				fread (&aux,1,1,arch);
				cont = aux / 16 + 1;
				color = aux % 16;
			}
			putpixel (cx,cy, color);
			cont--;
		}

	fclose (arch);
}

