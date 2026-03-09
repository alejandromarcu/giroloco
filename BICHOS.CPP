#include "defin.hpp"
#include "funcom.hpp"
#include "bichos.hpp"
#include "rot.hpp"
#include "pantalla.hpp"
#include <dos.h>
#include <stdio.h>

extern Rot rot;
extern Tiempo;
extern CProxPag ProxPag;
Bicho  *Bichos [MAX_BICHOS];

void *ImBichos;
int tamBicho;

int CargarPUN (void **P, char *narch)
{
     FILE *arch;
     int cont, tam;
     arch = fopen (narch,"rb");
	fread (&tam, sizeof (tam),1,arch);
     *P = farmalloc (tam * 24 * TIPO_BICHOS);
     fread (*P, tam * 24 * TIPO_BICHOS,1,arch);
     fclose (arch);
     return tam;
}

void InicBichos ()
{
	tamBicho = CargarPUN (&ImBichos, "bichos.pun");
}

void far *Imagen (int nim, int norm, int or, int tipo)
{
	return (char far *) ImBichos + tamBicho * (nim + norm * 3 + or * 6 + (tipo -1)* 24);
}

Bicho::Bicho (SInfoBicho i)
{
     info = i;
	x = xreal = (info.inix + 0.5) * DIST_NODOX + MARGEN_X;
	y = yreal = (info.iniy + 0.5) * DIST_NODOY + MARGEN_Y;
	dx= 1;
	dy=0;
     activo = 0;
     existe = 1;
     Fondo = farmalloc (imagesize (0,0,B_SAncho * 2, B_SAlto * 2));
     NumIm = 0;
     switch (info.tipo) {
     case 1: vel = 1.9; TCambioCuad = 3; break;
     case 2: vel = 1;   TCambioCuad = 4; break;
     case 3: vel = 0.3; TCambioCuad = 6; break;
     case 4: vel = 0.6; TCambioCuad = 5; break;
     }
     LeerFondo ();
}

Bicho::~Bicho ()
{
    farfree (Fondo);
}

void Bicho::Borrar ()
{
    putimage (x - B_SAncho, y - B_SAlto, Fondo, COPY_PUT);
}

void Bicho::BorrarProxPag ()
{
    putimage (x - B_SAncho, y - B_SAlto, Fondo, COPY_PUT);
    ProxPag.Agregar (x - B_SAncho, y - B_SAlto, Fondo, COPY_PUT);
}

int Bicho::Orientacion ()
{
     if (dy > 0) return 0;
     if (dx > 0) return 1;
     if (dy < 0) return 2;
     if (dx < 0) return 3;
}

void Bicho::LeerFondo ()
{
     getimage (x - B_SAncho, y - B_SAlto,x + B_SAncho, y + B_SAlto, Fondo);
}

void Bicho::Dibujar ()
{
	int orient = Orientacion ();
     putimage (x- B_SAncho+2,y-B_SAlto+2, Imagen (NumIm,1,orient,info.tipo), AND_PUT);
     putimage (x- B_SAncho+2,y-B_SAlto+2, Imagen (NumIm,0,orient,info.tipo), OR_PUT);
}

Result Bicho::VerificarROT ()
{
     if (!rot.PosibleChoque (x,y)) return NADA;
	if (rot.PuntoAdentro (x,y)) return PERDIO;
  	if (rot.PuntoAdentro (x-6,y-6)) return PERDIO;
 	if (rot.PuntoAdentro (x-6,y+6)) return PERDIO;
	if (rot.PuntoAdentro (x+6,y-6)) return PERDIO;
	if (rot.PuntoAdentro (x+6,y+6)) return PERDIO;
     return NADA;
}

