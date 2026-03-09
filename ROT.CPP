#include "defin.hpp"
#include "nodos.hpp"
#include "pantalla.hpp"
#include "rot.hpp"
#include "sonido.hpp"
#include "multitec.hpp"
#include "sincos.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROT_ANCHO_X		14
#define ROT_ANCHO_Y		10
#define ROT_DIST_BASE     0
#define ROT_ALTO_X 		30
#define ROT_ALTO_Y 		23
#define ROT_VEL_ANGOST    2

#define VIDASX			 21
#define VIDASY           316
#define VIDASDX	 	108
#define VIDASDY	 	 23

#define PUNTOSX	     172
#define PUNTOSY          316
#define PUNTOSDX	 	 67
#define PUNTOSDY	 	 23

#define NIVELX		 	272
#define NIVELY           316
#define NIVELDX	 	 67
#define NIVELDY	 	 23

#define CFONDOINFO		 LIGHTGRAY
//int CFONDOINFO=0;

extern CProxPag ProxPag;
extern Pantalla pant;
extern int np;
extern int Tiempo;
extern clock_t TiempoIn;

/*ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
						  CLASE ROT
  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл*/
Rot::Rot ()
{
	fondo   = farmalloc (10000);
}

Rot::~Rot ()
{
	farfree (fondo);
}

void Rot::IniciarJuego ()
{
	vel 		   = 12;
	quiere_pasar = 0;
	puntos [0]   = 0;
	vidas [0]    = 3;
     accion 	   = NORMAL;
     PildMBActiva =0;
     CambiarAnts ();
}

void Rot::PonerPuntos ()
{
	char pun [7];
  	if (puntos [1] < 0) puntos [1] = 1;
	ltoa (puntos [0], pun, 10);
	setfillstyle (SOLID_FILL, CFONDOINFO);
	bar (PUNTOSX,PUNTOSY,PUNTOSX + PUNTOSDX, PUNTOSY + PUNTOSDY);
	setcolor (WHITE);
	settextjustify (RIGHT_TEXT, CENTER_TEXT);
	settextstyle (SMALL_FONT,0,6);
	outtextxy (PUNTOSX + PUNTOSDX - 4, PUNTOSY + 9, pun);
	if (puntos [1] >= 0) puntos [1] = -5;
	else puntos [1] = puntos [0];
}

void Rot::PonerNivel (int nivel)
{
     char cad [6];
     sprintf (cad, "%d.%d",nivel / 256, nivel % 256);
	setfillstyle (SOLID_FILL, CFONDOINFO);
	bar (NIVELX,NIVELY,NIVELX + NIVELDX, NIVELY + NIVELDY);
	setcolor (WHITE);
	settextjustify (CENTER_TEXT, TOP_TEXT);
	settextstyle (0,0,2);
	outtextxy (NIVELX + NIVELDX / 2, NIVELY + 5, cad);
}

void PonerVida (int nro)
{
	int  vx = nro * 20 + 15 + VIDASX, vy = 21 + VIDASY, poly [6];
  	poly [0] = vx;
     poly [1] = vy;
	poly [2] = vx - 5;
     poly [3] = vy;
     poly [4] = vx;
	poly [5] = vy - 20;
     setcolor (RED);
     setfillstyle (1,RED);
     fillpoly (3,poly);
	poly [2] = vx + 5;
     setcolor (WHITE);
     setfillstyle (1, WHITE);
     fillpoly (3,poly);
}

void Rot::PonerVidas ()
{
	int cont;
	setfillstyle (SOLID_FILL, CFONDOINFO);
	bar (VIDASX,VIDASY,VIDASX + VIDASDX, VIDASY + VIDASDY);

     if (vidas [0] > 5)
     {
          char txt [5] = "  x";
          if (vidas [0] > 9) txt [0] = vidas [0] / 10 + '0';
          txt [1] = vidas [0] % 10 + '0';
          setcolor (WHITE);
          settextstyle (0,0,2);
		settextjustify (LEFT_TEXT, TOP_TEXT);
          outtextxy (VIDASX + 10, VIDASY + 5,txt);
          PonerVida (3);
     }
     else
		for (cont = 0; cont < (vidas [0]); cont++)
          	PonerVida (cont);

	if (vidas [1] >= 0) vidas [1] = -5;
	else vidas [1] = vidas [0];
}


void Rot::PonerInfo ()
{
     if (puntos [0] != puntos [1]) PonerPuntos ();
     if (vidas [0] != vidas [1]) PonerVidas ();
}

