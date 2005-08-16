int		InitServer (char dev[]);
int		RunServer ();
void	SwapNetworkheader (NETWORKSTATUS *ns);
void	SwapNetworkcommand (NETWORKCOMMAND *nc);
void	SwapNetworkstatus (void *pnt);
int		GetDeviceStatus (STATUSBUFFER *buf);
int		GetHotcode (char rem[],char com[],byte data[]);
void	PutNetworkStatus (int res,char msg[],STATUSBUFFER *buf);
void	StoreTransItem (TRANSLATECOMMAND *tr);
int		FileTransData (char nm[]);
int		LoadTranslation (TRANSLATEBUFFER *tb,char nm[],word offset);
int		SetFlashdataEx (byte bus,long iradr);
unsigned long GetMsTime (void);
void	udp_relay (char rem[],char com[],int adr);
void	InitMediacenter (void);
int		GetDeviceData (long cmd_num,DATABUFFER *dat);

extern byte status_changed;
extern unsigned long netmask[32];
extern unsigned long netip[32];
extern byte netcount;

