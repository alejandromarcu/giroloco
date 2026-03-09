#include "defin.hpp"
#include "nodos.hpp"
#include "pantalla.hpp"
#include "rot.hpp"
#include "bichos.hpp"
#include "sonido.hpp"
#include <stdio.h>
#include <mem.h>
#include <time.h>

#define TIEMPOX	 	 364
#define TIEMPOY           316
#define TIEMPODX	 	 109
#define TIEMPODY	 	 23

extern Rot rot;
extern CProxPag ProxPag;
int Tiempo, CantPant;
clock_t TiempoIn;
extern int NivelInicial;
int np;   // Numero de pagina

void BarProxPag (int x, int y, int xx, int yy, char color)
{
	setfillstyle (SOLID_FILL, color);
     bar (x,y,xx,yy);
     ProxPag.AgregarBar (x,y,xx,yy, color);
}

/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE CProxPag
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
void CProxPag::Agregar (int px, int py, void *pi, char pmodo)
{
     unsigned *tx, *ty;
     if (cant > MAX_REST_PP)
	{
		closegraph ();
		printf ("Error en CProxPag::Agregar: demasiadas imagenes\n\n");
		exit (1);
	}
	Imag [cant].x = px;
	Imag [cant].y = py;
	Imag [cant].modo = pmodo;
	tx = (unsigned int *) pi;
	ty = tx+1;
     Imag [cant].tam = imagesize (0,0,*tx,*ty);
     Imag [cant].i = (char *) farmalloc (Imag [cant].tam);

if (Imag [cant].i == NULL)
{
	closegraph ();
	printf ("Error en CProxPag::Agregar: insuficiente memoria\n\n");
	exit (1);
}
	memcpy (Imag [cant].i, pi,Imag [cant].tam);
     cant++;
}

void CProxPag::AgregarBar (int x, int y, int xx, int yy, char color)
{
     unsigned int *p;
     Imag [cant].x = 0xFFFF;
     Imag [cant].modo =color;

     Imag [cant].tam = 16;
	Imag [cant].i = (char *) farmalloc (16);
     p = (unsigned int *) Imag [cant].i;
     *p++ = x;
     *p++ = y;
     *p++ = xx;
     *p++ = yy;
	cant++;
}


void CProxPag::Limpiar ()
{
	int cont;
     unsigned int *p, x1,y1,x2,y2;
     for (cont = 0; cont < cant; cont++)
     {
     	if (Imag [cont].x == 0xFFFF)
          {
			p = (unsigned int *) Imag [cont].i;
               setfillstyle (SOLID_FILL, Imag [cont].modo);
               x1 = *p++; y1 = *p++; x2 = *p++; y2 = *p++;
               bar (x1,y1,x2,y2);
          }
		else putimage (Imag [cont].x,Imag [cont].y, Imag[cont].i,Imag[cont].modo);
          farfree (Imag [cont].i);
     }
     cant = 0;
}

void CProxPag::LiberarMem ()
{
	int cont;
     for (cont = 0; cont < cant; cont++)
          farfree (Imag [cont].i);
     cant = 0;
}

/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE Pantalla
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
void Pantalla::IniciarJuego ()
{
/*   	if (NivelInicial == 1)
   	{
		num = 1;
          return;
     }*/
     FILE *arch;
     int cont;

     if ((arch = fopen ("PANT.DAT","rb")) == NULL) return;
     fseek (arch, 0L, SEEK_END);
	CantPant = ftell (arch) /TAM_PANT;
     cont = 0;
     do {
     	fseek (arch, (long) (cont) * TAM_PANT, SEEK_SET);
     	fread (&Nivel, TAM_PANT, 1, arch);
          cont++;
     } while (Nivel.Nro != (NivelInicial * 256 + 1));
     fclose (arch);
     num = cont;
}

void Pantalla::PonerTiempo (int porc)
{
     static int ant = -1;
     int pos;
     if (ant == porc) return;
     ant = porc;
     pos = TIEMPOX + TIEMPODX - porc * TIEMPODX / 100;
     BarProxPag (TIEMPOX, TIEMPOY, pos, TIEMPOY+TIEMPODY, LIGHTGREEN);
     BarProxPag (pos, TIEMPOY, TIEMPOX+TIEMPODX, TIEMPOY + TIEMPODY, LIGHTRED);
}

