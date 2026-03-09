#include <stdio.h>
#include "ibp.cpp"

void main (int argc, char *argv [])
{
	int gd = VGA, gm = VGAMED;
	initgraph (&gd, &gm, "");
  	CargarIBP (0,0,0,argv [1]);
     getch ();
	closegraph ();
}
