#ifndef SONIDO
#include <alloc.h>

#define CANT_SONIDOS 7
#define S_PASO_NODO 		0
#define S_PERDIO_VIDA 		1
#define S_EXPLOSION			2
#define S_FIN_NODO_EN		3
#define S_APARECER			4
#define S_AVISO_FIN_PILDTC    5
#define S_FIN_PILDTC	     6
#define S_PRUEBA  	          6

int ResetDSP ();
void WriteDAC (int level);
int ReadDAC ();
void SpeakerOn ();
void SpeakerOff ();
void DMAStop ();
void DMAContinue ();
void Playback (void far *punt, unsigned tam, unsigned frec);

class WAV {
     	void far *sonido;
          unsigned tam, frec;
	public:
          void Desalocar () { farfree (sonido); };
          void Tocar () { Playback (sonido, tam, frec);}
          void Cargar (char *narch);
          WAV () { sonido = NULL; };
		WAV (char *narch) { Cargar (narch); };
		~WAV () { Desalocar (); };

     };

extern unsigned      DSP_RESET,
		    DSP_READ_DATA,
    		    DSP_WRITE_DATA,
  		    DSP_WRITE_STATUS,
    		    DSP_DATA_AVAIL;

extern WAV Sonidos [CANT_SONIDOS];

void Sonido (int nson);
void CargarSonidos ();

#endif
#define SONIDO
