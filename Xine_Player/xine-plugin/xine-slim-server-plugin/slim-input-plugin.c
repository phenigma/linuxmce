#include "slim-input-plugin.h"
#include "slim-protocol-handler.h"
#include "slim-stream.h"
#include "socket-utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define MAX_READ_BUFFER_SIZE 1024

struct slim_stream audio_stream;

static char commandNames[][4] = {
	"VERS",
	"strm",
	"ircc",
	"vfdc",
	"grdf"
};

/**
 * This will send a buffer back to the SlimServer.
 */
int	send_command(slim_input_plugin_t *plugin, xine_t *xine_handler, unsigned char *buffer, int bufferLength)
{
#ifndef WIN32
	if ( write (plugin->comm_socket, buffer, bufferLength) != bufferLength )
#else
	if ( send (plugin->comm_socket, buffer, bufferLength, 0) != bufferLength )
#endif
	{
		xine_log(xine_handler, XINE_LOG_PLUGIN, _(LOG_MODULE "Error while writing command to network: %s. Closing connection!\n"), strerror(errno));
		close((int)plugin->comm_socket);
		plugin->comm_socket = -1;
		return 0;
	}

	return 1;
}

int send_status_command(char command[4], slim_input_plugin_t *plugin, xine_t *handler)
{
	unsigned int bufferLength;
	unsigned char *buffer;
	int result;

	if ( slim_protocol_make_ack(&buffer, &bufferLength, command, 0, /* */ 0, 1, 262144, plugin->source_stream.readPtr, plugin->source_stream.writePtr, 1000) == 0 )
		return 0;

	result = send_command(plugin, handler, buffer, bufferLength);
	free(buffer);
	return result;
}

/************************************************************************/
/* This will read a command buffer from the server.                     */
/************************************************************************/
static int read_slim_server_command(SOCKET *socket, unsigned char *buffer, unsigned int *len, xine_t *xine_handler)
{
	unsigned int frameLength;

#ifndef WIN32
	if ( read (*socket, buffer, 2) != 2 )
#else
	if ( recv (*socket, buffer, 2, 0) != 2 )
#endif
	{
		xine_log(xine_handler, XINE_LOG_PLUGIN, _(LOG_MODULE "Error while reading frame length from network: %s. Closing connection!\n"), strerror(errno));
		close((int)*socket);
		*socket = -1;
		return 0;
	}

	frameLength = buffer[0] << 8 | buffer[1];

	if ( frameLength >= *len)
	{
		xine_log(xine_handler, XINE_LOG_PLUGIN, _(LOG_MODULE "The frame length was bigger than expected. Dropping connection! \n"));
		close((int)*socket);
		*socket = -1;
		return 0;
	}

#ifndef WIN32
	if ( read (*socket, buffer, frameLength) != frameLength)
#else
	if ( recv (*socket, buffer, frameLength, 0) != frameLength)
#endif
	{
		xine_log(xine_handler, XINE_LOG_PLUGIN, _(LOG_MODULE "Error while reading frame data from network: %s. Closing connection!\n"), strerror(errno));
		close((int)*socket);
		*socket = -1;
		return 0;
	}

	*len = frameLength;
	return 1;
}

static int read_and_decode_command(slim_input_plugin_t *plugin, xine_t *xine_handler, struct slimCommand *parsedCommand)
{
	static char			buffer[MAX_READ_BUFFER_SIZE];
	static unsigned int bufferLen = 0;

	bufferLen = MAX_READ_BUFFER_SIZE;
	if ( read_slim_server_command(&plugin->comm_socket, buffer, &bufferLen, xine_handler) == 0 )
		return 0;

	if ( slim_protocol_decode(buffer, bufferLen, xine_handler, parsedCommand) == 0 )
		return 0;

	return 1;
}

