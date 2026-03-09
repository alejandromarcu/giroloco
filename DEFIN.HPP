#ifndef DEFIN
#include "funcom.hpp"

#include <alloc.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <graphics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define GRAD_RAD 0.0174529388

#define MIN(X,Y) (X < Y? X : Y)
#define MAX(X,Y) (X > Y? X : Y)

int const DIST_NODOX 	= 40,
		DIST_NODOY 	= 30,
		MARGEN_X	 	= 60,
		MARGEN_Y   	= 45,
		NODOS_X    	= 14,
		NODOS_Y    	= 8,
		RADIO_NODO_X 	= 7,
		RADIO_NODO_Y   = 5,
		MEMORIA_NODO   = 94, // Bytes ocupados en memoria para alocar c/nodo
		ENERGIA_DEST 	= -180;

// Definiciones de limites, tamanos, etc
#define TAM_PANT 		512
#define MAX_REST_PP 	512
#define MAX_BICHOS       20


//#define DEBUG

typedef unsigned char byte;
typedef int IntAnt [2];
typedef long LongAnt [2];

enum Estados  {NO_EXISTE = 0, NO_ENERGIA, EXISTE};
enum Result {NADA = 0, PERDIO, PASO_NIVEL, PULSO_ESC, GANO};

int inline VerdX (int x) { return x * DIST_NODOX + MARGEN_X; };
int inline VerdY (int y) { return y * DIST_NODOY + MARGEN_Y; };
float inline FalsoX (int x) { return (x - MARGEN_X) / DIST_NODOX; };
float inline FalsoY (int y) { return (y - MARGEN_Y) / DIST_NODOY; };
int inline AngPar (int a1, int a2) {return abs (a1 - a2) < 30;};

#endif
#define DEFIN
