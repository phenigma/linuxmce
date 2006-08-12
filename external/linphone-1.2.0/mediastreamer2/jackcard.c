/*
  The mediastreamer library aims at providing modular media processing and I/O
	for linphone, but also for any telephony application.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org
  										
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  JACK support
  Copyright (C) 2004  Tobias Gehrig tobias@gehrig.tk
*/

#ifdef __JACK_ENABLED__

#include "jackcard.h"

#include "msossread.h"
#include "msosswrite.h"

#include <signal.h>

#define READBUFFERSIZE 524288
#define WRITEBUFFERSIZE 524288
#define BSIZE 512

//#define DEBUG

/**
 * This is the shutdown callback for this JACK application.
 * It is called by JACK if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
  JackCard* obj = (JackCard*) arg;

#ifdef DEBUG
  g_message("jack_shutdown");
#endif

  obj->jack_running = FALSE;
  obj->jack_active = FALSE;
  if (obj->read.open) obj->read.init = TRUE;
  if (obj->write.open) obj->write.init = TRUE;
  //  obj->read.can_process = FALSE;
  //  obj->write.can_process = FALSE;
  //  obj->can_process = FALSE;
  //  g_free(obj->read.phys_ports);
  //  g_free(obj->write.phys_ports);
}

int samplerate(jack_nframes_t rate, void *arg)
{
  JackCard* obj = (JackCard*) arg;
  int error;

#ifdef DEBUG
  g_message("samplerate : %d", rate);
#endif

  obj->rate = rate;
  if (obj->read.open) {
    obj->read.data.src_ratio = (double)obj->read.rate / (double)obj->rate;
    obj->read.data.input_frames = (long)((double)obj->read.frames/obj->read.data.src_ratio);
    g_free(obj->read.data.data_in);
    obj->read.data.data_in = malloc(obj->read.data.input_frames*sizeof(float));
    if (obj->read.src_state)
      if ((error = src_set_ratio(obj->read.src_state, obj->read.data.src_ratio)) != 0)
	g_warning("Error while resetting the write samplerate: %s", src_strerror(error));
  }
  if (obj->write.open) {
    obj->write.data.src_ratio = (double)obj->rate / (double)obj->write.rate;
    obj->write.data.output_frames = (long)((double)obj->write.frames*obj->write.data.src_ratio);
    g_free(obj->write.data.data_out);
    obj->write.data.data_out = malloc(obj->write.data.output_frames*sizeof(float));
    if (obj->write.src_state) 
      if ((error = src_set_ratio(obj->write.src_state, obj->write.data.src_ratio)) != 0)
	g_warning("Error while resetting the write samplerate: %s", src_strerror(error));
  }
  return 0;
}

/**
 * The process callback for this JACK application.
 * It is called by JACK at the appropriate times.
 */
int
process (jack_nframes_t nframes, void *arg)
{
  JackCard* obj = (JackCard*) arg;
  sample_t *out;
  sample_t *in;
  size_t bytes;
#ifdef DEBUG
  size_t bytes2;
#endif

  if (!obj->can_process)
    return 0;

  if(obj->read.can_process) {
    in = (sample_t *) jack_port_get_buffer (obj->read.port, nframes);
    bytes = jack_ringbuffer_write (obj->read.buffer, (void *) in, sizeof(sample_t) * nframes);
    obj->read.real_total_bytes += bytes;
#ifdef DEBUG
    if (bytes != sizeof(sample_t) * nframes)
      g_warning("not all read in process: %d %d", bytes, sizeof(sample_t) * nframes);
#endif
  }

  if (obj->write.can_process) {
    out = (sample_t *) jack_port_get_buffer (obj->write.port, nframes);
    memset (out, 0, nframes * sizeof(sample_t));
#ifdef DEBUG
    if ((bytes2 = jack_ringbuffer_read_space(obj->write.buffer))<nframes*sizeof(sample_t))
      g_warning("not enough here to process");
#endif
    bytes = jack_ringbuffer_read (obj->write.buffer, (void *) out, sizeof(sample_t) * nframes);
    obj->write.real_total_bytes += bytes;
#ifdef DEBUG
    if (bytes != MIN(bytes2,nframes*sizeof(sample_t)))
      g_warning("not all written in process: %d %d", bytes, MIN(bytes2,nframes*sizeof(sample_t)));
#endif
  }
  return 0;      
}

