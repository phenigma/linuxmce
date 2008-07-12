

int OpenSerialPortEx (const char Pname[],int *port);
int ReadSerialStringEx (int dev,char pnt[],int len,long timeout);
int WriteSerialStringEx (int dev,char pnt[],int len);