Result Bicho::Actuar ()
{
     Result Aux;
     Borrar ();
 	contCuad++;
 	if (contCuad >= TCambioCuad )
 	{
		NumIm = (NumIm + 1) % 3 ;
		contCuad = 0;
	}
     if (Tiempo >= (info.taparic + info.duracion-3)) return VerificarROT ();
	xreal += (float) dx * vel;
	yreal += (float) dy * vel;
     x = trunc (xreal);
     y = trunc (yreal);

	if ((((x - MARGEN_X + DIST_NODOX) % DIST_NODOX) == DIST_NODOX / 2) &&
	   (((y - MARGEN_Y + DIST_NODOY) % DIST_NODOY) == DIST_NODOY / 2))
     {
	   if (random (info.intel) < 100)
			switch (random (4)) {
			case 0 : dx = 1; dy = 0; break;
			case 1 : dx = 0; dy = 1; break;
			case 2 : dx = -1; dy = 0; break;
			case 3 : dx = 0; dy = -1; break;
			}
	   else
 		   if (abs (x - VerdX (rot.x)) > abs (y - VerdY (rot.y)))
		   {
		   		dx = x < VerdX (rot.x)? 1 : -1;
				dy = 0;
		   } else {
		   		dy = y < VerdY (rot.y)? 1 : -1;
				dx = 0;
		   }

     }
     if ((x - MARGEN_X) <= -(DIST_NODOX/2) && dx < 0) dx = -dx;
     if ((y - MARGEN_Y) <= -(DIST_NODOY/2) && dy < 0) dy = -dy;
     if (x >= (NODOS_X+1) * DIST_NODOX && dx > 0) dx = -dx;
     if (y >= (NODOS_Y+1) * DIST_NODOY && dy > 0) dy = -dy;
	Aux = VerificarROT ();
     if (!(rot.PildMBActiva)) return Aux;
     if (Aux == NADA) return NADA;
     // Si llego hasta aca es porque mato a un bicho.
     info.duracion = 0;
     rot.AumentarPuntos (10000);
     return NADA;
}

void Bicho::VerifTiempo ()
{
	if ((Tiempo == info.taparic) && !activo)
	{
		activo = 1;
          Dibujar ();
     }
     if (Tiempo >= (info.taparic + info.duracion))
	{
		activo = 0;
          existe = 0;
          BorrarProxPag ();
     }
}

void VerificarChoque (int b1, int b2)
{
     int orient1 = Bichos [b1]->Orientacion (),
     	orient2 = Bichos [b2]->Orientacion ();

	if ((Bichos [b1]->x == Bichos [b2]->x) &&
         (abs (Bichos [b1]->y - Bichos [b2]->y) < 20))
     {
     	if ((Bichos [b1]->y < Bichos [b2]->y) &&
          	(orient1 == 0)   &&
          	(orient2 == 2))
               Bichos [b1]->dy = -Bichos [b1]->dy;
     	if ((Bichos [b1]->y > Bichos [b2]->y) &&
          	(orient1 == 2)   &&
          	(orient2 == 0))
               Bichos [b1]->dy = -Bichos [b1]->dy;
     }
	if ((Bichos [b1]->y== Bichos [b2]->y) &&
         (abs (Bichos [b1]->x - Bichos [b2]->x) < 20))
     {
     	if ((Bichos [b1]->x < Bichos [b2]->x) &&
          	(orient1 == 1)   &&
          	(orient2 == 3))
               Bichos [b1]->dx = -Bichos [b1]->dx;
     	if ((Bichos [b1]->x > Bichos [b2]->x) &&
          	(orient1 == 3)   &&
          	(orient2 == 1))
               Bichos [b1]->dx = -Bichos [b1]->dx;
     }

}

Result Borrar_ActuarBichos ()
{
 	int cont, cont2;
     Result r, res;
     for (cont = 0; cont < MAX_BICHOS; cont++)
         if (Bichos [cont] && Bichos [cont]->existe) Bichos [cont]->VerifTiempo ();

 /*    for (cont = 0; cont < MAX_BICHOS;cont++)
     	for (cont2 = cont+1; cont2 < MAX_BICHOS;cont2++)
          	if (Bichos [cont] && Bichos [cont]->activo && Bichos [cont2]->activo)
               	VerificarChoque (cont, cont2);
*/

     res = NADA;
     for (cont = 0; cont < MAX_BICHOS;cont++)
     	if (Bichos [cont] && Bichos [cont]->activo)
         	    if ((r = Bichos [cont]->Actuar ()) != NADA) res = r;
	return res;
}

void DibujarBichos ()
{
 	int cont;
     for (cont = 0; cont < MAX_BICHOS;cont++)
     	if (Bichos [cont] && Bichos [cont]->activo)
         		Bichos [cont]->LeerFondo ();

     for (cont = 0; cont < MAX_BICHOS;cont++)
     	if (Bichos [cont] && Bichos [cont]->activo)
         		Bichos [cont]->Dibujar ();
}

