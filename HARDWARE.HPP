void EOI();
void EOISlave();
void HabilitaIRQ(char Num);
void DeshabilitaIRQ(char Num);

void ProgKDMA(char DMANum, void *Buffer, unsigned Tamanyo, char AutoInit, char DesdeMem);
void StopDMA(char DMANum);

void ProgTimer0(unsigned Divisor);
void SetTimerFrec(unsigned *Frecuencia);
