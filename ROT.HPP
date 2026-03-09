#ifndef ROT
#include "defin.hpp"

enum Rot_acciones { NORMAL = 0,
				PASAR_INICIAR, PASAR_ANGOSTAR,PASAR, PASAR_ENSANCHAR,
				APARECER_INICIAR, APARECER_CRECER, APARECER_ESPERA,
				PERDER_INICIAR, PERDER_ACHICAR, PERDER_FIN,
				DESAPARECER_INICIAR, DESAPARECER_ACHICAR, DESAPARECER_FIN};

class Rot {
		int quiere_pasar;
		IntAnt  vidas;
	public:
          char PildMBActiva;
		LongAnt puntos;
		Rot_acciones accion;
		void *fondo;
		int x, y, angulo, vel;
		Rot ();
          ~Rot ();
		int Vidas () { return vidas [0];};
          void IniciarJuego ();
		void UbicarEn (int cx, int cy) { x = cx; y = cy; };
		void Dibujar ();
		void Borrar ();
		void AumentarPuntos (int p) {puntos [0]+= p; }
		void PerderVida () {vidas [0]--;};
		void GanarVida ()  {vidas [0]++;};
		void PonerInfo ();
          void PonerPuntos ();
          void PonerVidas ();
          void PonerNivel (int nivel);
          void IniciarPant (int cx, int cy);
		void CambiarAnts () {puntos [1] = puntos [0] +1; vidas [1]= vidas [0]+1;};
          void Accion (Rot_acciones A) {accion = A;};
          int PosibleChoque (int xp, int yp);
		int PuntoAdentro (int x2, int y2);
		Result Mover ();
		 };

#endif
#define ROT