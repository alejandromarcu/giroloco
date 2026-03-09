#include "defin.hpp"
#include "multitec.hpp"
#include "sonido.hpp"
#include "menu.hpp"
#include <conio.h>
#include <stdio.h>

extern CRecords Records;
extern int SonidoActivo;
extern int NivelInicial;

void inline LineaH (int x, int y, int xx) { line (x, y, xx, y); };
void inline LineaV (int x, int y, int yy) { line (x,y,x,yy);    };

SConfig config;

/*ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
				Funciones graficas para el menu
  ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ*/

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  PintarArea
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void PintarArea (int X, int Y, int XX, int YY, int Col)
{
     setfillstyle (SOLID_FILL, Col);
     bar (X, Y, XX, YY);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  TextoCent
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void TextoCent (int X, int Y, char *S, int Col)
{
     setcolor (Col);
     settextjustify (CENTER_TEXT, CENTER_TEXT);
     outtextxy (X,Y,S);
     settextjustify (LEFT_TEXT, TOP_TEXT);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  Relieve
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void Relieve (int X,int Y,int XX,int YY, int Z, int Col1, int Col2)
{
     int Ancho = 0, Alto = 0, Aux;
     for (Aux = 1; Aux < Z; Aux++)
     {
	  Ancho++; Alto++;
	  setcolor (Col1);
	  LineaH (X-Ancho,Y-Alto,XX+Ancho);
	  LineaV (X-Ancho,Y-Alto,YY+Alto);
	  setcolor (Col2);
	  LineaV (XX+Ancho,Y-Alto,YY+Alto);
	  LineaH (X-Ancho,YY+Alto,XX+Ancho);
     };
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  Boton
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void Boton (int X,int Y,int XX,int Sel, char *Titulo)
{
	PintarArea (X, Y, XX, Y + 12, Sel == OPCION_SEL? CYAN : LIGHTGRAY);
     Relieve (X, Y, XX, Y + 12, 2, Sel == OPCION_SEL? DARKGRAY : WHITE, Sel == OPCION_SEL? WHITE : DARKGRAY);
     TextoCent ((X + XX) / 2, Y + 7, Titulo, Sel == OPCION_SEL? YELLOW : LIGHTGREEN);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  Area
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void Area (int X, int Y,int XX,int YY, char *Titulo)
{
     int TH;
     X++; Y++;
     TH      = textheight (Titulo);
     PintarArea (X, Y, XX, YY, LIGHTGRAY);
     Relieve (X,Y,XX,YY,3, WHITE, DARKGRAY);
     if (Titulo [0])
     {
	  PintarArea (X + 4, Y + 4,XX - 2, Y + TH +10, LIGHTBLUE);
	  Relieve (X+4, Y+4, XX-4, Y+TH+10, 3, WHITE, DARKGRAY);
	  TextoCent ((X + XX) / 2, Y + TH + 2, Titulo, WHITE);
     }
}

/*ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
				Funciones para la configuracion
  ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ*/
void CargarConfig ()
{
	FILE *arch;
     arch = fopen ("config.dat","rb");
     if (arch == NULL)
     {
		config.SonAct = 0;
		config.MaxNivel = 1;
          return;
     }
     fread (&config,sizeof (config),1,arch);
     fclose (arch);
}

void GrabarConfig ()
{
	FILE *arch;
     arch = fopen ("config.dat","wb");
     fwrite (&config,sizeof (config),1,arch);
     fclose (arch);
}


/*ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
						  CLASE CMEMU
  ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ*/
void CMenu::CambiarOp (int nro, char *txt)
{
	strcpy (Opc [nro],txt);
     EscribirOp (nro, OPCION_SEL);
}

void CMenu::EscribirOp (int nro, int modo)
{
     Boton (250, nro * 120 / CantOp + 110, 390, modo, Opc [nro]);
}

void CMenu::Dibujar (char *Tit)
{
	int cont;
     settextstyle (DEFAULT_FONT, HORIZ_DIR,1);
	Area (240, 80, 400, 230, Tit);
     for (cont = 0; cont < CantOp; cont++)
     	EscribirOp (cont, cont == OpAct? OPCION_SEL : OPCION_NO_SEL);
}

int CMenu::Menu ()
{
     unsigned char tec, tec2;
     do {
     	tec = getch ();
          if (!tec) tec2 = getch ();
          else tec2 = 0;
          if (tec2 == T_ARR || tec2 == T_ABA)
          {
               EscribirOp (OpAct,OPCION_NO_SEL);
          	OpAct += (tec2 == T_ABA) - (tec2 == T_ARR);
               OpAct = (OpAct + CantOp) % CantOp;
               EscribirOp (OpAct,OPCION_SEL);
          }
     } while (tec != 13);
     return OpAct;
}

/*ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
						  CLASE CRECORD
  ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ*/
void CRecords::Encriptar ()
{
     int cont;
     unsigned char *punt;
     punt = (char *) Rec;
     for (cont = 0; cont < (sizeof (SRecords)*MAX_RECORDS); cont++, punt++)
     	*punt = 255 - *punt;
}

void CRecords::Cargar ()
{
	FILE *arch;
     arch = fopen ("tr.dat","rb");
     if (!arch)
     {
     	CantRec=0;
          return;
     }
     fread (&CantRec, sizeof (CantRec),1, arch);
     fread (Rec, sizeof (SRecords),MAX_RECORDS, arch);
     fclose (arch);
	Encriptar ();
}

void CRecords::Grabar ()
{
	FILE *arch;
     arch = fopen ("tr.dat","wb");
     fwrite (&CantRec, sizeof (CantRec),1, arch);
	Encriptar ();
     fwrite (Rec, sizeof (SRecords),MAX_RECORDS, arch);
	Encriptar ();
     fclose (arch);
}

void CRecords::Tabla ()
{
     int cont;
     char aux [50];
     cleardevice ();
     CargarIBP (0,0,0,IBP_TABLAREC);
     settextstyle (0,0,2);
     settextjustify (LEFT_TEXT, TOP_TEXT);
     settextstyle (2,0,6);
     for (cont = 0; cont <CantRec; cont++)
     {
    		setcolor (cont? BLUE : LIGHTBLUE);
          outtextxy (190, cont * 18 + 82,Rec [cont].Nombre);
     	sprintf (aux,"%8ld",Rec [cont].Puntos);
          outtextxy (320, cont * 18 + 82, aux);
     	sprintf (aux,"%d.%d", Rec [cont].Nivel / 256,Rec [cont].Nivel % 256);
          outtextxy (440, cont * 18 + 82, aux);
     }
	LeeTecla ();
}

void PedirNombre (char *Nom)
{
     int pos = 0;
     char tec;
	char aux [MAX_LEN_RECORDS+2];
     CargarIBP (200, 80,0, IBP_PIDENOM);
     memset (aux, ' ', MAX_LEN_RECORDS);
     aux [MAX_LEN_RECORDS] = 0;
     setfillstyle (1,LIGHTGRAY);
     settextstyle (0,0,2);
     settextjustify (LEFT_TEXT, TOP_TEXT);
     setcolor (BLACK);
     do {
          aux [pos] = 'Ы';
		bar (228, 198, 403, 214);
          outtextxy (228, 199, aux);
     	tec = getch ();
          if (tec == 27) continue;
          if (!tec)
          {
          	getch ();
               continue;
          }
          if (tec == '\b')
		{
			if (pos) aux [pos--] =' ';
               continue;
          }
          if ((pos < (MAX_LEN_RECORDS - 1)) && (tec != 13))
		aux [pos++] = tec;

     } while (tec != 13);
     aux [pos] = 0;
     strcpy (Nom, aux);
}

void CRecords::Agregar (long puntos, int nivel)
{
     int c1,c2, posmax;
     long maxp;
     if ((nivel / 256) > config.MaxNivel) config.MaxNivel = nivel / 256;
     if ((CantRec == MAX_RECORDS) && (puntos < Rec [CantRec-1].Puntos)) return;
     if (CantRec != MAX_RECORDS) CantRec++;
     setvisualpage (0);
     setactivepage (0);
     SRecords RecAux [MAX_RECORDS];
     memcpy (RecAux, Rec, (CantRec-1) * sizeof (SRecords));
 	PedirNombre (RecAux [CantRec-1].Nombre);

     RecAux [CantRec-1].Puntos = puntos;
     RecAux [CantRec-1].Nivel = nivel;
     for (c1 = 0; c1 < CantRec; c1++)
     {
          maxp = -1;
     	for (c2 = 0; c2 < CantRec; c2++)
     		if (RecAux [c2].Puntos > maxp)
			{
                    maxp = RecAux [c2].Puntos;
                    posmax = c2;
               }
          Rec [c1] = RecAux [posmax];
		RecAux [posmax].Puntos = -1;
     }
	Grabar ();
	Tabla ();
}

void AcercaDe ()
{
	CargarIBP (0,0,0,IBP_ACERCADE);
	settextstyle (DEFAULT_FONT, HORIZ_DIR, 1);
	settextjustify (LEFT_TEXT, TOP_TEXT);

	setfillstyle (SOLID_FILL, BLACK);
	bar (25, 85, 305, 225);

	setfillstyle (SOLID_FILL, BROWN);
	bar (20, 80, 300, 220);


	setcolor (YELLOW);
	outtextxy (40,90, "IDEA, DISEҐO Y PROGRAMACION:");
	setcolor (WHITE);
	outtextxy (100,110, "Alejandro Marcu");

	setcolor (YELLOW);
	outtextxy (40,140, "COLABORARON:");
	setcolor (WHITE);
	outtextxy (100,160, "Edgardo Garelik");
	outtextxy (100,175, "Sebasti n Sejzer");
	outtextxy (100,190, "Pablo Zimmerman");
	outtextxy (100,205, "Alejandro Zylberman");

  //	setcolor (YELLOW);

	setfillstyle (SOLID_FILL, BLACK);
	bar (325, 85, 605, 225);

	setfillstyle (SOLID_FILL, BLUE);
	bar (320, 80, 600, 220);


	setcolor (WHITE);
	outtextxy (380, 100,  "    Giroloco 1997   ");
	outtextxy (380, 120, "       Copyright");

	setcolor (LIGHTGRAY);
	outtextxy (340, 150, "Este paquete incluye:");
	outtextxy (340, 170, "                     Memotest");
	outtextxy (340, 185, "                     Masco");
	outtextxy (340, 200, "                     Bolonki");

	LeeTecla ();
}

void SobreCaiman ()
{
	CargarIBP (0,0,0,IBP_ACERCADE);
	settextstyle (DEFAULT_FONT, HORIZ_DIR, 1);
	settextjustify (LEFT_TEXT, TOP_TEXT);
	setcolor (WHITE);
	outtextxy (15,80,  "CAIMAN C.O. es una empresa que se desarrolla de soft nacional");
	outtextxy (15,98 , "ya sea dedicado al  mbito hogare¤o o profesional.");
	outtextxy (15,128, "En cada uno de nuestros productos se busca la m s alta calidad al");
	outtextxy (15,146, "mejor precio para el pЈblico, para facilitarle el acceso a nuestra");
	outtextxy (15,164, "linea de soft nacional e incrementar la utilidad de su ordenador.");
	outtextxy (15,194, "Su inquietud o propuesta nos permitir  un mejor servicio.");
	outtextxy (15,212, "Escriba a: Casilla de correo 26 (1422) Suc 22 o E-Mail CAIMAN_CO@HOTMAIL.COM");
	outtextxy (15,230, "Servicio t‚cnico telefўnico de lunes a viernes de 8 a 13 hs al 856-8072");

	LeeTecla ();
}


int MenuPrincipal ()
{
	int op=-1;
	char aux [40];
	CMenu Menu;
	CargarIBP (0,0,0,IBP_FONDOMEN);
	setvisualpage (0);
     setactivepage (0);
     Menu.AgregarOp ("Comenzar");
     sprintf (aux, "Desde: %d.1", NivelInicial);
	Menu.AgregarOp (aux);
	sprintf (aux, "Sonido: %s", config.SonAct? "SI" : "NO");
	Menu.AgregarOp (aux);
	Menu.AgregarOp ("Records");
	Menu.AgregarOp ("Acerca de...");
	Menu.AgregarOp ("Sobre Caim n");
	Menu.AgregarOp ("Salir");
	do {
		if (op != 1) Menu.Dibujar ("Menu Principal");
		op = Menu.Menu ();
		switch (op) {
		case 1: NivelInicial++;
			   if (NivelInicial > config.MaxNivel) NivelInicial = 1;
			   sprintf (aux, "Desde: %d.1", NivelInicial);
			   Menu.CambiarOp (op,aux);
			   break;
		case 2: config.SonAct = !config.SonAct;
			   sprintf (aux, "Sonido: %s", config.SonAct? "SI" : "NO");
			   Menu.CambiarOp (op,aux);
			   break;
          case 3: Records.Tabla ();
     		   CargarIBP (0,0,0,IBP_FONDOMEN);break;
          case 4: AcercaDe ();
     		   CargarIBP (0,0,0,IBP_FONDOMEN);break;
          case 5: SobreCaiman ();
     		   CargarIBP (0,0,0,IBP_FONDOMEN);break;
          }
     } while ((op != 0) && (op != 6));
	return op == 0;
}