int jack_init(JackCard* obj)
{
  char* client_name;
  int error;

  if (!obj->jack_running) {
    obj->client = NULL;
    client_name = g_strdup_printf("linphone-%u", g_random_int());
    if ((obj->client = jack_client_new (client_name)) == NULL) {
      g_warning("cannot create jack client");
      g_free(client_name);
      return -1;
    }
    g_message("Found Jack Daemon");
    g_free(client_name);
    
    /* tell the JACK server to call `process()' whenever
       there is work to be done.
    */
    jack_set_process_callback (obj->client, process, obj);

    /* tell the JACK server to call `jack_shutdown()' if
       it ever shuts down, either entirely, or if it
       just decides to stop calling us.
    */
    jack_on_shutdown (obj->client, jack_shutdown, obj);
    jack_set_sample_rate_callback (obj->client, samplerate, obj);
    obj->rate = jack_get_sample_rate (obj->client);
    if (obj->rate == 0) {
      g_warning ("rate is 0???");
      if (jack_client_close(obj->client) != 0)
	g_warning("could not close client");
      return -1;
    }
    obj->buffer_size = jack_get_buffer_size(obj->client);
    obj->jack_running = TRUE;
  }

  if (!obj->jack_active) {
    if (jack_activate (obj->client)) {
      g_warning("cannot activate jack client");
      return -1;
    } else obj->jack_active = TRUE;
  }

  if (obj->read.init) {
    if ((obj->read.port = jack_port_register (obj->client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0))==NULL) {
      g_warning("error while trying to register input port");
      return -1;
    }
    if (obj->read.phys_ports) {
      g_free(obj->read.phys_ports);
      obj->read.phys_ports = NULL;
    }
    if ((obj->read.phys_ports = jack_get_ports (obj->client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput)) == NULL) {
      g_warning("Cannot find any physical capture ports\n");
      jack_port_unregister(obj->client, obj->read.port);
      return -1;
    }
#ifdef DEBUG
    else g_message("found port %s", obj->read.phys_ports[0]);
#endif
    if ((error = jack_connect (obj->client, obj->read.phys_ports[0], jack_port_name (obj->read.port))) != 0) {
      g_warning("cannot connect input ports: %s -> %s\n", jack_port_name (obj->read.port), obj->read.phys_ports[0]);
      if (error == EEXIST) g_warning("connection already made");
      else {
	jack_port_unregister(obj->client, obj->read.port);
	return -1;
      }
    }
    obj->read.init = FALSE;
  }
  if (obj->write.init) {
    if ((obj->write.port = jack_port_register (obj->client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0))==NULL) {
      g_warning("error while trying to register output port");
      return -1;
    }
    if (obj->write.phys_ports) {
      g_free(obj->write.phys_ports);
      obj->write.phys_ports = NULL;
    }
    if ((obj->write.phys_ports = jack_get_ports (obj->client, NULL, NULL, JackPortIsPhysical|JackPortIsInput)) == NULL) {
      g_warning("Cannot find any physical playback ports\n");
      jack_port_unregister(obj->client, obj->write.port);
      return -1;
    }
#ifdef DEBUG
    else g_message("found port %s and %s", obj->write.phys_ports[0], obj->write.phys_ports[1]);
#endif
    if ((error = jack_connect (obj->client, jack_port_name (obj->write.port), obj->write.phys_ports[0])) != 0) {
      g_warning("cannot connect output ports: %s -> %s\n", jack_port_name (obj->write.port), obj->write.phys_ports[0]);
      if (error == EEXIST) g_warning("connection already made");
      else {
	jack_port_unregister(obj->client, obj->write.port);
	return -1;
      }
    }
    if ((error = jack_connect (obj->client, jack_port_name (obj->write.port), obj->write.phys_ports[1])) != 0) {
      g_warning("cannot connect output ports: %s -> %s\n", jack_port_name (obj->write.port), obj->write.phys_ports[1]);
      if (error == EEXIST) g_warning("connection already made");
      else {
	jack_port_unregister(obj->client, obj->write.port);
	return -1;
      }
    }
    obj->write.init = FALSE;
  }
  return 0;
}