int process_command_stream(slim_input_plugin_t *plugin, slim_input_class_t *plugin_class, struct slimCommand *command)
{
	struct sockaddr_in peerAddress;
	int peerAddressLen;

	if ( command->data.stream.hostAddr.s_addr == 0 )
	{
		peerAddressLen = sizeof(peerAddress);
		getpeername(plugin->comm_socket, (struct sockaddr*)&peerAddress, &peerAddressLen);
		command->data.stream.hostAddr = peerAddress.sin_addr;
	}

	plugin->source_stream.hostAddr = command->data.stream.hostAddr;
	plugin->source_stream.hostPort = command->data.stream.hostPort;

	if ( plugin->source_stream.uri)
		free(plugin->source_stream.uri);

	plugin->source_stream.uri = (unsigned char*)xine_xmalloc(command->data.stream.urlSize + 1);

	strncpy(plugin->source_stream.uri, command->data.stream.urlAddress, command->data.stream.urlSize);
	plugin->source_stream.uri[command->data.stream.urlSize] = 0;

	switch(command->data.stream.command )
	{
	case STREAM_QUIT:
		stream_close(&plugin->source_stream, plugin_class->xine);
		break;
	case STREAM_START:
		stream_open(&plugin->source_stream, plugin_class->xine);
		send_status_command("STMc", plugin, plugin_class->xine);
		send_status_command("STMh", plugin, plugin_class->xine);
		break;
	case STREAM_PAUSE:
	case STREAM_UNPAUSE:
	case STREAM_NO_COMMAND: default: break;
		// ignore this command
	}

	return 1;
}

