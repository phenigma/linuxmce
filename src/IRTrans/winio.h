
HANDLE hCom;
HANDLE hComEvent;

void SetSerialTimeout (word time);

int		OpenSerialPort (char device[]);
void	WriteSerialString (byte pnt[],int len);
int		ReadSerialString (byte pnt[],int len,word timeout);
int	WriteSerialStringEx (DEVICEINFO *dev,byte pnt[],int len);
int		ReadSerialStringEx (DEVICEINFO *dev,byte pnt[],int len,word timeout);
int		OpenSerialPortEx (char Pname[],HANDLE *port);
int		GetSerialAvailableEx (DEVICEINFO *dev);
