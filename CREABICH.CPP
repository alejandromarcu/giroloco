#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <alloc.h>
#include <mem.h>

#define NARCH "BICHOS.PUN"

typedef struct S_Hdr    {
		     unsigned char marca,
				   version,
				   codigo,
				   bits_pixel;
		     unsigned int  xmin,
				   ymin,
				   xmax,
				   ymax,
				   hres,
				   vres,
				   colores [48];
		     unsigned char reservado,
				   planos;
		     unsigned int  bytes_linea,
				   info_paleta;
		     unsigned char Relleno [58];
		} T_Hdr;

typedef struct S_Info {
			  unsigned int  Ancho,
					Alto;
			  unsigned char Paleta [768];
		       } T_Info;

unsigned char Buffer [4096];
long          Posic;
FILE         *ArchivoB;

unsigned int Res,
	     Cant,
	     Repit;

T_Info Info;

int EGA_COL [16] = {0,1,2,3,4,5,7,20,56,57,58,59,60,61,62,63};

/*-------------------- Leer_Info --------------------*/
void Leer_Info (char *Narch)
{
    FILE *Archivo;
    unsigned char Aux;
    int cont;
    T_Hdr Hdr;

    Archivo = fopen (Narch, "rb");
    fread (&Hdr, sizeof (Hdr), 1,Archivo);

    Info.Ancho   = Hdr.xmax - Hdr.xmin + 1;
    Info.Alto    = Hdr.ymax - Hdr.ymin + 1;
    fseek (Archivo, 0L, SEEK_END);
    fseek (Archivo, ftell (Archivo) - 768L, SEEK_SET);

    fclose (Archivo);
}

/*------------------------------ PonerPcx ------------------------------*/
int PonerPcx (char *Nombre, int CX, int CY)
{
   int X, Y, cont, rep = 0;
   unsigned char Aux;

   Leer_Info (Nombre);

   ArchivoB = fopen (Nombre, "rb");
   if (!ArchivoB) return 1;
   fseek (ArchivoB, 128, 0);

/*   for (cont = 0; cont < 48; cont+=3)
      setrgbpalette (EGA_COL [cont],Info.Paleta [cont],Info.Paleta [cont+1],Info.Paleta [cont+2]);*/
    X = Info.Ancho - 1 + CX;
    for (Y = CY; Y < (Info.Alto + CY - 1); Y++)
    {
/*       if (X >= (Info.Ancho + CX - 1))
       {
          X -= Info.Ancho + 1;
       	line (CX-1,Y-1,X-1,Y-1);
       }
	  else*/ X =CX - 1;
	  do {
		fread (&Aux,1,1,ArchivoB);
		if (Aux < 192) putpixel (X - 1, Y - 1, Aux);
		else {
		     rep = Aux - 192;
		     fread (&Aux,1,1,ArchivoB);

		     setcolor (Aux);
		     line (X - 1, Y - 1, X + rep - 1, Y - 1);
		     X+=rep-1;
		  }
		X++;
	  } while (X < (Info.Ancho + CX - 1));
      }
    fclose (ArchivoB);
    return 0;
}

void CrearMasc (int x, int y, int xx, int yy)
{
     int cx, cy,a;
	for (cy = y; cy < yy; cy++)
		for (cx = x; cx < xx; cx++)
          	putpixel (cx, cy + 90, getpixel (cx, cy)? 0 : 15);
}

void Rotar (int x, int y)
{
	int cx, cy;
     for (cy = 0; cy < Info.Alto; cy++)
     	for (cx = 0; cx < Info.Ancho; cx++)
			putpixel (cx + x + 30, cy + y, getpixel (cy +x , cx + y));
}

void Rotar2 (int x, int y)
{
	int cx, cy;
     for (cy = 0; cy < 30; cy++)
     	for (cx = 0; cx < Info.Ancho; cx++)
			putpixel (cx + x + 30, y + Info.Alto - cy, getpixel (cy +x , cx + y));
}

void Espejo (int x, int y)
{
	int cx, cy;
     for (cy = 0; cy < Info.Alto; cy++)
     	for (cx = 0; cx < Info.Ancho; cx++)
			putpixel (cx + x + 60, cy + y, getpixel (x + cx , y + Info.Alto - cy - 1));
}

void Grabar (int PrimeraVez)
{
	int cx, cy, tam;
     FILE *Arch;
     void *punt;
     setfillstyle (1, 14);
     bar (300,0,600,300);
     tam = imagesize (1,1,Info.Ancho, Info.Alto);
     punt = malloc (tam);
     Arch = fopen (NARCH,"a+b");
     if (PrimeraVez) fwrite (&tam,sizeof (tam) ,1,Arch);
	for (cx = 1; cx <= 4; cx++)
		for (cy = 1; cy <= 6; cy++)
          {
               getimage (cx*30,cy*30, cx*30+Info.Ancho-1, cy*30+Info.Alto-1, punt);
               putimage (cx*30+300,cy*30, punt,0);
		     fwrite (punt,tam,1,Arch);
          }
     fclose (Arch);
}

void main ()
{
	int gd = VGA, gm = VGAMED, cy, nb;
     char arch [15];
	initgraph (&gd, &gm, "");
     remove (NARCH);
     nb =1;
     do {
         sprintf (arch, "BICHO%d-1.PCX",nb);
  	    if (PonerPcx (arch,31,30)) break;
         sprintf (arch, "BICHO%d-2.PCX",nb);
  	    PonerPcx (arch,31,60);
         sprintf (arch, "BICHO%d-3.PCX",nb);
  	    PonerPcx (arch,31,90);
         CrearMasc (30,30,Info.Ancho+30, Info.Alto + 30);
         CrearMasc (30,60,Info.Ancho+30, Info.Alto + 60);
         CrearMasc (30,90,Info.Ancho+30, Info.Alto + 90);
	    for (cy = 1; cy <= 6; cy++)
         {
         	    Rotar (30, cy * 30);
              Espejo (30, cy * 30);
      	    Rotar (90, cy * 30);
         }
	    Grabar (nb == 1);
         nb++;
     } while (1);
     getch ();
	closegraph ();
}