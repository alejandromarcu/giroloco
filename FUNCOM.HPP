#ifndef FUNCOM

int  trunc (float n);
void EmpezarGraf ();
void EsperarWR ();
void PintArea (int x,int y, int xx, int yy, int col);
void CopiarPagina (int pag);

void EncriptPant (int pag, int ocult);
void inline OcultarPant (int pag) {EncriptPant (pag, 1);};
void inline DesocultarPant (int pag) {EncriptPant (pag, -1);};

void IniciarCronom ();
void VerificarCronom ();
void ActivarCronom (int Durac, void (*f) ());

// IBP.CPP
#define IBP_ENTORNO    "I0.IBP"
#define IBP_SALIR      "I1.IBP"
#define IBP_TABLAREC   "I2.IBP"
#define IBP_PIDENOM    "I3.IBP"
#define IBP_FONDOMEN   "I4.IBP"
#define IBP_ACERCADE   "I5.IBP"
#define IBP_GANO       "I6.IBP"

void CargarIBP (int x, int y,int pag, char *narch);

// MENU.CPP
int MenuPrincipal ();

#define FUNCOM
#endif
