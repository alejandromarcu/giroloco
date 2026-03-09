#ifndef WINTRUCH
#define WINTRUCH
const
     GRAY	  = 5;


void PonerPresent ();
void SacarPresent ();
void PintarArea (int X, int Y, int XX, int YY, int Col);
void TextoCentrado (int X, int Y, char *S, int Col);
void TextoCent (int X, int Y, char *S, int Col);
void Texto (int X, int Y, char *S, int Col);
void TextoNoC (int X, int Y, char *S, int Col);
void TextoNum (int X, int Y, long n, int Col, int nd = 0);
void TextoNumNoC (int X, int Y, long n, int Col, int nd = 0);
void TextoBorra (int X, int Y, char *S, int Col, int ColFondo);
void LineaH (int x, int y, int xx);
void LineaV (int x, int y, int yy);

void BorrarPant ();
void Area (int X, int Y,int XX,int YY, int Z, char *Titulo);
void AreaGZ (int X, int Y,int XX,int YY, int Z, char *Titulo,TZona *zona);
void SubArea (int X, int Y,int XX,int YY,int Z);
void Relieve (int X,int Y,int XX,int YY, int Z, int Col1, int Col2);
void Boton (int X,int Y,int XX,int Z, char *Titulo);
void BotonNoCent (int X,int Y,int XX,int Z, char *Titulo, int tipo = 0);
void BotonNoCentCurs (int X,int Y,int XX,int Z, char *Titulo, int tipo = 0);
void BotonOp (TOpcion op, int pulsado);
void Area_Sub (int X,int Y,int XX,int YY, int Z);
void Sub_Area (int X, int Y, int XX, int YY, int Z);
void GuardarZona (int X, int Y,int XX,int YY, TZona *zona);
void RestaurarZona (TZona *zona);
void RestaurarZonaNF (TZona *zona);

#endif