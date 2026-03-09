#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <dir.h>
#include <pantalla.hpp>

#define MAX_PANT 2000
#define TAM_PANT 512
char nombres [MAX_PANT][6];
int CantNomb;

int qsort_f ( const void *a, const void *b)
{
   return (strcmp((char *)a,(char *)b) );
}

void LeerNombres ()
{
	struct ffblk ffblk;
	int fin;
	fin = findfirst("*.pan",&ffblk,0);
     CantNomb = 0;
	while (!fin)
	{
          strncpy (nombres [CantNomb],ffblk.ff_name,5);
   		fin = findnext(&ffblk);
          CantNomb++;
     }
}


void Juntar ()
{
	int cont,nh,nl, exnh=-1, ah=0, al;
     FILE *dat, *pan;
     char nomaux [13];
     union {
		SNivel Niv;
          char p2 [TAM_PANT];
     };

     dat = fopen ("pant.dat","wb");
     fseek (dat,0L,	SEEK_SET);
     for (cont = 0; cont < CantNomb; cont++)
     {
     	nh = (nombres [cont][0] - '0') * 10 + (nombres [cont][1] - '0');
     	nl = (nombres [cont][2] - '0') * 10 + (nombres [cont][3] - '0');
          if (nh != exnh)
          {
          	ah++;
               al = 1;
               exnh = nh;
          } else al++;
          strcpy (nomaux, nombres [cont]); strcat (nomaux, ".pan");
		pan = fopen (nomaux, "rb");
          fread (&Niv, TAM_PANT, 1, pan);
          fclose (pan);
          Niv.NroNivel = ah * 256 + al;
          fwrite (&Niv, TAM_PANT,1, dat);

          printf ("%s -----> %02d.%02d\n", nombres [cont],ah, al);

     }
     fclose (dat);

}

int main ()
{
     LeerNombres ();
   	qsort (nombres, CantNomb, sizeof (nombres [0]), qsort_f);
     Juntar ();
}
