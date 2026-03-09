#include "defin.hpp"
#include "nodos.hpp"
#include "pantalla.hpp"
#include "rot.hpp"
#include "menu.hpp"
#include <stdio.h>
#include <string.h>

#define MNodos 1
#define MEntre 2

class CreaPant : public Pantalla {
		public:
			void Crear ();
               void Opciones ();
			void PasarANodos ();
               void PulsoEnNodo (char c);
               void EscrBichos ();
               void EscrMenu ();
          	void AgregarBicho ();
          	void BorrarBicho ();
          	void EditarBicho ();
			void NroFondo ();
			};

CProxPag ProxPag;
CreaPant pant;
CRecords Records;
Rot rot;
int      NivelInicial;

int x = 0, y = 0;


void DibujarCurs (int x, int y)
{
    setwritemode (1);
    setcolor (WHITE);
    line (VerdX (x) - 10, VerdY (y),VerdX (x) + 10, VerdY (y) );
    line (VerdX (x), VerdY (y) - 8,VerdX (x), VerdY (y) + 8);
    setwritemode (0);
}

void CreaPant::PulsoEnNodo (char c)
{
      int param;
	 if (c==ID_NODO_COM)
	 {
		 AsignarNodo (Nivel.cx,Nivel.cy,0,ID_NODO);
		 nodos [Nivel.cx][Nivel.cy]->Dibujar ();
	 }
	 if (c>= 'K')
	 {
		outtextxy (10, 300, "Ingrese el parametro:");
		gotoxy (32, 22);
		scanf ("%d", &param);
		setfillstyle (SOLID_FILL, LIGHTGRAY);
		bar (0,290,639,349);
	 }
	 if ((x != Nivel.cx) || (y != Nivel.cy)) AsignarNodo (x,y,param,c);
	 setfillstyle (SOLID_FILL, 0);
	 bar (VerdX (x) - 7, VerdY (y) -7, VerdX (x) + 7, VerdY (y) + 7);
	 if (c != ID_NODO_INEX) nodos [x][y]->Dibujar ();
      else nodos [x][y]->GuardarFondo_Dibujar ();

}

void CreaPant::Crear ()
{
   Nivel.cx = 0; Nivel.cy = 0;
   char c, c2;
   DibujarCurs (x,y);
   do {
	 c = toupper (getch ());
	 if (!c) c2 = getch ();
	 else c2 = 0;
	 DibujarCurs (x,y);
      if (c2 == 68) //F10
      {
         int cx,cy;
	    for (cy = 0;cy < NODOS_Y; cy++)
			for (cx = 0; cx < NODOS_X; cx++)
      		 AsignarNodo (cx,cy,0,ID_NODO_INEX);
       	AsignarNodo (0,0,0,ID_NODO_COM);
		pant.DibujarNodos ();

      }

	 switch (c2) {
	 case 72 : if (y) y--; break;
	 case 75 : if (x) x--; break;
	 case 77 : if (x < (NODOS_X - 1)) x++; break;
	 case 80 : if (y < (NODOS_Y - 1)) y++; break;
	 }
      PulsoEnNodo (c);
	 DibujarCurs (x,y);
   } while (c != 27);
}


void CreaPant::PasarANodos ()
{
    int x, y, cont = 0;
    for (y = 0; y < NODOS_Y; y++)
	   for (x = 0; x < NODOS_X; x++)
	   {
		Nivel.Info [cont] = nodos [x][y]->ID ();
		cont++;
	   }
}

