#include <dos.h>
#include <stdio.h>
#include <graphics.h>
#include <mem.h>
#include "defin.hpp"

#define WriteBitPlane(x) (outport (0x3C4,(1<<(x+8))+2))
#define ReadBitPlane(x) (outport (0x3CE,x * 256+4))
#define CantCronom 10

extern int Tiempo;

typedef struct SCronom {
				char activo;
                    int TFinal;
                    void (*func) ();
			};

SCronom Cronom [CantCronom];

void EmpezarGraf ()
{
	int gd = VGA, gm = VGAMED, error;
     error = registerbgidriver (EGAVGA_driver);
	if (error >= 0)
	{
		initgraph (&gd, &gm, "");
          error = graphresult ();
     }
     if (error < 0)
     {
     	printf ("\n\nNo se pudo inicializar el modo gr fico.\n");
          printf ("Programa abortado.\n");
          exit (1);
     }

     error = registerbgifont (small_font);
     if (error < 0)
     {
     	printf ("\n\nError en modo gr fico.\n");
          printf ("Programa abortado.\n");
          exit (1);
     }
}

int trunc (float n)
{
	if ((n - abs (n)) > 0.5) return (int) n+1;
     else return (int) n;
}


void EsperarWR ()
{
    while (!(inportb (0x3DA) & 8));
    while (inportb (0x3DA) & 8);
}

void PintArea (int x,int y, int xx, int yy, int col)
{
	setfillstyle (SOLID_FILL, col);
	bar (x,y,xx,yy);
}

void CopiarPagina (int pag)
{
     unsigned SF = 0xA000, SD = 0xA000;
     int bp;

     if (pag == 0) SD = 0xA800;
     else		    SF = 0xA800;

     for (bp = 0; bp < 4; bp++)
     {
          ReadBitPlane (bp);
          WriteBitPlane (bp);
		memcpy (MK_FP (0xA800,0),MK_FP (0xA000,0),0x6d60);
     }
}
void EncriptPant (int pag, int ocult)
{
     unsigned char aux;
     int bp;
     unsigned SF, dir, x,y;
     SF = pag? 0xA800 : 0xA000;
     for (bp = 0; bp < 4; bp++)
     {
          ReadBitPlane (bp);
          WriteBitPlane (bp);
          for (y = 0; y < 275; y++)
     		for (x = 0; x < 75; x++)
               {
                    dir = (y + 15) * 80 + x + 2;
                    aux = ((int) peekb (SF,dir) + (x + y) * ocult) % 256;
               	pokeb (SF, dir, aux);
               }
     }
}


// ESTAS FUNCIONES NO SE USARON AUN!!!!
void IniciarCronom ()
{
     int cont;
	for (cont = 0; cont < CantCronom; cont++)
     	Cronom [cont].activo = 0;
}

void VerificarCronom ()
{
     int cont;
	for (cont = 0; cont < CantCronom; cont++)
     	if (Cronom [cont].activo && (Cronom [cont].TFinal <= Tiempo))
          {
          	Cronom [cont].func ();
               Cronom [cont].activo = 0;
          }
}

void ActivarCronom (int Durac, void (*f) ())
{
     int cont;
	for (cont = 0; cont < CantCronom; cont++)
     	if (!Cronom [cont].activo)
          {
               Cronom [cont].activo = 1;
               Cronom [cont].TFinal = Tiempo + Durac;
               Cronom [cont].func = f;
               break;
          }
}