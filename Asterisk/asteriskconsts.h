#ifndef __ASTERISKCONSTS_H_
#define __ASTERISKCONSTS_H_

/* tokens */
#define TOKEN_EVENT			"Event"
#define TOKEN_STATE			"State"
#define TOKEN_CHANNEL		"Channel"
#define TOKEN_RESPONSE		"Response"
#define TOKEN_MESSAGE		"Message"
#define TOKEN_CALLERID		"Callerid"
#define TOKEN_APPLICATION	"Application"
#define TOKEN_APPDATA		"AppData"
#define TOKEN_ACTION		"Action"
#define TOKEN_EXTENSION		"Exten"
#define TOKEN_CONTEXT		"Context"
#define TOKEN_PRIORITY		"Priority"



/* events */
#define EVENT_NEWCHANNEL 	"Newchannel"
#define EVENT_NEWEXTEN 		"Newexten"
#define EVENT_HANGUP 		"Hangup"

/* states */
#define STATE_RING 			"Ring"
#define STATE_RINGING 		"Ringing"

/* responses */
#define RESPONSE_SUCCESS    "Success"

/* applications */
#define APPLICATION_DIAL	"Dial"

/*actions*/
#define ACTION_ORIGINATE	"Originate"
#define ACTION_REDIRECT		"Redirect"
#define ACTION_HANGUP		"Hangup"

#endif
