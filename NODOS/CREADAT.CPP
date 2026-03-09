#include <bmp.cpp>
#include <dos.h>

void main ()
{
   char tec;
   int x,y,tam, gdriver = DETECT, gmode, errorcode;
   void *p;
   FILE *arch;
   clrscr ();
   printf ("Este programa lee NODOS.BMP o MASCARAS.BMP, separa los nodos\n");
   printf ("y los graba en un .DAT\n\n");
   printf ("1=NODOS.BMP\n2=MASCARAS.BMP\nOtra tecla:salir");
   tec= getch ();
   if ((tec != '1') && (tec != '2')) return;

   initgraph(&gdriver, &gmode, "");

   LeerBMP (0,0, tec == '1'? "nodos.bmp" : "mascaras.bmp");

   tam = imagesize (0, 0,14,10);
   p = malloc (tam);

   arch = fopen (tec == '1'? "nodos.dat" : "mascaras.dat","wb");
   setcolor (14);
   for (y = 1; y <= 16; y++)
   	for (x = 1; x <= 16; x++)
     {
          if (tec == '1') getimage (x * 35 - 7, y * 20 - 4, x * 35 + 7, y * 20 + 6, p);
          else getimage (x * 35 - 5, y * 20 - 4, x * 35 + 9, y * 20 + 6, p);
          //rectangle (x * 35 - 5, y * 20 - 4, x * 35 + 9, y * 20 + 6);
     	fwrite (p,tam,1,arch);
     }

   fclose (arch);
   sound (440);
   delay (100);
   nosound ();
   getch ();
   closegraph ();
}