/*
 * Copyright (C) 2002-2017 the xine project
 *
 * This file is part of xine, a free video player.
 *
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * sdp/sdpplin parser.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define LOG_MODULE "sdpplin"
#define LOG_VERBOSE
/*
#define LOG
*/

#include "rmff.h"
#include "rtsp.h"
#include "sdpplin.h"
#include <xine/xineutils.h>

static char *nl(char *data) {

  char *nlptr = (data) ? strchr(data,'\n') : NULL;
  return (nlptr) ? nlptr + 1 : NULL;
}

static int filter(const char *in, const char *filter, char **out) {

  size_t flen=strlen(filter);
  size_t len;

  if (!in)
    return 0;

  len = (strchr(in,'\n')) ? (size_t)(strchr(in,'\n')-in) : strlen(in);

  if (!strncmp(in,filter,flen))
  {
    if(in[flen]=='"') flen++;
    if(in[len-1]==13) len--;
    if(in[len-1]=='"') len--;
    xine_buffer_copyin(*out, 0, in+flen, len-flen+1);
    (*out)[len-flen]=0;

    return len-flen;
  }

  return 0;
}

static void sdpplin_free_stream(sdpplin_stream_t **pp)
{
  if (*pp) {
    sdpplin_stream_t *p = *pp;
    _x_freep(&p->id);
    _x_freep(&p->stream_name);
    _x_freep(&p->mime_type);
    _x_freep(&p->mlti_data);
    _x_freep(&p->asm_rule_book);
    _x_freep(pp);
  }
}

static sdpplin_stream_t *XINE_MALLOC sdpplin_parse_stream(char **data) {

  sdpplin_stream_t *desc = calloc(1, sizeof(sdpplin_stream_t));
  char      *buf=xine_buffer_init(32);
  int       handled;

  if (filter(*data, "m=", &buf)) {
    desc->id = strdup(buf);
  } else
  {
    lprintf("sdpplin: no m= found.\n");
    free(desc);
    xine_buffer_free(buf);
    return NULL;
  }
  *data=nl(*data);

  while (*data && **data && *data[0]!='m') {
    int dlen;

    handled=0;

    if(filter(*data,"a=control:streamid=",&buf)) {
      /* This way negative values are mapped to unfeasibly high
       * values, and will be discarded afterward
       */
      unsigned long tmp = strtoul(buf, NULL, 10);
      if ( tmp > UINT16_MAX )
	lprintf("stream id out of bound: %lu\n", tmp);
      else
	desc->stream_id=tmp;
      handled=1;
      *data=nl(*data);
    }

    if(filter(*data,"a=MaxBitRate:integer;",&buf)) {
      desc->max_bit_rate=atoi(buf);
      if (!desc->avg_bit_rate)
        desc->avg_bit_rate=desc->max_bit_rate;
      handled=1;
      *data=nl(*data);
    }

    if(filter(*data,"a=MaxPacketSize:integer;",&buf)) {
      desc->max_packet_size=atoi(buf);
      if (!desc->avg_packet_size)
        desc->avg_packet_size=desc->max_packet_size;
      handled=1;
      *data=nl(*data);
    }

    if(filter(*data,"a=StartTime:integer;",&buf)) {
      desc->start_time=atoi(buf);
      handled=1;
      *data=nl(*data);
    }

    if(filter(*data,"a=Preroll:integer;",&buf)) {
      desc->preroll=atoi(buf);
      handled=1;
      *data=nl(*data);
    }

    if(filter(*data,"a=length:npt=",&buf)) {
      desc->duration=(uint32_t)(atof(buf)*1000);
      handled=1;
      *data=nl(*data);
    }

    if(filter(*data,"a=StreamName:string;",&buf)) {
      desc->stream_name=strdup(buf);
      desc->stream_name_size=strlen(desc->stream_name);
      handled=1;
      *data=nl(*data);
    }

    if(filter(*data,"a=mimetype:string;",&buf)) {
      desc->mime_type=strdup(buf);
      desc->mime_type_size=strlen(desc->mime_type);
      handled=1;
      *data=nl(*data);
    }

    if((dlen=filter(*data,"a=OpaqueData:buffer;",&buf))!=0) {
      desc->mlti_data = malloc((dlen + 2) * 3 / 4 + 4);
      desc->mlti_data_size = xine_base64_decode(buf, desc->mlti_data);
      desc->mlti_data[desc->mlti_data_size] = 0;
      if ( desc->mlti_data_size > 0 ) {
	handled=1;
	*data=nl(*data);
	lprintf("mlti_data_size: %i\n", desc->mlti_data_size);
      } else {
	free(desc->mlti_data);
	desc->mlti_data = NULL;
      }
    }

    if(filter(*data,"a=ASMRuleBook:string;",&buf)) {
      desc->asm_rule_book=strdup(buf);
      handled=1;
      *data=nl(*data);
    }

    if(!handled) {
#ifdef LOG
      int len=strchr(*data,'\n')-(*data);
      xine_buffer_copyin(buf, 0, *data, len+1);
      buf[len]=0;
      printf("libreal: sdpplin: not handled: '%s'\n", buf);
#endif
      *data=nl(*data);
    }
  }

  xine_buffer_free(buf);

  return desc;
}

