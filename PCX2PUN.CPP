#include <img.cpp>
#include <alloc.h>
#include <string.h>

typedef struct PUN_Encab {
               	char mens [16];
                    int cant;
                    int posic [64];
			};

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

   if ((ArchivoB = fopen (Nombre, "rb")) == NULL) return 0;
   fseek (ArchivoB, 128, 0);

/*   for (cont = 0; cont < 48; cont+=3)
      setrgbpalette (EGA_COL [cont],Info.Paleta [cont],Info.Paleta [cont+1],Info.Paleta [cont+2]);*/
    X = Info.Ancho - 1 + CX;
    for (Y = CY; Y < (Info.Alto + CY - 1); Y++)
    {
	  X = CX - 1;
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
    return 1;
}


void main (int argc, char *argv [])
{
	int gd = VGA, gm = VGAMED;
	initgraph (&gd, &gm, "");
     FILE *inf,*dest;
     void *punt;
     int cant, tam;
     char cad [80];
     PUN_Encab enc;
     if ((inf = fopen (argv [1],"rt")) == NULL) exit (1);
	fgets (cad,80,inf);
     cad [strlen (cad)-1]=0;
     if ((dest = fopen (cad,"wb")) == NULL) exit (1);
     if ((punt = farmalloc (64000)) == NULL) exit (1);
	fwrite (&enc, sizeof (enc),1,dest);
	fgets (cad,80,inf);
     cant = 0;
     do {
          cad [strlen (cad)-1]=0;
          cleardevice ();
          if (PonerPcx (cad,0,0))
          {
               getimage (0,0, Info.Ancho, Info.Alto, punt);
               enc.posic [cant] = ftell (dest);
               tam = imagesize (0,0, Info.Ancho, Info.Alto);
		     fwrite (&tam,sizeof (tam) ,1,dest);
		     fwrite (punt,tam ,1,dest);
               cant++;
          }
		fgets (cad,80,inf);
     } while (!feof (inf));
     enc.cant = cant;
	fseek (dest,0L, SEEK_SET);
	fwrite (&enc, sizeof (enc),1,dest);
     fclose (inf);
}