int jack_card_open_r(JackCard *obj,int bits,int stereo,int rate)
{
  int channels = stereo + 1, bsize, error;

#ifdef DEBUG
  g_message("jack_card_open_r");
#endif

  obj->read.init = TRUE;

  if (jack_init(obj) != 0) return -1;

  obj->read.rate = rate;
  obj->sample_size = bits / 8;
  obj->frame_size = channels * obj->sample_size;
  bsize = BSIZE;
  obj->read.frames = bsize / 2;
  obj->read.total_bytes = 0;
  obj->read.real_total_bytes = 0;
  SND_CARD(obj)->bsize = bsize;
  SND_CARD(obj)->flags |= SND_CARD_FLAGS_OPENED;
  obj->read.channels = channels;
  if ((obj->read.src_state = src_new (SRC_SINC_FASTEST, channels, &error)) == NULL)
    g_warning("Error while initializing the samplerate converter: %s", src_strerror(error));
  obj->read.data.src_ratio = (double)rate / (double)obj->rate;
  obj->read.data.input_frames = (long)((double)obj->read.frames/obj->read.data.src_ratio);
  obj->read.data.data_in = malloc(obj->read.data.input_frames*sizeof(float));
  obj->read.data.data_out = malloc(obj->read.frames*sizeof(float));
  obj->read.data.end_of_input = 0;
  obj->read.buffer = jack_ringbuffer_create(READBUFFERSIZE);
  obj->read.can_process = TRUE;
  obj->can_process = TRUE;
  obj->read.open = TRUE;
  obj->read.init = FALSE;
  return 0;
}

int jack_card_open_w(JackCard *obj,int bits,int stereo,int rate)
{
  int channels = stereo + 1, bsize, err;

#ifdef DEBUG
  g_message("jack_card_open_w");
#endif

  obj->write.init = TRUE;

  if (jack_init(obj) != 0) return -1;

  obj->write.rate = rate;
  obj->sample_size = bits / 8;
  obj->frame_size = channels * obj->sample_size;
  bsize = BSIZE;
  obj->write.frames = bsize / 2;
  obj->write.total_bytes = 0;
  obj->write.real_total_bytes = 0;
  SND_CARD(obj)->bsize = bsize;
  SND_CARD(obj)->flags |= SND_CARD_FLAGS_OPENED;
  obj->write.channels = channels;
  if ((obj->write.src_state = src_new (SRC_SINC_FASTEST, channels, &err)) == NULL)
    g_warning("Error while initializing the samplerate converter: %s", src_strerror(err));
  obj->write.data.src_ratio = (double)obj->rate / (double)rate;
  obj->write.data.data_in = malloc(obj->write.frames*sizeof(float));
  obj->write.data.end_of_input = 0;
  obj->write.data.output_frames = (long)((double)obj->write.frames*obj->write.data.src_ratio);
  obj->write.data.data_out = malloc(obj->write.data.output_frames*sizeof(float));
  obj->write.buffer = jack_ringbuffer_create(WRITEBUFFERSIZE);
  obj->write.chan = 0;
  obj->write.can_process = TRUE;
  obj->can_process = TRUE;
  obj->write.open = TRUE;
  obj->write.init = FALSE;
  return 0;
}

void jack_card_set_blocking_mode(JackCard *obj, gboolean yesno)
{
}

void jack_card_close_r(JackCard *obj)
{
#ifdef DEBUG
  g_message("jack_card_close_r");
#endif
  obj->read.open = FALSE;
  obj->read.init = FALSE;
  obj->read.can_process = FALSE;
  if (obj->jack_running) {
    jack_disconnect(obj->client, obj->read.phys_ports[0], jack_port_name(obj->read.port));
    g_free(obj->read.phys_ports);
    obj->read.phys_ports = NULL;
    jack_port_unregister(obj->client, obj->read.port);
    if ((obj->jack_active)&&(!obj->write.open)) {
      obj->can_process = FALSE;
      if (jack_deactivate(obj->client))
	g_warning("jack_card_close_r: Error while deactivating JACK");
      obj->jack_active = FALSE;
    }
  }
  if (obj->read.src_state)
    obj->read.src_state = src_delete (obj->read.src_state);
  g_free(obj->read.data.data_in);
  g_free(obj->read.data.data_out);
  jack_ringbuffer_free(obj->read.buffer);
  obj->read.buffer=NULL;
}

