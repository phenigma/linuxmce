void xAP_SendHeartbeat (void);
void xAP_SendData (char data[]);
void xAP_SendIREvent (char remote[],char command[],int bus,int adr);
void xAP_EventReceived (void);
void xAP_ProcessHeader (char data[]);
int  xAP_GetItem (char data[],char item[],char value[]);
int  xAP_GetLine (char data[],char line[],int pos);

extern SOCKET xAP_rcv;
extern SOCKET xAP_send;
extern int xAP_rcv_port;
extern int xAP_last_hbeat;


#define XAP_UID			"FF2121"
#define XAP_HBEAT		60
#define XAP_ADR			"IRTrans.irserver"
#define XAP_VERSION		12
