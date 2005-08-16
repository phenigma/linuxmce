
void	SetSerialTimeout (word time);
void	msSleep (long time);
int		OpenSerialPort (char device[]);
void	WriteSerialString (byte pnt[],int len);
int		ReadSerialString (byte pnt[],int len,word timeout);

int		OpenSerialPortEx (char Pname[],HANDLE *port);
int		WriteSerialStringEx (DEVICEINFO *dev,byte pnt[],int len);
void	SetSerialTimeoutEx (DEVICEINFO *dev,word time);