void Rot::Dibujar ()
{
	float seno = SIN [angulo] / 256.0, coseno = COS [angulo] / 256.0;
	float cos2 = -seno, sen2 = coseno;
	int  vx = VerdX (x), vy = VerdY (y),
		ancho_x = ROT_ANCHO_X, ancho_y = ROT_ANCHO_Y,
          alto_x  = ROT_ALTO_X,  alto_y  = ROT_ALTO_Y;
	static int Auxiliar, Auxiliar2;
     switch (accion) {
	case PASAR_INICIAR :     Auxiliar = angulo % 180? ROT_ANCHO_Y : ROT_ANCHO_X;
          				accion++;
     case PASAR_ANGOSTAR :	ancho_x = ancho_y = Auxiliar;
                    		Auxiliar -= ROT_VEL_ANGOST;
     					if (Auxiliar <= 1) accion++;
                    		break;
     case PASAR :       		ancho_x = ancho_y = Auxiliar;
     					Auxiliar2 = angulo % 180? ROT_ANCHO_Y : ROT_ANCHO_X;
						accion++; break;
     case PASAR_ENSANCHAR : 	ancho_x = ancho_y = Auxiliar;
                    		Auxiliar += ROT_VEL_ANGOST;
     					if (Auxiliar >= Auxiliar2) accion = NORMAL;
                              break;
     case APARECER_INICIAR :  Auxiliar = 1; Sonido (S_APARECER);
						Auxiliar2 = angulo % 180? alto_y : alto_x;
						accion++;
     case APARECER_CRECER  :  alto_x = alto_y = ++Auxiliar;
     					if (Auxiliar >= Auxiliar2) {accion++;Auxiliar = 10;}
                              break;
     case APARECER_ESPERA  :  Auxiliar--;
						if (!Auxiliar) accion = NORMAL;
                              break;
     case DESAPARECER_INICIAR:
     case PERDER_INICIAR: 	Auxiliar = angulo % 180? alto_y : alto_x;
						 accion++;
     case DESAPARECER_ACHICAR:
     case PERDER_ACHICAR: 	alto_x = alto_y = --Auxiliar;
     					if (Auxiliar <= 0) accion++;
                              break;
     }

     vx = vx - ROT_DIST_BASE * coseno;
     vy = vy + ROT_DIST_BASE * seno;
	setcolor (LIGHTCYAN);
     int poly [6];
  	poly [0] = vx;
     poly [1] = vy;
	poly [2] = vx - ancho_x / 2 * cos2;
     poly [3] = vy + ancho_y / 2 * sen2;
     poly [4] = vx + (alto_x + ROT_DIST_BASE) * coseno;
	poly [5] = vy - (alto_y + ROT_DIST_BASE) * seno;
     setcolor (LIGHTGRAY);
     setfillstyle (1,LIGHTGRAY);
     fillpoly (3,poly);
     poly [0] = vx + ancho_x / 2 * cos2;
     poly [1] = vy - ancho_y / 2 * sen2;
	poly [2] = vx;
     poly [3] = vy;
     setcolor (WHITE);
     setfillstyle (1,/*quiere_pasar? LIGHTGREEN :*/ WHITE);
     fillpoly (3,poly);
}

void Rot::Borrar ()
{
	int dx = RADIO_NODO_X+1,
		dy = RADIO_NODO_Y+1;
	putimage (VerdX (x) + dx - DIST_NODOX, VerdY (y) + dy - DIST_NODOY, fondo,0);
}

int Rot::PosibleChoque (int xp, int yp)
{
	return ((xp >= VerdX (x-1)) && (xp <= VerdX (x+1)) &&
     	   (yp >= VerdY (y-1)) && (yp <= VerdY (y+1)));
}

int Rot::PuntoAdentro (int x2, int y2)
{
	int x0, y0, x3, y3, DPar, DPerp, aux;
     float  p1, p2;
     static float a, b;
     static int x1, y1, angant;
     if (abs (angulo % 90) < 5) return 0;
     x0 = VerdX (x);
	y0 = VerdY (y);
     if (angulo != angant)
     {
    		x1 = x0 + ROT_ALTO_X * COS [angulo] / 256;
		y1 = y0 - ROT_ALTO_Y * SIN [angulo] / 256;
          if (x0==x1) x1--;	// Para evitar divisiones por 0
          if (y0==y1) y1--;
     	a = (float) (y0 - y1) / (x0 - x1);
     	b = (float) y1 - a * (float) x1;
          angant = angulo;
     }
     x3 = (y2 + x2 / a - b)*(a / (a * a + 1));
     y3 = a * x3 + b;
     if (x3 < MIN (x0, x1) || x3 > MAX (x0,x1)) return 0;
     DPar  = hypot (x1 - x3, y1 - y3);
     DPerp = hypot (x2 - x3, y2 - y3);
     if (DPerp) p1 = (float) DPar / (float) DPerp;
     else p1 = 1000;
	p2 = (float) ROT_ALTO_X / (float) (ROT_ANCHO_X / 2.0);
     aux = p1 >= p2;
     return aux;
}

