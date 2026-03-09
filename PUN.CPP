#include <stdio.h>
#include <string.h>
#include <graphics.h>
#include <alloc.h>

typedef struct PUN_Encab {
               	char mens [16];
                    int cant;
                    int posic [64];
			};

/*void CargarPUN (void *P [], char *narch)
{
     FILE *arch;
     PUN_Encab enc;
     int cont, tam;
     arch = fopen (narch,"rb");
	fread (&enc, sizeof (enc),1,arch);

	for (cont = 0; cont < enc.cant; cont++)
     {
          fseek (arch, enc.posic [cont],SEEK_SET);
          fread (&tam, sizeof (tam),1,arch);
          P [cont] = farmalloc (tam);
          fread (P [cont], tam,1,arch);
     }
     fclose (arch);
}*/

int CargarPUN (void **P, char *narch)
{
     FILE *arch;
     PUN_Encab enc;
     int cont, tam;
     arch = fopen (narch,"rb");
	fread (&tam, sizeof (tam),1,arch);
     *P = farmalloc (tam * 24);
     fread (*P, tam * 24,1,arch);
     fclose (arch);
     return tam;
}

