#ifndef MENU

#include "defin.hpp"
#include "multitec.hpp"
#include "sonido.hpp"
#include <conio.h>
#include <stdio.h>

#define MAX_OPC     10
#define MAX_LEN_OPC 40

#define MAX_RECORDS     10
#define MAX_LEN_RECORDS 11

#define OPCION_SEL    1
#define OPCION_NO_SEL 2


class CMenu {
			int CantOp, OpAct;
               char Opc [MAX_OPC][MAX_LEN_OPC];

			void EscribirOp (int nro, int modo);
		  public:
               CMenu () { CantOp = 0; OpAct = 0;};
		  	void AgregarOp (char *txt) { strcpy (Opc [CantOp], txt); CantOp++;};
               void CambiarOp (int nro, char *txt);
               void Dibujar (char *Tit);
               int  Menu ();
		};

typedef struct SRecords {
					char Nombre [MAX_LEN_RECORDS];
                         long Puntos;
                         int 	Nivel;
                         };

typedef struct SConfig  {
					   char DMA, SonAct;
                            int Base;
                            int MaxNivel;
					};

class CRecords {
			unsigned char CantRec;
               SRecords Rec [MAX_RECORDS];

               void Encriptar ();
            	void Grabar ();
		  public:
               void Cargar ();
               void Tabla ();
               void Agregar (long puntos, int nivel);
		};

void CargarConfig ();
void GrabarConfig ();

#define MENU
#endif