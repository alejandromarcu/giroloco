#ifndef NODOS
#include "defin.hpp"

int const INODO    			= 1,
		INODO_AS 			= 2,
		INODO_EN 			= 3,
		INODO_EN_SIN_EN	= 16,
		INODO_COMIENZO		= 24,
		INODO_FINAL		= 25,
		INODO_1PASO		= 32,
		INODO_VIDA		= 36,
		INODO_PUNTOS		= 37,
          INODO_PILDTC	     = 38,
          INODO_PILDMB		= 39;

char const
          ID_NODO_INEX 		= ' ',
		ID_NODO			= 'A',
		ID_NODO_AS		= 'B',
		ID_NODO_COM		= 'C',
		ID_NODO_VIDA		= 'D',
		ID_NODO_FIN		= 'F',

		ID_NODO_EN		= 'K',
		ID_NODO_1PASO		= 'L',
		ID_NODO_PUNTOS		= 'M',
          ID_NODO_PILDMB		= 'N',
          ID_NODO_PILDTC		= 'O';

class Nodo {
	protected:
		byte x, y, NumIm;
	public:
          void far *Fondo;
		Estados estado;
		Nodo (byte cx, byte cy) { x = cx; y = cy; estado = EXISTE; NumIm = INODO; Fondo = NULL;}
		Nodo (byte cx, byte cy, byte p2) { x = cx; y = cy; estado = NO_EXISTE; NumIm = 0; Fondo = NULL;}
          ~Nodo () { delete Fondo;}
		virtual unsigned int ID () {return (unsigned int) estado == EXISTE? (ID_NODO << 8) : (ID_NODO_INEX << 8); };
		virtual void Dibujar ();
		virtual void DibujarProxPag ();
		virtual void Borrar ();
		virtual void BorrarProxPag ();
		virtual void *PuntImag ();
		virtual void *PuntMasc ();
		virtual void GuardarFondo ();
		virtual void GuardarFondo_Dibujar ();
		virtual void Salio () {};
		virtual void Actuar () {};
		virtual Result Acercarse (int Angulo) {return NADA; };
		virtual Result Usar (int Angulo) {return estado == EXISTE? NADA : PERDIO;};
		virtual int AutorizarSalida (int Angulo) {return 1;};
		virtual int AutorizarLlegada (int Angulo) {return 1;};
		 };

class NodoCom : public Nodo {
	public:
		NodoCom (byte cx, byte cy) : Nodo (cx,cy) { NumIm = INODO_COMIENZO;};
		virtual unsigned int ID () {return (unsigned int) (ID_NODO_COM << 8); };
		};

class NodoFin : public Nodo {
		 int cuadro, dir;
	public:
	 NodoFin (byte cx, byte cy) : Nodo (cx,cy) { NumIm = INODO_FINAL; cuadro = 0; dir = 1;};
		 virtual unsigned int ID () {return (unsigned int) (ID_NODO_FIN << 8); };
		 virtual Result Usar (int Angulo) {return PASO_NIVEL;};
		 virtual void Actuar ();
		};

class NodoEn : public Nodo {
		 int energia, cuadro;
	 public:
		 NodoEn (byte cx, byte cy, int e) : Nodo (cx,cy) {energia = e * 180; NumIm = INODO_EN;cuadro = 0;}
		 virtual unsigned int ID () {return (unsigned int) (ID_NODO_EN << 8) + energia / 180; };
		 virtual void Dibujar ();
		 virtual void Salio () {};
		 virtual void Actuar ();
		 virtual Result Usar (int ang);
		 };

class NodoAs : public Nodo {
	 public:
		 NodoAs (byte cx, byte cy) : Nodo (cx,cy) {NumIm = INODO_AS;};
		 virtual unsigned int ID () {return (unsigned int) (ID_NODO_AS << 8); };
		 virtual int AutorizarLlegada (int Angulo) {return 0;};
		 virtual Result Acercarse (int Angulo);
		 virtual Result Usar (int Angulo) {return PERDIO;};
		 };

class Nodo1Paso : public Nodo {
		 int dirVal;
	 public:
		 Nodo1Paso (byte cx, byte cy, int d) : Nodo (cx,cy) {NumIm = INODO_1PASO + d - 1;dirVal = d;};
		 virtual unsigned int ID () {return (unsigned int) (ID_NODO_1PASO << 8) + dirVal; };
		 virtual int AutorizarSalida (int Angulo) {return abs (Angulo - (dirVal -1)* 90) < 15;};
		 };

class NodoVida : public Nodo {
		 int Usada;
	 public:
		 NodoVida (byte cx, byte cy) : Nodo (cx,cy) {NumIm = INODO_VIDA;Usada = 0;};
		 virtual unsigned int ID () {return (unsigned int) (ID_NODO_VIDA << 8); };
		 virtual Result Usar (int Angulo);
		 };

class NodoPuntos : public Nodo {
		 int Puntos;
	 public:
		 NodoPuntos (byte cx, byte cy,int p) : Nodo (cx,cy) {NumIm = INODO_PUNTOS;Puntos = p * 100;};
		 virtual unsigned int ID () {return (unsigned int) (ID_NODO_PUNTOS << 8) + Puntos / 100; };
		 virtual Result Usar (int Angulo);
		 };

class NodoPildTC : public Nodo {
		 int Durac,TFinal, Usado;
		 Nodo *nodosAnt [NODOS_X][NODOS_Y];
	 public:
		 NodoPildTC (byte cx, byte cy,int p) : Nodo (cx,cy) {NumIm = INODO_PILDTC;Durac = p; Usado=0;};
		 virtual unsigned int ID () {return (unsigned int) (ID_NODO_PILDTC << 8) + Durac; };
		 virtual Result Usar (int Angulo);
		 virtual void Actuar ();
		 };

class NodoPildMB : public Nodo {
		 int Durac,TFinal, Usado;
	 public:
		 NodoPildMB (byte cx, byte cy,int p) : Nodo (cx,cy) {NumIm = INODO_PILDMB;Durac = p; Usado=0;};
		 virtual unsigned int ID () {return (unsigned int) (ID_NODO_PILDMB << 8) + Durac; };
		 virtual Result Usar (int Angulo);
		 virtual void Actuar ();
		 };

void CargarImNodos ();

#define NODOS
#endif
