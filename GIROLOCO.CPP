#include "defin.hpp"
#include "nodos.hpp"
#include "pantalla.hpp"
#include "rot.hpp"
#include "sonido.hpp"
#include "bichos.hpp"
#include "multitec.hpp"
#include "menu.hpp"
#include "vocplay.hpp"

Pantalla pant;
CProxPag ProxPag;
CRecords Records;
Rot 	    rot;
int      NivelInicial;

void IniciarPrograma ()
{
     NivelInicial = 1;
     randomize ();
     CargarConfig ();
	EmpezarGraf ();
     InstalarMT ();
     InitVocPlay ();
     CargarSonidos ();
	CargarImNodos ();
     InicBichos ();
     Records.Cargar ();
}

void IniciarJuego ()
{
     rot.IniciarJuego ();
	pant.IniciarJuego ();
}

void Ganar ()
{
     setvisualpage (0);
     setactivepage (0);
	CargarIBP (0,0,0,IBP_GANO);
     LeeTecla ();
    	Records.Agregar (rot.puntos [0], pant.Nivel.Nro);
}


void main ()
{
	Result r;
     IniciarPrograma ();
     while (MenuPrincipal ())
     {
          IniciarJuego ();
		do {
			r = pant.Jugar ();
		} while (rot.Vidas () && (r != PULSO_ESC) && (r!= GANO));
          if (!rot.Vidas ()) Records.Agregar (rot.puntos [0], pant.Nivel.Nro);
          if (r == GANO) Ganar ();
     }
     DoneVocPlay ();
     DesInstalarMT ();
     GrabarConfig ();
	closegraph ();
}