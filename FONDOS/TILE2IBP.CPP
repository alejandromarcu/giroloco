#include <string.h>
#include <bmp.cpp>
#include <ibp.cpp>

void main ()
{
   int x, y,gdriver = DETECT, gmode, errorcode, an, al;
   char nbmp [14];
   printf ("Nombre del BMP:");
   gets (nbmp);
   strcat (nbmp, ".bmp");
   initgraph(&gdriver, &gmode, "");
   LeerBMP (0,0,nbmp, &an, &al);
   for (y = 0; y <= (270 / an); y++)
   	for (x = 0; x <= (600 / al); x++)
   		LeerBMP (x*an,y*al,nbmp, &an, &al);
   GrabarIBP (0,0,600,270,"f2.ibp");
   getch ();
}
