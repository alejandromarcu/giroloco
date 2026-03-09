#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#define TamBuf 4096

typedef struct SCab {
               	char p2 [18];
                    long Ancho;
                    long Alto;
                    char p3 [1052];
			};


void LeerBMP (int x, int y, char *nom)
{
     int cx, cy, PosBuf = TamBuf;
     unsigned char Buf [TamBuf+10];
     long p;
	FILE *arch;
     SCab Cab;
     arch = fopen (nom, "rb");
     fread (&Cab,sizeof (SCab),1,arch);
     Cab.Ancho = (Cab.Ancho +1) / 2 * 2;
	for (cy = Cab.Alto; cy; cy--)
		for (cx = 0; cx < Cab.Ancho; cx++)
          {
          	if (PosBuf == TamBuf)
               {
			    p = fread (&Buf, 1,TamBuf,arch);
                   PosBuf=0;
               }
               putpixel (cx+x,cy+y,Buf [PosBuf]);
               PosBuf++;
          }
     fclose (arch);
}