sdpplin_t *sdpplin_parse(char *data) {

  sdpplin_t        *desc = calloc(1, sizeof(sdpplin_t));
  char             *buf=xine_buffer_init(32);
  int              handled;
  int              len;

  desc->stream = NULL;

  while (data && *data) {
    int dlen;

    handled=0;

    if (filter(data, "m=", &buf)) {
      sdpplin_stream_t *stream;
      if ( ! desc->stream ) {
	fprintf(stderr, "sdpplin.c: stream identifier found before stream count, skipping.");
	continue;
      }
      stream=sdpplin_parse_stream(&data);
      lprintf("got data for stream id %u\n", stream->stream_id);
      if ( stream->stream_id >= desc->stream_count ) {
	lprintf("stream id %u is greater than stream count %u\n", stream->stream_id, desc->stream_count);
        sdpplin_free_stream(&stream);
      } else {
        if (desc->stream[stream->stream_id])
          sdpplin_free_stream(&desc->stream[stream->stream_id]);
        desc->stream[stream->stream_id]=stream;
      }
      continue;
    }

    if((dlen=filter(data,"a=Title:buffer;",&buf))!=0) {
      desc->title = malloc((dlen + 2) * 3 / 4 + 4);
      len = xine_base64_decode(buf, desc->title);
      desc->title[len] = 0;
      if ( len > 0 ) {
	handled=1;
	data=nl(data);
      } else {
	free(desc->title);
	desc->title = NULL;
      }
    }

    if((dlen=filter(data,"a=Author:buffer;",&buf))!=0) {
      desc->author = malloc((dlen + 2) * 3 / 4 + 4);
      len = xine_base64_decode(buf, desc->author);
      desc->author[len] = 0;
      if ( len > 0 ) {
	handled=1;
	data=nl(data);
      } else {
	free(desc->author);
	desc->author = NULL;
      }
    }

    if((dlen=filter(data,"a=Copyright:buffer;",&buf))!=0) {
      desc->copyright = malloc((dlen + 2) * 3 / 4 + 4);
      len = xine_base64_decode(buf, desc->copyright);
      desc->copyright[len] = 0;
      if ( len > 0 ) {
	handled=1;
	data=nl(data);
      } else {
	free(desc->copyright);
	desc->copyright = NULL;
      }
    }

    if((dlen=filter(data,"a=Abstract:buffer;",&buf))!=0) {
      desc->abstract = malloc ((dlen + 2) * 3 / 4 + 4);
      len = xine_base64_decode(buf, desc->abstract);
      desc->abstract[len] = 0;
      if ( len > 0 ) {
	handled=1;
	data=nl(data);
      } else {
	free(desc->abstract);
	desc->abstract = NULL;
      }
    }

    if(filter(data,"a=StreamCount:integer;",&buf)) {
      /* This way negative values are mapped to unfeasibly high
       * values, and will be discarded afterward
       */
      unsigned long tmp = strtoul(buf, NULL, 10);
      if ( tmp > UINT16_MAX )
	lprintf("stream count out of bound: %lu\n", tmp);
      else
	desc->stream_count = tmp;
      desc->stream = calloc(desc->stream_count, sizeof(sdpplin_stream_t*));
      handled=1;
      data=nl(data);
    }

    if(filter(data,"a=Flags:integer;",&buf)) {
      desc->flags=atoi(buf);
      handled=1;
      data=nl(data);
    }

    if(!handled) {
#ifdef LOG
      int len=strchr(data,'\n')-data;
      xine_buffer_copyin(buf, 0, data, len+1);
      buf[len]=0;
      printf("libreal: sdpplin: not handled: '%s'\n", buf);
#endif
      data=nl(data);
    }
  }

  xine_buffer_free(buf);

  return desc;
}

void sdpplin_free(sdpplin_t *p) {

  if (p->stream) {
    unsigned i;
    for (i = 0; i < p->stream_count; i++) {
      sdpplin_free_stream(&p->stream[i]);
    }
    _x_freep(&p->stream);
  }

  _x_freep(&p->title);
  _x_freep(&p->author);
  _x_freep(&p->copyright);
  _x_freep(&p->abstract);

  free(p);
}

