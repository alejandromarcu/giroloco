#include "defin.hpp"
#include "nodos.hpp"
#include "pantalla.hpp"
#include "rot.hpp"
#include "sonido.hpp"
#include <dos.h>
#include <stdio.h>

extern Rot rot;
extern Pantalla pant;
extern CProxPag ProxPag;
extern int Tiempo;

char far *ImNodos, *ImNodosMasc;
int PTCX, PTCY;

void *Nodo::PuntImag ()
{
     return ImNodos+MEMORIA_NODO * NumIm;
}

void *Nodo::PuntMasc ()
{
     return ImNodosMasc+MEMORIA_NODO * NumIm;
}

void CargarImNodos ()
{
    int tam;
    FILE *arch;
    arch = fopen ("NODOS.DAT","rb");
    fseek (arch, 0L, SEEK_END);
    tam = ftell (arch);
    fseek (arch, 0L, SEEK_SET);
    ImNodos = (char far *) farmalloc (tam);
    if (!ImNodos) exit (1);
    fread ((void *) ImNodos, tam, 1, arch);
    fclose (arch);

    arch = fopen ("MASCARAS.DAT","rb");
    fseek (arch, 0L, SEEK_END);
    tam = ftell (arch);
    fseek (arch, 0L, SEEK_SET);
    ImNodosMasc = (char far *) farmalloc (tam);
    if (!ImNodosMasc) exit (1);
    fread ((void *) ImNodosMasc, tam, 1, arch);
    fclose (arch);
}


/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE NODO
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
void Nodo::Dibujar ()
{
// Estas tres lineas siguientes son para depuracion.
//	putimage (500, 330, ImNodosMasc+MEMORIA_NODO * NumIm ,COPY_PUT);
//	putimage (500, 310, ImNodos+MEMORIA_NODO * NumIm ,COPY_PUT);
//	putimage (520,310 , Fondo, COPY_PUT);
	if (!NumIm) return;
	putimage (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, ImNodosMasc+MEMORIA_NODO * NumIm , AND_PUT);
	putimage (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, ImNodos+MEMORIA_NODO * NumIm, OR_PUT);
}

void Nodo::DibujarProxPag ()
{
	if (!NumIm) return;
	putimage (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, ImNodosMasc+MEMORIA_NODO * NumIm , AND_PUT);
	putimage (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, ImNodos+MEMORIA_NODO * NumIm, OR_PUT);
     ProxPag.Agregar (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, ImNodosMasc+MEMORIA_NODO * NumIm , AND_PUT);
	ProxPag.Agregar (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, ImNodos+MEMORIA_NODO * NumIm, OR_PUT);
}

void Nodo::Borrar ()
{
	putimage (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, Fondo, COPY_PUT);
}

void Nodo::BorrarProxPag ()
{
	putimage (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, Fondo, COPY_PUT);
	ProxPag.Agregar (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, Fondo, COPY_PUT);
}

void Nodo::GuardarFondo ()
{
     if (!Fondo) Fondo = farmalloc (imagesize (0,0,2*RADIO_NODO_X,2*RADIO_NODO_Y));
     getimage (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y,
     		VerdX (x)+RADIO_NODO_X, VerdY (y)+RADIO_NODO_Y, Fondo);
}

void Nodo::GuardarFondo_Dibujar ()
{
     GuardarFondo ();
	Dibujar ();
}

/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE NODOEN
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
void NodoEn::Dibujar ()
{
	if (estado != EXISTE) return;
	NumIm = INODO_EN + energia / 180;
	Nodo::Dibujar ();
}


Result NodoEn::Usar (int ang)
{
	energia -= abs (ang);
	if ((energia <= 0) && (estado == EXISTE))
	{
		estado = NO_ENERGIA;
		cuadro = 0;
          Sonido (S_EXPLOSION);
	}
	if (energia <= 0) rot.AumentarPuntos (1);
	if (estado == NO_EXISTE) return PERDIO;
	return NADA;
}

void NodoEn::Actuar ()
{
	if (estado != NO_ENERGIA) return;
	if (!(cuadro % 8))
	{
 		NumIm = cuadro / 8 + INODO_EN_SIN_EN;
		if (NumIm == (INODO_EN_SIN_EN + 8)) estado = NO_EXISTE;
		if (NumIm >= (INODO_EN_SIN_EN + 7)) NumIm = 0;
          if (NumIm == (INODO_EN_SIN_EN + 6)) Sonido (S_FIN_NODO_EN);
	}
	cuadro++;
	if (rot.x != x || rot.y != y) Nodo::Borrar ();
	Nodo::Dibujar ();
}