void Rot::IniciarPant (int cx, int cy)
{
  	int dx = RADIO_NODO_X+1, dy = RADIO_NODO_Y+1;
     getimage (VerdX (cx) + dx - DIST_NODOX, VerdY (cy) + dy - DIST_NODOY,
			VerdX (cx) - dx + DIST_NODOX, VerdY (cy) - dy + DIST_NODOY, fondo);
     angulo = 0;
     accion = APARECER_INICIAR;
	UbicarEn (cx,cy);
	PonerInfo ();
}

int PulsoEsc ()
{
     clock_t TCom, TFin;
     int ancho = 200, alto = 100;
     char t;
     void *fondo;
	TCom = clock ();
     setactivepage (np);
	OcultarPant (np);
     fondo = farmalloc (imagesize (0, 0,ancho+2, alto+2));
     getimage (316 - ancho / 2, 150 - alto / 2, 316 + ancho / 2, 150 + alto / 2, fondo);
     CargarIBP (316 - ancho / 2, 150 - alto / 2, np, IBP_SALIR);
     do {
        t = TeclaPulsada ();
     } while ((t !=31) && (t != 49));
     putimage (316 - ancho / 2,150 - alto / 2, fondo, COPY_PUT);
     setactivepage (1-np);
     farfree (fondo);
     DesocultarPant (np);
     TFin = clock ();
     TiempoIn += TFin - TCom;
     return t == 49? 0 : 1;
}

Result Rot::Mover ()
{
	int dx = RADIO_NODO_X+1,
		dy = RADIO_NODO_Y+1;
     static int xx, yy, CantLlam = -10, T=Tiempo+3;

	if (quiere_pasar && accion == NORMAL)
	{
	    xx = x; yy = y;
	    if (abs (angulo-0)   <= abs (vel)) xx++;
	    if (abs (angulo-90)  <= abs (vel)) yy--;
	    if (abs (angulo-180) <= abs (vel)) xx--;
	    if (abs (angulo-270) <= abs (vel)) yy++;
	    if (((y != yy || x != xx)) && pant.Existe (xx,yy))
	    	   if (pant.AutorizarSalida (x,y,angulo) &&
		   	  pant.AutorizarLlegada (xx,yy, angulo))
             {
                    angulo = (angulo + abs (vel)) / 90 * 90;
     		     accion = PASAR_INICIAR;
          		Sonido (S_PASO_NODO);
	        }
	}
	Borrar ();
	Dibujar ();
     if (accion == PASAR)
     {
     			ProxPag.Agregar (VerdX (x) + dx - DIST_NODOX, VerdY (y) + dy - DIST_NODOY, fondo, COPY_PUT);
          		Borrar ();
          		pant.DibujarNodo (x,y);
                    pant.AgregarNodoProxPag (x,y);
 			     pant.SalirNodo (x,y);
			     angulo = (angulo + 540) % 360;
			     x = xx;
			     y = yy;
				pant.nodos [x][y]->Borrar ();
			     quiere_pasar = 0;
          		getimage (VerdX (x) + dx - DIST_NODOX, VerdY (y) + dy - DIST_NODOY,
						VerdX (x) - dx + DIST_NODOX, VerdY (y) - dy + DIST_NODOY, fondo);
     }
     if (accion != NORMAL) return NADA;
	angulo += vel + 360;
	angulo %= 360;
	unsigned char p60 = TeclaPulsada ();
	if (p60 == T_IZQ) { vel = abs (vel); quiere_pasar = 0;}
	if (p60 == T_DER) { vel = -abs (vel);quiere_pasar = 0; }
	if (p60 == 1) p60 = PulsoEsc ();
	if (Presionada [T_BARRA] && !quiere_pasar)
	    quiere_pasar = AngPar (angulo, 0)   || AngPar (angulo, 90) ||
				    AngPar (angulo, 180) || AngPar (angulo, 270);
     #ifdef DEBUG
     if (p60 == 0x3B) return PASO_NIVEL;      // F1= pasar de nivel
     if (p60 == 0x3C) AumentarPuntos (10000); // F2= 10000 puntos mas
     if (p60 == 0x3D) GanarVida ();		 // F3= Gana una vida
	if (p60 == 0x3E) return PERDIO;
     #endif

	while (kbhit ()) getch ();
	if (p60 == 1) return PULSO_ESC;
	return NADA;
}
