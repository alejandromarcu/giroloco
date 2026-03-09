#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>

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

int EGA_COL [16] = {0,1,2,3,4,5,7,20,56,57,58,59,60,61,62,63};

/*-------------------- Leer_Info --------------------*/
void Leer_Info (char *Narch, T_Info *Info)
{
    FILE *Archivo;
    unsigned char Aux;
    int cont;
    T_Hdr Hdr;

    Archivo = fopen (Narch, "rb");
    fread (&Hdr, sizeof (Hdr), 1,Archivo);

    Info->Ancho   = Hdr.xmax - Hdr.xmin + 1;
    Info->Alto    = Hdr.ymax - Hdr.ymin + 1;
    fseek (Archivo, 0L, SEEK_END);
    fseek (Archivo, ftell (Archivo) - 768L, SEEK_SET);

/*     for (Cont = 0; Cont < 16; Cont++)
	 GetRgbPalette (Cont, Info.Paleta [Cont]);*/

     for (cont = 0; cont < 768; cont += 3)
     {
	  fread (&Aux, 1, 1, Archivo);
	  Info->Paleta [cont] = Aux / 4;
	  fread (&Aux, 1, 1, Archivo);
	  Info->Paleta [cont+1] = Aux / 4;
	  fread (&Aux, 1, 1, Archivo);
	  Info->Paleta [cont+2] = Aux / 4;
     }
    fclose (Archivo);
}

/*------------------------------ PonerPcx ------------------------------*/
void PonerPcx (char *Nombre, int CX, int CY)
{
   T_Info Info;
   int X, Y, cont, rep = 0;
   unsigned char Aux;

   Leer_Info (Nombre,&Info);

   ArchivoB = fopen (Nombre, "rb");
   //setvbuf (ArchivoB, NULL, _IOFBF, 8192);
   fseek (ArchivoB, 128, 0);

/*   for (cont = 0; cont < 48; cont+=3)
      setrgbpalette (EGA_COL [cont],Info.Paleta [cont],Info.Paleta [cont+1],Info.Paleta [cont+2]);*/
    X = Info.Ancho - 1 + CX;
    for (Y = CY; Y < (Info.Alto + CY - 1); Y++)
    {
       if (X >= (Info.Ancho + CX - 1))
       {
          X -= Info.Ancho + 1;
       	line (CX-1,Y-1,X-1,Y-1);
       }
	  else X =CX;
	  do {
		fread (&Aux,1,1,ArchivoB);
//          if (Aux < 192 && Aux > 0) Aux = BROWN;
		if (Aux < 192) putpixel (X - 1, Y - 1, Aux);
		else {
		     rep = Aux - 192;
		     fread (&Aux,1,1,ArchivoB);
          if (Aux < 192 && Aux > 0) Aux = BROWN;

		     setcolor (Aux);
		     line (X - 1, Y - 1, X + rep - 1, Y - 1);
		     X+=rep-1;
		  }
		X++;
	  } while (X < (Info.Ancho + CX - 1));
      }
    fclose (ArchivoB);
}


void DibujarPCX ()
{
   T_Info Info;
   PonerPcx ("pru.pcx",20,15);
}