/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE NODOAS
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
Result NodoAs::Acercarse (int Angulo)
{
    if (Angulo <= abs (rot.vel/2)) return PERDIO;
    return NADA;
}
/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE NODOFIN
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
void NodoFin::Actuar ()
{
	if (cuadro == 5)
	{
		if (NumIm == INODO_FINAL) dir = 1;
		if (NumIm == (INODO_FINAL+6)) dir = -1;
		NumIm += dir;
     	Nodo::Borrar ();
		Nodo::Dibujar ();
	}
	if (cuadro == 6)
	{
     	Nodo::Borrar ();
		Nodo::Dibujar ();
		cuadro = 0;
	}
	cuadro++;
}

/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE NODOVIDA
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
Result NodoVida::Usar (int Angulo)
{
	if (Usada) return NADA;
	rot.GanarVida ();
	NumIm = INODO;
	Usada = 1;
	pant.CambiarPantalla (x,y, ID_NODO << 8);
	return NADA;
}

/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE NODOPUNTOS
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
Result NodoPuntos::Usar (int Angulo)
{
	if (!Puntos) return NADA;
	rot.AumentarPuntos (Puntos);
	NumIm = INODO;
	Puntos = 0;
	pant.CambiarPantalla (x,y, ID_NODO << 8);
	return NADA;
}


/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE NODOPILDTC
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
Result NodoPildTC::Usar (int Angulo)
{
	int cx, cy;
     if (Usado) return NADA;
     Usado = 1;
     TFinal = Durac + Tiempo;
     PTCX = x; PTCY = y;
     for (cy = 0; cy < NODOS_Y; cy++)
     	for (cx = 0; cx < NODOS_X; cx++)
          {
                   if (x==cx && y==cy) continue;
                   if (pant.nodos [cx][cy]->estado == NO_EXISTE) continue;
                   nodosAnt [cx][cy] = pant.nodos [cx][cy];
			    switch (nodosAnt [cx][cy]->ID () / 256) {
                   case ID_NODO_FIN:
                   case ID_NODO_COM: break;
                   default:
                        pant.AsignarNodo (cx,cy,0, ID_NODO);
                        pant.nodos [cx][cy]->GuardarFondo ();
               	    pant.nodos [cx][cy]->DibujarProxPag ();
				}
          }
     return NADA;
}

void NodoPildTC::Actuar ()
{
     int cx, cy;
     static int TiempoAnt;
     if (Usado == 1) pant.PonerTiempo ((Durac - TFinal + Tiempo) * 100 / Durac);
     if ((Usado != 1) || (Tiempo < (TFinal-3))) return;
     if ((TiempoAnt != Tiempo) && (Tiempo < TFinal))
     {
     	TiempoAnt = Tiempo;
          Sonido (S_AVISO_FIN_PILDTC);
          return;
     }
	if (Tiempo < TFinal) return;
     Usado = 2;
     Sonido (S_FIN_PILDTC);
     pant.FinMedicionTiempo ();
     for (cy = 0; cy < NODOS_Y; cy++)
     	for (cx = 0; cx < NODOS_X; cx++)
          {
                   if (pant.nodos [cx][cy]->estado == NO_EXISTE) continue;
                   if (x==cx && y==cy)
			    {
                         NumIm = INODO;
				     pant.nodos [cx][cy]->DibujarProxPag ();
			    	     continue;
                   }
			    switch (nodosAnt [cx][cy]->ID () / 256) {
                   case ID_NODO_FIN:
                   case ID_NODO_COM: break;
                   default:
                        delete pant.nodos [cx][cy];
                        pant.nodos [cx][cy] = nodosAnt [cx][cy];
				    if (pant.nodos [cx][cy]->estado == EXISTE)
				    		pant.nodos [cx][cy]->DibujarProxPag ();
                        else
                        		pant.nodos [cx][cy]->BorrarProxPag ();
                   }
          }
}

/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE NODOPILDMB
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
Result NodoPildMB::Usar (int Angulo)
{
     if (Usado) return NADA;
     rot.PildMBActiva = 1;
     Usado = 1;
     TFinal = Durac + Tiempo;
     return NADA;
}

void NodoPildMB::Actuar ()
{
     static int TiempoAnt;
     if (Usado == 1) pant.PonerTiempo ((Durac - TFinal + Tiempo) * 100 / Durac);
     if ((Usado != 1) || (Tiempo < (TFinal-3))) return;
     if ((TiempoAnt != Tiempo) && (Tiempo < TFinal))
     {
     	TiempoAnt = Tiempo;
          Sonido (S_AVISO_FIN_PILDTC);
          return;
     }
	if (Tiempo < TFinal) return;
     NumIm = INODO;
     pant.nodos [x][y]->DibujarProxPag ();
     Usado = 2;
     rot.PildMBActiva = 0;
     Sonido (S_FIN_PILDTC);
     pant.FinMedicionTiempo ();
}

