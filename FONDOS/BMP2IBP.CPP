#include <string.h>
#include <bmp.cpp>
#include <ibp.cpp>

void main ()
{
   int x, y,gdriver = DETECT, gmode, errorcode, an, al;
   char nbmp [14],nibp [14],n [14];
   printf ("Nombre del archivo:");
   gets (n);
   strcpy (nbmp,n); strcat (nbmp, ".bmp");
   strcpy (nibp,n); strcat (nibp, ".ibp");

   initgraph(&gdriver, &gmode, "");
   LeerBMP (0,0,nbmp, &an, &al);
   GrabarIBP (0,0,an,al,nibp);
   getch ();
}