void jack_card_close_w(JackCard *obj)
{
#ifdef DEBUG
  g_message("jack_card_close_w");
#endif
  obj->write.open = FALSE;
  obj->write.init = FALSE;
  if (obj->jack_running) {
    // hopefully there weren't that much dropped frames otherwise this will
    // probably lead to an infinite loop. But if we do not wait until jack
    // has played back everything, we won't hear everything.
    while (obj->write.real_total_bytes + 1024 < obj->write.total_bytes) g_usleep(1000);
    obj->write.can_process = FALSE;
    jack_disconnect (obj->client, jack_port_name (obj->write.port), obj->write.phys_ports[0]);
    jack_disconnect (obj->client, jack_port_name (obj->write.port), obj->write.phys_ports[1]);
    g_free(obj->write.phys_ports);
    obj->write.phys_ports = NULL;
    jack_port_unregister(obj->client, obj->write.port);
    if ((obj->jack_active)&&(!obj->read.open)) {
      obj->can_process = FALSE;
      if (jack_deactivate(obj->client))
	g_warning("jack_card_close_w: Error while deactivating JACK");
      obj->jack_active = FALSE;
    }
  }
  if (obj->write.src_state)
    obj->write.src_state = src_delete (obj->write.src_state);
  g_free(obj->write.data.data_in);
  g_free(obj->write.data.data_out);
  jack_ringbuffer_free(obj->write.buffer);
  obj->write.buffer=NULL;
}

int jack_card_probe(JackCard *obj,int bits,int stereo,int rate)
{
  //  int ret;

#ifdef DEBUG
  g_message("jack_card_probe");
#endif
  /*
  ret = jack_card_open_w(obj,bits,stereo,rate);
  if (ret<0) return -1;
  ret = SND_CARD(obj)->bsize;
  jack_card_close_w(obj);
  return ret;
  */
  if (obj->jack_running) return BSIZE;
  else if (jack_init(obj) == 0) return BSIZE;
  else return -1;
}

void jack_card_destroy(JackCard *obj)
{
#ifdef DEBUG
  g_message("jack_card_destroy");
#endif

  if (obj->jack_running) jack_client_close (obj->client);
  snd_card_uninit(SND_CARD(obj));
  if (obj->read.buffer!=0) jack_ringbuffer_free(obj->read.buffer);
  if (obj->write.buffer!=0) jack_ringbuffer_free(obj->write.buffer);
}

gboolean jack_card_can_read(JackCard *obj)
{
#ifdef DEBUG
  //  g_message("jack_card_can_read");
#endif

  g_return_val_if_fail(obj->read.buffer!=NULL,0);
  if (jack_ringbuffer_read_space(obj->read.buffer)>=(long)((double)obj->read.frames/obj->read.data.src_ratio)*sizeof(sample_t)) return TRUE;
  else return FALSE;
}

int jack_card_read(JackCard *obj,char *buf,int size)
{
#ifdef DEBUG
  //  g_message("jack_card_read");
#endif
  size_t bytes, can_read, i;
  int error;
  float norm, value;

  g_return_val_if_fail((obj->read.buffer!=NULL)&&(obj->read.src_state!=NULL),-1);
  //  if (!obj->jack_running) return -1;
  if (jack_init(obj) != 0) return -1;
  size /= 2;
  can_read = MIN(size, obj->read.frames);
  //  can_read = MIN(((long)((double)can_read / obj->read.data.src_ratio))*sizeof(sample_t), jack_ringbuffer_read_space(obj->read.buffer));
  can_read = ((long)((double)can_read / obj->read.data.src_ratio))*sizeof(sample_t);
  obj->read.can_process = FALSE;
  bytes = jack_ringbuffer_read (obj->read.buffer, (void *)obj->read.data.data_in, can_read);
  obj->read.can_process = TRUE;
#ifdef DEBUG
  if (bytes != can_read)
    g_warning("not read everything by ringbuffer, bytes: %d, in: %d", bytes, can_read);
#endif
  obj->read.data.input_frames = bytes / sizeof(sample_t);
  can_read = MIN(size, obj->read.frames);
  obj->read.data.output_frames = can_read;
  if ((error = src_process(obj->read.src_state, &(obj->read.data))) != 0)
    g_warning("error while samplerate conversion. error: %s", src_strerror(error));
#ifdef DEBUG
  if (bytes != obj->read.data.input_frames_used* sizeof(sample_t))
    g_warning("not read everything by rate conversion, bytes: %d, used: %d", bytes, obj->read.data.input_frames_used*sizeof(sample_t));
  if (can_read != obj->read.data.output_frames_gen)
    g_warning("not read everything by rate conversion, can_read: %d, gen: %d", can_read, obj->read.data.output_frames_gen);
#endif
  norm = obj->read.level*obj->level*(float)0x8000;
  for (i=0; i < obj->read.data.output_frames_gen; i++) {
    value = obj->read.data.data_out[i]*norm;
    if (value >= 32767.0) 
      ((short*)buf)[i] = 32767;
    else if (value <= -32768.0)
      ((short*)buf)[i] = -32768;
    else
      ((short*)buf)[i] = (short)value;
  }
#ifdef DEBUG
  if (can_read != size)
    g_warning("not read everything by format conversion, can_read: %d, size: %d", can_read, size);
#endif
  bytes = obj->read.data.output_frames_gen * 2;
  obj->read.total_bytes += bytes;
  return bytes;
  //  return 0;
}