void Pantalla::FinMedicionTiempo ()
{
     BarProxPag (TIEMPOX, TIEMPOY, TIEMPOX+TIEMPODX, TIEMPOY+TIEMPODY, BROWN);
}


void Pantalla::DibujarNodos ()
{
	int x, y;
	setactivepage (0);
	for (y = 0; y < NODOS_Y; y++)
		for (x = 0; x < NODOS_X; x++)
			nodos [x][y]->GuardarFondo_Dibujar ();
}

void Pantalla::DibujarEntorno ()
{
//     setactivepage (1);
//     cleardevice ();
     setvisualpage (1);
     setactivepage (0);
     CargarIBP (0,0,0,IBP_ENTORNO);
     CopiarPagina (0);
}


Result Pantalla::VerVecinos (int x, int y, int a)
{
	if ((a > 45) && (a <= 135)) if (!y) return NADA;
						   else return nodos [x][y-1]->Acercarse (abs (90-a));
	if ((a > 135) && (a <= 225)) if (!x) return NADA;
						   else return nodos [x-1][y]->Acercarse (abs (180-a));
	if ((a > 225) && (a <= 315)) if (y >= (NODOS_Y-1)) return NADA;
						   else return nodos [x][y+1]->Acercarse (abs (270-a));
						   if (x >= (NODOS_X-1)) return NADA;
						   else return nodos [x+1][y]->Acercarse (abs ((a + 45)% 360 - 45));
}

void Pantalla::AsignarNodo (int x, int y, int p, char codigo)
{
     int a =codigo;
	switch (toupper (codigo)) {
//Sin parametros
	case ID_NODO_INEX   : nodos [x][y] = new Nodo (x,y,NO_EXISTE); break;
	case ID_NODO 		: nodos [x][y] = new Nodo (x,y); break;
	case ID_NODO_AS 	: nodos [x][y] = new NodoAs (x,y); break;
	case ID_NODO_COM 	: nodos [x][y] = new NodoCom (x,y); Nivel.cx = x; Nivel.cy = y; break;
	case ID_NODO_VIDA 	: nodos [x][y] = new NodoVida (x,y); break;
//Con parametros
	case ID_NODO_EN 	: nodos [x][y] = new NodoEn (x,y,p); break;
	case ID_NODO_1PASO 	: nodos [x][y] = new Nodo1Paso (x,y,p); break;
	case ID_NODO_PUNTOS	: nodos [x][y] = new NodoPuntos (x,y,p); break;
	case ID_NODO_FIN 	: nodos [x][y] = new NodoFin (x,y); break;
     case ID_NODO_PILDTC : nodos [x][y] = new NodoPildTC (x,y,p); break;
     case ID_NODO_PILDMB : nodos [x][y] = new NodoPildMB (x,y,p); break;
	}
}

void Pantalla::AgregarNodoProxPag (int x, int y)
{
    ProxPag.Agregar (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y, nodos [x][y]->PuntMasc (), AND_PUT);
    ProxPag.Agregar (VerdX (x)-RADIO_NODO_X, VerdY (y)-RADIO_NODO_Y,nodos [x][y]->PuntImag (), OR_PUT);
}


void Pantalla::Grabar (char *nom)
{
    FILE *arch;
    int cont;
    arch = fopen (nom,"wb");
    fwrite (&Nivel, TAM_PANT, 1, arch);
    fclose (arch);
}

int Pantalla::Cargar (char *nom)
{
    int	c;
    FILE *arch;
    if ((arch = fopen (nom,"rb")) == NULL) return 0;
    fseek (arch, 0L, SEEK_SET);
    c = fread (&Nivel, TAM_PANT, 1, arch);
    fclose (arch);
    return c;
}