void CreaPant::EscrBichos ()
{
	int cont;
	gotoxy (29, 1);
     printf ("ษอออออออออออออออออออออออออออออออออออออออออป");
     gotoxy (29,2);
     printf ("บNro. ณTipo ณ  X  ณ  Y  ณIntelณApar.ณDuracบ");
	gotoxy (29, 3);
     printf ("บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤบ");
     for (cont = 0; cont < Nivel.CantBichos; cont++)
     {
     	gotoxy (29, cont + 4);
          printf ("บ%4d ณ%4d ณ%4d ณ%4d ณ%4d ณ%4d ณ%4d บ",cont, Nivel.Bichos [cont].tipo,
			   Nivel.Bichos [cont].inix,Nivel.Bichos [cont].iniy,Nivel.Bichos [cont].intel,
			   Nivel.Bichos [cont].taparic,Nivel.Bichos [cont].duracion);
     }
     gotoxy (29,cont+4);
     printf ("บ     ณ     ณ     ณ     ณ     ณ     ณ     บ");
     gotoxy (29,cont+5);
     printf ("ศอออออออออออออออออออออออออออออออออออออออออผ");
}


void CreaPant::AgregarBicho ()
{
     SInfoBicho B;
     int y = Nivel.CantBichos+4;
     gotoxy (30,y);
	printf ("%4d",Nivel.CantBichos);
	gotoxy (36,y);
     scanf ("%d", &B.tipo);
     gotoxy (42,y);
     scanf ("%d", &B.inix);
     gotoxy (48,y);
     scanf ("%d", &B.iniy);
     gotoxy (54,y);
     scanf ("%d", &B.intel);
     gotoxy (60,y);
     scanf ("%d", &B.taparic);
     gotoxy (66,y);
     scanf ("%d", &B.duracion);
     Nivel.Bichos [Nivel.CantBichos] = B;
	Nivel.CantBichos++;
}

void CreaPant::BorrarBicho ()
{
     int n, cont;
	gotoxy (1,24);
     printf ("Nro de bicho: ");
     scanf ("%d", &n);
     if (n >= Nivel.CantBichos) return;
     Nivel.CantBichos--;
     for (cont = n; cont < Nivel.CantBichos; cont++)
        Nivel.Bichos [cont] = Nivel.Bichos [cont+1];
}

void CreaPant::EditarBicho ()
{
}

void CreaPant::NroFondo ()
{
    gotoxy (1,24);
    printf ("Numero de fondo: ");
    scanf ("%d",&Nivel.NroFondo);
}

void CreaPant::EscrMenu ()
{
	printf ("A. Agregar bicho\n");
	printf ("B. Borrar bicho\n");
//	printf ("E. Editar bicho\n");
	printf ("\n");
	printf ("F. Nro de fondo: %d\n",Nivel.NroFondo);
	printf ("\n");
	printf ("ESC. Grabar y salir\n");
}

void CreaPant::Opciones ()
{
     char tec;
 	do {
 		clrscr ();
 		EscrMenu ();
     	EscrBichos ();
          gotoxy (24, 1);
          tec = toupper (getch ());
     	switch (tec) {
          case 'A': AgregarBicho (); break;
          case 'B': BorrarBicho (); break;
          case 'E': EditarBicho (); break;
          case 'F': NroFondo (); break;
          }

 	} while (tec != 27);

}


void main ()
{
	int gd = VGA, gm = VGAMED;
     char NomArch [20];

     clrscr ();
     printf ("Nombre de la pantalla a editar/crear (p/ej 10-20): ");
     gets (NomArch);
     strcat (NomArch, ".pan");
	if (!pant.Cargar (NomArch))
	{
	    int x, y;
	    for (y = 0; y < NODOS_Y; y++)
			for (x = 0; x < NODOS_X; x++)
				pant.AsignarNodo (x,y, 0, ID_NODO);
		pant.AsignarNodo (0,0,0,ID_NODO_COM);
          pant.Nivel.CantBichos = 0;
	}

	initgraph (&gd, &gm, "");
	CargarImNodos ();
	pant.Preparar (-1);

	pant.DibujarNodos ();
	setactivepage (0);
	pant.Crear ();
 	closegraph ();
     pant.Opciones ();
	pant.PasarANodos ();
	pant.Grabar (NomArch);
}