int jack_card_write(JackCard *obj,char *buf,int size)
{
  size_t bytes, can_write, i;
  int error;
  float norm;

#ifdef DEBUG
  //   g_message("jack_card_write, size: %d, written bytes : %d", size, obj->write.total_bytes);
#endif

  g_return_val_if_fail((obj->write.buffer!=NULL)&&(obj->write.src_state!=NULL),-1);
  //  if (!obj->jack_running) return -1;
  if (jack_init(obj) != 0) return -1;
  size /= 2;
  //  if (obj->write.chan == 0) {
    can_write = MIN(size, obj->write.frames);
    norm = obj->write.level*obj->level/(float)0x8000;
    for (i=0; i<can_write; i++) {
      obj->write.data.data_in[i] = (float)((short*)buf)[i]*norm;
    }
#ifdef DEBUG
    if (can_write != size)
      g_warning("not written everything by format conversion can_write: %d, size: %d", can_write, size);
#endif
    obj->write.data.input_frames = can_write;
    if ((error = src_process(obj->write.src_state, &(obj->write.data))) != 0)
      g_warning("error while samplerate conversion. error: %s", src_strerror(error));
    obj->write.can_process = FALSE;
    bytes = jack_ringbuffer_write (obj->write.buffer, (void *) obj->write.data.data_out, sizeof(sample_t)*obj->write.data.output_frames_gen);
    obj->write.can_process = TRUE;
    obj->write.total_bytes += bytes;
#ifdef DEBUG
    if (bytes != obj->write.data.output_frames_gen*sizeof(float))
      g_warning("not written everything by ringbuffer, bytes: %d, gen: %d", bytes, obj->write.data.output_frames_gen*sizeof(float));
    if (can_write != obj->write.data.input_frames_used)
      g_warning("not written everything by rate conversion, bytes: %d, gen: %d", can_write, obj->write.data.input_frames_used);
#endif
    //  }
//  obj->write.chan++;
//  obj->write.chan %= obj->write.channels;
  return bytes;
}

void jack_card_set_level(JackCard *obj,gint way,gint a)
{
#ifdef DEBUG
  //  g_message("jack_card_set_level");
#endif

  switch(way){
  case SND_CARD_LEVEL_GENERAL:
    obj->level = (float)a / 100.0;
    break;
  case SND_CARD_LEVEL_INPUT:
    obj->read.level = (float)a / 100.0;
    break;
  case SND_CARD_LEVEL_OUTPUT:
    obj->write.level = (float)a / 100.0;
    break;
  default:
    g_warning("jack_card_set_level: unsupported command.");
  }
}

gint jack_card_get_level(JackCard *obj,gint way)
{
  gint value = 0;

#ifdef DEBUG
  //  g_message("jack_card_get_level");
#endif

  switch(way){
  case SND_CARD_LEVEL_GENERAL:
    value = (gint)(obj->level*100.0);
    break;
  case SND_CARD_LEVEL_INPUT:
    value = (gint)(obj->read.level*100.0);
    break;
  case SND_CARD_LEVEL_OUTPUT:
    value = (gint)(obj->write.level*100.0);
    break;
  default:
    g_warning("jack_card_get_level: unsupported command.");
  }
  return value;
}

