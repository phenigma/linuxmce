#ifndef SLIM_INPUT_PLUGIN_H
#define SLIM_INPUT_PLUGIN_H

#include "local_defines.h"

#include "slim-stream.h"

#include "xine_internal.h"
#include "xineutils.h"
#include "input/input_plugin.h"

// #include "net_buf_ctrl.h"

#define SLIM_SERVER_BS_LEN	2324
// #define BUFSIZE         1024

#ifndef WIN32
#ifndef SOCKET
	#define SOCKET int
#endif
#endif

struct slim_input_class_s {
	input_class_t     input_class;

	xine_t           *xine;
	config_values_t  *config;
};

typedef struct slim_input_class_s slim_input_class_t;

struct slim_input_plugin_s 
{
	input_plugin_t input_plugin; // the base plugin data	

	// network connection data
	SOCKET			 comm_socket;
	char            *mrl;
	char            *slim_server_specification;	


	// the stream we are playing 
	xine_stream_t	*stream;

	struct slim_stream source_stream;
	// data preview	buffer
	char             preview_buffer[MAX_PREVIEW_SIZE];
	off_t            preview_size;

	// current position 
	off_t            current_position;
};

typedef struct slim_input_plugin_s slim_input_plugin_t;


#endif // SLIM_INPUT_PLUGIN_H
