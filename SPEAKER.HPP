void InitSpeaker();
void CloseSpeaker();
void SpkSendByte(unsigned char b);
void SpkPlayBuf(void *pBuffer, unsigned BufferSize, unsigned *Frec, void (*BufProc)());
void SpkStopBuf();
