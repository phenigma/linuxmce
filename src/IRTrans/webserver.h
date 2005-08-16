void ProcessWebRequest (SOCKET sock);
void ParseRequest (char* data,SOCKET sock);
int GetHtmlPage (char *mem,char *page);
void GetHtmlParameter (char *pnt,char *name,char *value);
void GetIndexPage (char *mem,char *lang);
void GetDevicesPage (char *mem,char *lang,char *parm);
void GenerateErrorPage (char *mem,int error);
void GenerateHtmlHeader (char *mem,char *data);
int GenerateImageHeader (char *mem,char *data,char *type,int len);
void GetIRTransType (char ver,char typ[]);
void GetDeviceConfigPage (char *mem,char *lang,int id);
void GetSendPage (char *mem,char *lang,char *parm);


