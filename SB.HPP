char SBDetect();
void SBSendByte(unsigned char b);
void SBWrite(unsigned char b);
unsigned char SBRead();
void SBPlayBuf(void *pBuffer, unsigned BufferSize, unsigned *Frec, void (*BufProc)(...));
void SBStopBuf();

extern
char DSPVerMajor,
     DSPVerMinor,           /* Versi¢n del DSP */
     SBIRQ,                 /* IRQ de la SB    */
     SB8DMA,                /* DMA de 8 bits   */
     SB16DMA,               /* DMA de 16 bits  */
     TimeConst,             /* Constante de tiempo del DSP */
     SBVector;              /* N£mero de vector de interrupci¢n de la SB */
extern
char SBReadError,           /* Error de lectura en el DSP */
     SBWriteError;          /* Error de escritura en el DSP */

/* Puertos de la SB */
extern
unsigned SBBase,            /* Puerto base                         */
         SBMixAddr,         /* Registro de direcciones del mixer   */
         SBMixData,         /* Registro de datos del mixer         */
         SBReset,           /* Puerto de reset                     */
         SBReadp,           /* Puerto de lectura del DSP           */
         SBWritep,          /* Puerto de escritura del DSP         */
         SBRStat;           /* Puerto de estado de lectura del DSP */
