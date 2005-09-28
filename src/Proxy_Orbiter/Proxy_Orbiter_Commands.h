#ifndef Proxy_Orbiter_Commands_h
#define Proxy_Orbiter_Commands_h

#define BD_PC_DISCONNECT				2
#define BD_PC_KEY_WAS_PRESSED			3
#define BD_PC_TOUCH_CLICK				4
#define BD_PC_SET_IMAGE_QUALITY         12

#define BD_CP_SHOW_IMAGE				102
#define BD_CP_DISCONNECT				105
#define BD_CP_SIMULATE_EVENT			109

class BDCommand *BuildCommandFromData( unsigned long dwType );

#endif
