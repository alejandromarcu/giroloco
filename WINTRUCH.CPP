#include <defin.hpp>

int
    COLOR_AREA        = LIGHTGRAY,
    COLOR_AREA_REL1   = WHITE,
    COLOR_AREA_REL2   = DARKGRAY,
    COLOR_SAREA	      = GRAY,
    COLOR_SAREA_REL1  = LIGHTGRAY,
    COLOR_SAREA_REL2  = WHITE,
    COLOR_FONDO_TIT   = LIGHTBLUE,
    COLOR_TIT	      = WHITE,
    COLOR_TEXTO_BOTON = YELLOW,

    BP_X	      = 20,
    BP_Y	      = 4,
    BP_XX             = 60,
    BP_YY	      = 20;

char ega_cols [16]= {0,1,2,3,4,5,7,20,56,57,58,59,60,61,62,52};

void *Fondo;

void inline LineaH (int x, int y, int xx) { line (x, y, xx, y); };
void inline LineaV (int x, int y, int yy) { line (x,y,x,yy);    };

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  PintarArea
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void PintarArea (int X, int Y, int XX, int YY, int Col)
{
     setfillstyle (SOLID_FILL, Col);
     bar (X, Y, XX, YY);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  TextoCentrado
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void TextoCentrado (int X, int Y, char *S, int Col)
{
     setcolor (Col);
     settextjustify (CENTER_TEXT, CENTER_TEXT);
     HideCursor ();
     outtextxy (X,Y,S);
     ShowCursor ();
     settextjustify (LEFT_TEXT, TOP_TEXT);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  TextoCent
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void TextoCent (int X, int Y, char *S, int Col)
{
     X = VerdX (X); Y = VerdY (Y);
     TextoCentrado (X, Y, S, Col);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  TextoNoC
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
inline void TextoNoC (int X, int Y, char *S, int Col)
{
     X = VerdX (X); Y = VerdY (Y);
     setcolor (Col);
     outtextxy (X,Y,S);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  Texto
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void Texto (int X, int Y, char *S, int Col)
{
     HideCursor ();
     TextoNoC (X, Y, S, Col);
     ShowCursor ();
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  TextoNumNoC
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
inline void TextoNumNoC (int X, int Y, long n, int Col, int nd)
{
     char S [11], S2 [11] = "", cont;
     X = VerdX (X); Y = VerdY (Y);
     ltoa (n, S, 10);
     if (nd)
     {
	 for (cont = 0; cont < nd - strlen (S);cont++)
	     strcat (S2, "0");
	 strcat (S2, S);
	 strcpy (S, S2);
     }
     setcolor (Col);
     settextjustify (RIGHT_TEXT, TOP_TEXT);
     outtextxy (X,Y,S);
     settextjustify (LEFT_TEXT, TOP_TEXT);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  TextoNum
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void TextoNum (int X, int Y, long n, int Col, int nd)
{
     HideCursor ();
     TextoNumNoC (X, Y, n, Col, nd);
     ShowCursor ();
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  TextoBorra
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void TextoBorra (int X, int Y, char *S, int Col, int ColFondo)
{
     X = VerdX (X); Y = VerdY (Y);
     setcolor (Col);
     HideCursor ();
     PintarArea (X, Y, X+ textwidth (S), Y + textheight (S), ColFondo);
     outtextxy (X,Y,S);
     ShowCursor ();
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
				  Area
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void Area (int X, int Y,int XX,int YY, int Z, char *Titulo)
{
     X = VerdX (X); XX = VerdX (XX);
     Y = VerdY (Y); YY = VerdY (YY);
     int TH;
     X++; Y++;
     TH      = textheight (Titulo);
     HideCursor ();
     PintarArea (X, Y, XX, YY, COLOR_AREA);
     Relieve (X,Y,XX,YY,Z, COLOR_AREA_REL1, COLOR_AREA_REL2);
     if (Titulo [0])
     {
	  PintarArea (X + 4, Y + 4,XX - 2, Y + TH +12, COLOR_FONDO_TIT);
	  Relieve (X+4, Y+4, XX-4, Y+TH+12, 3, COLOR_AREA_REL1, COLOR_AREA_REL2);
	  TextoCentrado ((X + XX) / 2, Y + TH + 4, Titulo, COLOR_TIT);
     };
     ShowCursor ();
}
/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  Area_Sub
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void Area_Sub (int X,int Y,int XX,int YY, int Z)
{
     X = VerdX (X); XX = VerdX (XX);
     Y = VerdY (Y); YY = VerdY (YY);
     int ContX, ContY;
     X++; Y++;
     HideCursor ();
     Relieve (X,Y,XX,YY,Z, COLOR_SAREA_REL1, COLOR_SAREA_REL2);
     for (ContY = Y; ContY < YY; ContY ++)
	 for (ContX = X; ContX < XX; ContX++)
	     if (getpixel (ContX, ContY) == COLOR_AREA)
		   putpixel (ContX, ContY, COLOR_SAREA);
     ShowCursor ();
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  Sub_Area
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void Sub_Area (int X, int Y, int XX, int YY, int Z)
{
     X = VerdX (X); XX = VerdX (XX);
     Y = VerdY (Y); YY = VerdY (YY);
     int ContX, ContY;
     X++; Y++;
     HideCursor ();
     Relieve (X,Y,XX,YY,Z, COLOR_AREA_REL1, COLOR_AREA_REL2);
     for (ContY = Y; ContY < YY; ContY ++)
	 for (ContX = X; ContX < XX; ContX++)
	     if (getpixel (ContX, ContY) == COLOR_SAREA)
		   putpixel (ContX, ContY, COLOR_AREA);
     ShowCursor ();
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  SubArea
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void SubArea (int X, int Y,int XX,int YY,int Z)
{
     X = VerdX (X); XX = VerdX (XX);
     Y = VerdY (Y); YY = VerdY (YY);
     X++;  Y++;
     HideCursor ();
     PintarArea (X, Y, XX, YY, COLOR_SAREA);
     Relieve (X,Y,XX,YY,Z, COLOR_SAREA_REL1, COLOR_SAREA_REL2);
     ShowCursor ();
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  Boton
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void Boton (int X,int Y,int XX,int Sel, char *Titulo)
{
     if (Sel) PintarArea (X, Y, XX, Y + 10, DARKGRAY);
     else PintarArea (X, Y, XX, Y + 10, LIGHTGRAY);
     Relieve (X, Y, XX, Y + 10, 3, DARKGRAY, WHITE);
     TextoCentrado ((X + XX) / 2, Y + 5, Titulo, COLOR_TEXTO_BOTON);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  BotonNoCent
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void BotonNoCent (int X,int Y,int XX,int Z, char *Titulo, int tipo)
{
     char Tit [80], cont;
     strcpy (Tit, Titulo);
     for (cont = strlen (Tit) + 1; cont < 80; cont++) Tit [cont] = 0;
     if (tipo == TIPO_FECHA)
     {
	 Tit [8] = Tit [6] ? Tit [6] : ' ';
	 Tit [7] = Tit [5] ? Tit [5] : ' ';
	 Tit [6] = Tit [4] ? Tit [4] : ' ';
	 Tit [4] = Tit [3] ? Tit [3] : ' ';
	 Tit [3] = Tit [2] ? Tit [2] : ' ';
	 Tit [1] = Tit [1] ? Tit [1] : ' ';
	 Tit [0] = Tit [0] ? Tit [0] : ' ';
	 Tit [2] = '/';
	 Tit [5] = '/';
     }
     if (tipo == TIPO_HORA)
     {
	 Tit [5] = Tit [4] ? Tit [4] : ' ';
	 Tit [4] = Tit [3] ? Tit [3] : ' ';
	 Tit [3] = Tit [2] ? Tit [2] : ' ';
	 Tit [1] = Tit [1] ? Tit [1] : ' ';
	 Tit [0] = Tit [0] ? Tit [0] : ' ';
	 Tit [2] = ':';
     }

     X = VerdX (X); XX = VerdX (XX);
     Y = VerdY (Y);
     HideCursor ();
     PintarArea (X, Y, XX, Y + 10, COLOR_SAREA);
     Relieve (X, Y, XX, Y + 10, Z, DARKGRAY, WHITE);
     setcolor (COLOR_TEXTO_BOTON);
     outtextxy (X + 4, Y + 2, Tit);
     ShowCursor ();
}
/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  BotonNoCentCurs
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void BotonNoCentCurs (int X,int Y,int XX,int Z, char *Titulo, int tipo)
{
   char *aux, cursor [2] = "Ь";
   aux = (char *) malloc (strlen (Titulo) + 4);
   strcpy (aux, Titulo);
   strcat (aux, cursor);
   BotonNoCent (X, Y, XX, Z, aux, tipo);
   free (aux);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  BotonOp
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void BotonOp (TOpcion op, int pulsado)
{
     int XX = VerdX (op.X + op.Ancho),
	  X = VerdX (op.X),
	  Y = VerdY (op.Y),
	  cont;

     HideCursor ();
     if (pulsado)
     {
	 PintarArea (X, Y, XX, Y + 10, GRAY);
	 Relieve (X, Y, XX, Y + 10, 3, DARKGRAY, WHITE);
     }
     else
     {
	 PintarArea (X, Y, XX, Y + 10, LIGHTGRAY);
	 Relieve (X, Y, XX, Y + 10, 3, WHITE, DARKGRAY);
     }
     setcolor (LIGHTGREEN);
     switch (op.op [0]) {
     case '<':   for (cont = 0; cont < 6; cont++)
		    line (X + cont / 2 + 2, Y + cont + 2, X + 6 - cont / 2, Y + cont + 2); break;
     case '>':   for (cont = 0; cont < 6; cont++)
		    line (X + cont / 2 + 2, Y + 8 - cont, X + 6 - cont / 2, Y + 8 - cont); break;
     case '-':   setcolor (RED);
		 line (X + 1, Y + 1, X + 7, Y + 9);
		 line (X + 7, Y + 1, X + 1, Y + 9);  break;
     case '+':   setcolor (WHITE);
		 line (X + 1, Y + 4, X + 4, Y + 8);
		 line (X + 4, Y + 8, X + 7, Y + 1); break;
     default : TextoCentrado ((X + XX) / 2, Y + 5, op.op, BLACK);
     }
     ShowCursor ();
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  GuardarZona
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void GuardarZona (int X, int Y,int XX,int YY, TZona *zona)
{
    X = VerdX (X) - 6; XX = VerdX (XX) + 6;
    Y = VerdY (Y) - 6; YY = VerdY (YY) + 6;
    zona->X = X;
    zona->Y = Y;
    zona->punt = malloc (imagesize (X,Y,XX,YY));
    HideCursor ();
    getimage (X,Y,XX,YY,zona->punt);
    ShowCursor ();

}
/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  RestaurarZona
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void RestaurarZona (TZona *zona)
{
     HideCursor ();
     putimage (zona->X,zona->Y, zona->punt,COPY_PUT);
     ShowCursor ();
     free (zona->punt);
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  RestaurarZonaNF
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void RestaurarZonaNF (TZona *zona)
{
     HideCursor ();
     putimage (zona->X,zona->Y, zona->punt,COPY_PUT);
     ShowCursor ();
}

/*ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
				  AreaGZ
ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД*/
void AreaGZ (int X, int Y,int XX,int YY, int Z, char *Titulo,TZona *zona)
{
    GuardarZona (X,Y,XX,YY,zona);
    Area (X,Y,XX,YY,Z,Titulo);
}