int Pantalla::Cargar ()
{
    int c;
    FILE *arch;
    if ((arch = fopen ("PANT.DAT","rb")) == NULL) return 0;
    fseek (arch, (long) (num-1) * TAM_PANT, SEEK_SET);
    c = fread (&Nivel, TAM_PANT, 1, arch);
    fclose (arch);
    return c;
}

int Pantalla::Existe (int x, int y)
{
     if (x >= 0 && x < NODOS_X && y >= 0 && y < NODOS_Y)
		return (nodos [x][y]->estado) == EXISTE;
     else return NO_EXISTE;
}

void Pantalla::Preparar (int n)
{
	int x,y, cont= 0;
     char aux [15];
	num = n;
     setactivepage (0);
     if (n >= 0) Cargar ();
     sprintf (aux, "F%d.ibp",(Nivel.Nro-256) / 512 /*Nivel.NroFondo*/);
   	CargarIBP (20,15,0,aux);
	for (y = 0; y < NODOS_Y; y++)
		for (x = 0; x < NODOS_X; x++)
		{
			AsignarNodo (x,y, Nivel.Info [cont] % 256,
						   Nivel.Info [cont] / 256);
			cont++;
		}

     for (cont = 0; cont < MAX_BICHOS; cont++)
     	if (cont < Nivel.CantBichos) Bichos [cont] = new Bicho (Nivel.Bichos [cont]);
          else Bichos [cont] = NULL;

     rot.PonerNivel (Nivel.Nro);
}


void Pantalla::ActuarNodos ()
{
	int conty, contx;
	for (conty = 0; conty < NODOS_Y; conty++)
		for (contx = 0; contx < NODOS_X; contx++)
			nodos [contx][conty]->Actuar ();
}

void Pantalla::LiberarMem ()
{
     int x, y, cont;
	for (y = 0; y < NODOS_Y; y++)
		for (x = 0; x < NODOS_X; x++)
               delete nodos [x][y];

     for (cont = 0; cont < Nivel.CantBichos; cont++)
     	delete Bichos [cont];

     ProxPag.LiberarMem ();
}

inline Result PonerRes (Result R, Result *ant)
{
	if (*ant == NADA) *ant = R;
}

Result Pantalla::Jugar ()
{
     np = 0;
	Result res = NADA;

     DibujarEntorno ();
	Preparar (num);

     rot.IniciarPant (Nivel.cx,Nivel.cy);
	DibujarNodos ();
     CopiarPagina (0);
     TiempoIn = clock ();
	do {
          Tiempo = (clock () - TiempoIn) / CLK_TCK;
          VerificarCronom ();
		setactivepage (np);
  		ProxPag.Limpiar ();
		np = 1-np;
		EsperarWR ();
		setvisualpage (np);
		EsperarWR ();
		rot.PonerInfo ();

if (kbhit ()) getch ();

          PonerRes (Borrar_ActuarBichos (), &res);
		PonerRes (rot.Mover (), &res);
          DibujarBichos ();
		ActuarNodos ();
		PonerRes (nodos [rot.x][rot.y]->Usar (rot.vel), &res);
		PonerRes (VerVecinos (rot.x, rot.y, rot.angulo), &res);
		nodos [rot.x][rot.y]->Dibujar ();
          if (res == PERDIO && rot.accion == NORMAL)
		{
			rot.PerderVida ();
          	Sonido (S_PERDIO_VIDA);
			rot.accion = PERDER_INICIAR;
          }
          if (res == PASO_NIVEL && rot.accion == NORMAL)
		{
			Sonido (S_APARECER);
			rot.accion = DESAPARECER_INICIAR;
          }
          if (rot.accion == PERDER_FIN) rot.accion = NORMAL;
          if (rot.accion == DESAPARECER_FIN) rot.accion = NORMAL;
	} while (res == NADA || rot.accion != NORMAL);

	if (res == PASO_NIVEL)
	{
	   num++;
	   rot.AumentarPuntos (1000);
        if (num > CantPant)
        {
             rot.AumentarPuntos (20000);
	        res= GANO;
        }
	}
	rot.CambiarAnts ();
     LiberarMem ();
	return res;
}