int process_command_from_server(slim_input_plugin_t *plugin, slim_input_class_t *plugin_class, struct slimCommand *command)
{
	switch ( command->type )
	{
	case COMMAND_STREAM:
		return process_command_stream(plugin, plugin_class, command);
	case COMMAND_I2C:
		// TODO: Do this
	case COMMAND_VFDISPLAY:
		// TODO: Do this
	case COMMAND_DISPLAY:
		// TODO: Do this
		break;

	default:
		xine_log(plugin_class->xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Command type %d from server not processed!\n"), command->type);
		return 0;
	}
}

int do_one_command(slim_input_plugin_t *plugin, slim_input_class_t *plugin_class)
{
	struct slimCommand command;

	if ( read_and_decode_command(plugin, plugin_class->xine, &command) == 0 )
	{
		xine_log(plugin_class->xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Error reading frame from server and decoding it. Closing connection!\n"));
		close((int)plugin->comm_socket);
		plugin->comm_socket = -1;
		return 0;
	}

	if ( process_command_from_server(plugin, plugin_class, &command) == 0 )
		xine_log(plugin_class->xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Error processing command from server. Skipping command!\n"));

	return send_status_command(commandNames[command.type], plugin, plugin_class->xine);
}

int parse_server_address(const char *address, unsigned char **hostname, int *port, xine_t *xine)
{
	// helpers in parsing the URL
	const char *pChar, *pStartPortNumber;
	char *pPortNumberAsString; // this is used only when we have a port number specified.

	pChar = address;

	while ( *pChar && *pChar != ':' && *pChar != '/' ) pChar++;
	if ( pChar == address )
	{
		xine_log(xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Could not detect the hostname from URL: %s!\n"), address);
		return 0;
	}

	// we read this as the host name to connect to
	*hostname = (char *) xine_xmalloc(pChar - address + 1);

	strncpy(*hostname, address, pChar - address);

	(*hostname)[pChar - address] = 0;

	*port = 3483; // the default slim protocol port.
	if ( *pChar == ':' )
	{
		pStartPortNumber = ++pChar;
		while ( *pChar && ('0' <= *pChar && *pChar <= '9') ) pChar++;

		if ( pChar != pStartPortNumber )
		{
			pPortNumberAsString = (char*)xine_xmalloc(pChar - pStartPortNumber + 1);
			strncpy(pPortNumberAsString, pStartPortNumber, pChar - pStartPortNumber);
			pPortNumberAsString[pChar - pStartPortNumber] = 0;
			*port = atoi(pPortNumberAsString);
			free(pPortNumberAsString);
		}
	}

	return 1;
}
static int slim_plugin_open (input_plugin_t *plugin_generic )
{
	slim_input_plugin_t *plugin = (slim_input_plugin_t*) plugin_generic;
	slim_input_class_t  *plugin_class = (slim_input_class_t*)plugin->input_plugin.input_class;

	char  macAddress[6] = {11, 11, 11, 11, 11, 11 };
	unsigned char *buffer = NULL;
	int   bufferLength = 0;

	struct slimCommand command;

	xine_log(plugin_class->xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Plug-in open called!\n"));

	if ( parse_server_address(plugin->slim_server_specification, &plugin->hostAddr, &plugin->hostPort, plugin_class->xine) == 0 )
		return 0;

	xine_log(plugin_class->xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Connecting to host: %s port: %d\n"), plugin->hostAddr, plugin->hostPort);
	if ( (plugin->comm_socket = connect_to_host_with_name(plugin->hostAddr, plugin->hostPort, plugin_class->xine)) == -1)
	{
		free(plugin->hostAddr);
		plugin->hostAddr = NULL;
		return 0;
	}
	xine_log(plugin_class->xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Connected to host: %s port: %d\n"), plugin->hostAddr, plugin->hostPort);


	slim_protocol_make_helo(&buffer, &bufferLength, 3, 0, macAddress);
	send_command(plugin, plugin_class->xine, buffer, bufferLength);
	free(buffer);

	if ( read_and_decode_command(plugin, plugin_class->xine, &command) == 0 ||
		 command.type != COMMAND_VERSION )
	{
		xine_log(plugin_class->xine, XINE_LOG_PLUGIN, _(LOG_MODULE "The server didn't reply to HELO or the replay was invalid (not a version)! Closing connection!\n"));
		close((int)plugin->comm_socket);
		plugin->comm_socket = -1;
		return 0;
	}

	xine_log(plugin_class->xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Server replayed with version: %s\n"), command.data.version.versionData);
	return 1;
}

static uint32_t slim_plugin_get_capabilities (input_plugin_t *this_gen)
{
	return INPUT_CAP_PREVIEW | INPUT_CAP_RIP_FORBIDDEN;
}

static off_t slim_plugin_get_current_pos (input_plugin_t *this_gen)
{
	slim_input_plugin_t *this_plugin = (slim_input_plugin_t *) this_gen;

	return this_plugin->current_position;
}

static off_t slim_plugin_seek (input_plugin_t *this_gen, off_t offset, int origin) {
  slim_input_plugin_t *this_plugin = (slim_input_plugin_t*) this_gen;

  return offset;
}


static char* slim_plugin_get_mrl (input_plugin_t *this_gen) {
  slim_input_plugin_t *this_plugin = (slim_input_plugin_t *) this_gen;

  return this_plugin->mrl;
}

static int slim_plugin_get_optional_data (input_plugin_t *general_plugin, void *data, int data_type)
{
// 	slim_input_plugin_t *plugin = (slim_input_plugin_t *) general_plugin;
// 	slim_input_class_t  *plugin_class = (slim_input_class_t *) plugin->input_plugin.input_class;
//
// 	fill_preview_buffer(plugin, plugin_class);
//
// 	switch (data_type)
// 	{
// 		case INPUT_OPTIONAL_DATA_PREVIEW:
// 			memcpy (data, plugin->preview_buffer, plugin->preview_size);
// 			return plugin->preview_size;
// 			break;
// 	}

	return INPUT_OPTIONAL_UNSUPPORTED;
}

static void slim_plugin_dispose (input_plugin_t *this_gen ) {
  slim_input_plugin_t *this_plugin = (slim_input_plugin_t *) this_gen;

  if (this_plugin->comm_socket != -1) {
    close((int)this_plugin->comm_socket);
    this_plugin->comm_socket = -1;
  }

  free (this_plugin->mrl);
  free (this_plugin->slim_server_specification);

  //if (this->nbc) {
  //  nbc_close (this->nbc);
  //  this->nbc = NULL;
  //}

  free (this_gen);
}
static off_t slim_plugin_read (input_plugin_t *input_plugin, char *buf, off_t len)
{
	slim_input_plugin_t *plugin = (slim_input_plugin_t *) input_plugin;
	slim_input_class_t  *plugin_class = (slim_input_class_t *) plugin->input_plugin.input_class;
	off_t n, total;

	// xine_log(plugin_class->xine, XINE_LOG_MSG, _(LOG_MODULE ": Plugin open called!"));
	xine_log(plugin->stream->xine, XINE_LOG_MSG, _(LOG_MODULE ": Reading data from network"));

	do_one_command(plugin, plugin_class);

	total=0;

/*	if( (len - total) > 0 )
	{
		n = _x_read_abort (this_plugin->stream, (int)this_plugin->comm_socket, &buf[total], len-total);

		xine_log(this_plugin->stream->xine, XINE_VERBOSITY_DEBUG, _(LOG_MODULE ": got %d bytes (%d/%d bytes read)\n"), (intmax_t)n, (intmax_t)total, (intmax_t)len);
		if (n < 0)
		{
			_x_message(this_plugin->stream, XINE_MSG_READ_ERROR, this_plugin->slim_server_specification, NULL);
			return 0;
		}

		this_plugin->current_position += n;
		total += n;
	}*/
	return total;
}

static buf_element_t *slim_plugin_read_block (input_plugin_t *this_gen, fifo_buffer_t *fifo, off_t todo)
{
	buf_element_t       *buf = fifo->buffer_pool_alloc (fifo);
	off_t                total_bytes;

	buf->content	= buf->mem;
	buf->type		= BUF_DEMUX_BLOCK;

	total_bytes = slim_plugin_read(this_gen, buf->content, todo);

	if (total_bytes != todo)
	{
		buf->free_buffer (buf);
		return NULL;
	}

	buf->size = total_bytes;

	return buf;
}

static off_t slim_plugin_get_length (input_plugin_t *this_gen)
{
	return 0;
}

static uint32_t slim_plugin_get_blocksize (input_plugin_t *this_gen)
{
	return SLIM_SERVER_BS_LEN;
}

static char *slim_class_get_description (input_class_t *this_gen)
{
	return _( LOG_MODULE "input plugin for xine used in the pluto system (see http://www.plutohome.com).");
}

static const char *slim_class_get_identifier (input_class_t *this_gen)
{
	return "SlimTCP";
}

static void slim_class_dispose (input_class_t *this_class_gen)
{
	slim_input_class_t  *this_class = (slim_input_class_t *) this_class_gen;

	free (this_class);
}




static input_plugin_t *slim_class_get_instance (
		input_class_t *generator_class,
		xine_stream_t *stream,
		const char *mrl)
{
	slim_input_plugin_t *this_object = NULL;

	// slim_input_plugin_t *this;
//	nbc_t *nbc = NULL;
	char *filename;

	if ( ! strncmp (mrl, "slim://", 7) )
	{
		filename = (char *) &mrl[7];

		if((!filename) || (strlen(filename) == 0)) {
			return NULL;
		}

// 		nbc = nbc_init (stream);

//	} else if (!strncasecmp (mrl, "slave://", 8)) {

//		filename = (char *) &mrl[8];

//		if((!filename) || (strlen(filename) == 0)) {
//			return NULL;
//		}

		/* the only difference for slave:// is that network buffering control
		* is not used. otherwise, dvd still menus are not displayed (it freezes
		* with "buffering..." all the time)
		*/

//		nbc = NULL;

	} else {
		return NULL;
	}



	this_object						= xine_xmalloc(sizeof(slim_input_plugin_t));
	this_object->mrl				= strdup(mrl);
	this_object->slim_server_specification = strdup(filename);
	this_object->stream				= stream;
	this_object->comm_socket		= -1;
	this_object->current_position	= 0;
	// this->nbc           = nbc;
	// this_object->preview_size = 0;

	this_object->input_plugin.open              = slim_plugin_open;
	this_object->input_plugin.get_capabilities  = slim_plugin_get_capabilities;
	this_object->input_plugin.read              = slim_plugin_read;
	this_object->input_plugin.read_block        = slim_plugin_read_block;
	this_object->input_plugin.seek              = slim_plugin_seek;
	this_object->input_plugin.get_current_pos   = slim_plugin_get_current_pos;
	this_object->input_plugin.get_length        = slim_plugin_get_length;
	this_object->input_plugin.get_blocksize     = slim_plugin_get_blocksize;
	this_object->input_plugin.get_mrl           = slim_plugin_get_mrl;
	this_object->input_plugin.get_optional_data = slim_plugin_get_optional_data;
	this_object->input_plugin.dispose           = slim_plugin_dispose;
	this_object->input_plugin.input_class       = generator_class;

	return &this_object->input_plugin;
}

void *init_slim_input_class (xine_t *xine, void *data)
{
	slim_input_class_t *this;

	this         = (slim_input_class_t *) xine_xmalloc(sizeof(slim_input_class_t));
	this->config = xine->config;
	this->xine   = xine;

	this->input_class.get_instance      = slim_class_get_instance;
	this->input_class.get_description   = slim_class_get_description;
	this->input_class.get_identifier    = slim_class_get_identifier;
	this->input_class.get_dir           = NULL;
	this->input_class.get_autoplay_list = NULL;
	this->input_class.dispose           = slim_class_dispose;
	this->input_class.eject_media       = NULL;

	return this;
}

/*
* exported plugin catalog entry
*/

plugin_info_t xine_plugin_info[] = {
	/* type, API, "name", version, special_info, init_function */
	{ PLUGIN_INPUT, 15, "slim", XINE_VERSION_CODE, NULL, init_slim_input_class },
	{ PLUGIN_NONE, 0, "", 0, NULL, NULL }
};
