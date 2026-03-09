#ifndef MULTITEC

int const
 		T_BARRA	= 0x39,
		T_IZQ  	= 0x4B,
		T_DER	= 0x4D,
          T_ARR	= 0x48,
          T_ABA	= 0x50,
          T_ENTER	= 0x1C;

unsigned char LeeTecla ();
int TeclaPulsada ();
void InstalarMT ();
void DesInstalarMT ();

extern char Presionada [127];


#define MULTITEC
#endif