void jack_card_set_source(JackCard *obj,int source)
{
}

MSFilter *jack_card_create_read_filter(JackCard *card)
{
	MSFilter *f=ms_oss_read_new();
	ms_oss_read_set_device(MS_OSS_READ(f),SND_CARD(card)->index);
	return f;
}

MSFilter *jack_card_create_write_filter(JackCard *card)
{
	MSFilter *f=ms_oss_write_new();
	ms_oss_write_set_device(MS_OSS_WRITE(f),SND_CARD(card)->index);
	return f;
}
SndCard * jack_card_new(jack_client_t *client)
{
	JackCard * obj;
	SndCard *base;

#ifdef DEBUG
  g_message("jack_card_new");
#endif

	obj= g_new0(JackCard,1);

	if (!client) return NULL;
	obj->client = client;
	obj->jack_running = TRUE;
	obj->jack_active = FALSE;
	obj->can_process = FALSE;
	obj->write.can_process = FALSE;
	obj->write.open = FALSE;
	obj->write.init = FALSE;
	obj->write.phys_ports = NULL;
	obj->read.can_process = FALSE;
	obj->read.open = FALSE;
	obj->read.init = FALSE;
	obj->read.phys_ports = NULL;

	/* tell the JACK server to call `process()' whenever
           there is work to be done.
        */
        jack_set_process_callback (client, process, obj);

        /* tell the JACK server to call `jack_shutdown()' if
           it ever shuts down, either entirely, or if it
           just decides to stop calling us.
        */
        jack_on_shutdown (client, jack_shutdown, obj);

	jack_set_sample_rate_callback (client, samplerate, obj);

	obj->rate = jack_get_sample_rate (client);
	obj->buffer_size = jack_get_buffer_size(obj->client);
	/*
	if ((obj->write.port = jack_port_register (obj->client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0))==NULL)
	  g_warning("error while trying to register output port");
	if ((obj->read.port = jack_port_register (obj->client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0))==NULL)
	  g_warning("error while trying to register input port");
	*/
	base= SND_CARD(obj);
	snd_card_init(base);
	
#ifdef HAVE_GLIB
	base->card_name=g_strdup_printf("JACK client");
#else
	base->card_name=malloc(100);
	snprintf(base->card_name, 100, "JACK client");
#endif

	base->_probe=(SndCardOpenFunc)jack_card_probe;
	base->_open_r=(SndCardOpenFunc)jack_card_open_r;
	base->_open_w=(SndCardOpenFunc)jack_card_open_w;
	base->_can_read=(SndCardPollFunc)jack_card_can_read;
	base->_set_blocking_mode=(SndCardSetBlockingModeFunc)jack_card_set_blocking_mode;
	base->_read=(SndCardIOFunc)jack_card_read;
	base->_write=(SndCardIOFunc)jack_card_write;
	base->_close_r=(SndCardCloseFunc)jack_card_close_r;
	base->_close_w=(SndCardCloseFunc)jack_card_close_w;
	base->_set_rec_source=(SndCardMixerSetRecSourceFunc)jack_card_set_source;
	base->_set_level=(SndCardMixerSetLevelFunc)jack_card_set_level;
	base->_get_level=(SndCardMixerGetLevelFunc)jack_card_get_level;
	base->_destroy=(SndCardDestroyFunc)jack_card_destroy;
	base->_create_read_filter=(SndCardCreateFilterFunc)jack_card_create_read_filter;
	base->_create_write_filter=(SndCardCreateFilterFunc)jack_card_create_write_filter;
	
	obj->read.buffer=NULL;
	obj->write.buffer=NULL;
	obj->buffer_size = 0;
	obj->level = 1.0;
	obj->write.level = 1.0;
	obj->read.level = 1.0;

	return base;
}


gint jack_card_manager_init(SndCardManager *m, gint index)
{
  jack_client_t *client = NULL;
  char* client_name;

#ifdef DEBUG
  g_message("jack_card_manager_init");
#endif
  client_name=g_strdup_printf("linphone-%u", g_random_int());
  if ((client = jack_client_new (client_name))!= NULL)
    {
      g_message("Found Jack Daemon");
      g_free(client_name);
      m->cards[index]=jack_card_new(client);
      m->cards[index]->index=index;
      return 1;
    } else {
      g_free(client_name);
      return 0;
    }
}

#endif
