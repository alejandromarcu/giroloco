#ifndef BICHOS
#include "defin.hpp"

//#define MAX_BICHOS 10
#define TIPO_BICHOS 4

#define B_SAncho 10
#define B_SAlto  10

typedef struct {
		byte tipo, inix, iniy;
          int  intel,taparic, duracion;
          } SInfoBicho;

class Bicho {
	protected:
		int NumIm, contCuad;
	public:
          int  	x, y,dx,dy, TCambioCuad;
          float	xreal, yreal, vel;
          char 	existe, activo;
     	void 	far *Fondo;
          SInfoBicho info;
		Bicho (SInfoBicho i);
          ~Bicho ();
          Result 	VerificarROT ();
		Result 	Actuar ();
          void 	LeerFondo ();
          void 	Dibujar ();
          void 	Borrar ();
          void		BorrarProxPag ();
          void 	VerifTiempo ();
     	int 		Orientacion ();
		};

extern Bicho *Bichos [MAX_BICHOS];
Result Borrar_ActuarBichos ();
void DibujarBichos ();
void InicBichos ();

//extern void CargarPUN (void *P [], char *narch)


#define BICHOS
#endif
