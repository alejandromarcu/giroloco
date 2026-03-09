#ifndef PANTALLA
#include "defin.hpp"
#include "nodos.hpp"
#include "bichos.hpp"

#define TAM_PANT 512

typedef struct {
			unsigned int    Info [NODOS_X * NODOS_Y];
               byte			 CantBichos;
               SInfoBicho      Bichos [MAX_BICHOS];
               byte			 cx, cy;			// Coordenadas del comienzo
               unsigned int	 Nro;			// Nro de nivel
               byte			 NroFondo;
			} SNivel;

class Pantalla {
	public:
		int num;
		Nodo *nodos [NODOS_X][NODOS_Y];
          union {
			SNivel Nivel;
               char p2 [TAM_PANT];
          };
		void IniciarJuego ();
          void PonerTiempo (int porc);
		void FinMedicionTiempo ();
		void Grabar (char *nom);
		int  Cargar ();
		int  Cargar (char *nom);
		void ActuarNodos ();
		void DibujarNodos ();
		void AsignarNodo (int x, int y, int p, char codigo);
          void AgregarNodoProxPag (int x, int y);
		void DibujarEntorno ();
		void Preparar (int n);
		void CambiarPantalla (int x, int y, int id) { Nivel.Info [x + y * NODOS_X] = id; }
          void LiberarMem ();
		Result VerVecinos (int x, int y, int a);
		Result Jugar ();

          void DibujarNodo (int x, int y) { nodos [x][y]->Dibujar ();}
		void SalirNodo (byte x, byte y) { nodos [x][y]->Salio (); }
		int Existe (int x, int y);
		int AutorizarSalida (int x, int y, int Ang) { return nodos [x][y]->AutorizarSalida (Ang);}
		int AutorizarLlegada (int x, int y, int Ang) { return nodos [x][y]->AutorizarLlegada (Ang);}
			};

typedef struct {
		unsigned int x, y, tam;
          char modo;
          char *i;
          } SImag;

class CProxPag {
		int cant;
          SImag Imag [MAX_REST_PP];
        public:
          CProxPag () {cant = 0;}
          void Agregar (int px, int py, void *pi, char modo);
          void AgregarBar (int x, int y, int xx, int yy, char color);
		void Limpiar ();
          void LiberarMem ();
			};


void BarProxPag (int x, int y, int xx, int yy, char color);

#endif
#define PANTALLA