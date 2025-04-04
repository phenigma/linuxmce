/*
 * Copyright (C) 2000-2017 the xine project
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
 * code based on old libsputext/xine_decoder.c
 *
 * code based on mplayer module:
 *
 * Subtitle reader with format autodetection
 *
 * Written by laaz
 * Some code cleanup & realloc() by A'rpi/ESP-team
 * dunnowhat sub format by szabi
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#define LOG_MODULE "demux_sputext"
#define LOG_VERBOSE
/*
#define LOG
*/

#include <xine/xine_internal.h>
#include <xine/xineutils.h>
#include <xine/demux.h>

#define ERR           (void *)-1
#define SUB_MAX_TEXT  5
#define SUB_BUFSIZE   1024
#define LINE_LEN      1000
#define LINE_LEN_QUOT "1000"

/*
 *  Demuxer typedefs
 */

typedef struct {

  int lines;

  long start; /* csecs */
  long end;   /* csecs */

  char *text[SUB_MAX_TEXT];

} subtitle_t;


typedef struct {

  demux_plugin_t     demux_plugin;
  xine_stream_t     *stream;
  input_plugin_t    *input;

  int                status;

  char               buf[SUB_BUFSIZE];
  off_t              buflen;

  float              mpsub_position;

  int                uses_time;
  int                errs;
  subtitle_t        *subtitles;
  int                num;            /* number of subtitle structs */
  int                cur;            /* current subtitle           */
  int                format;         /* constants see below        */
  char               next_line[SUB_BUFSIZE]; /* a buffer for next line read from file */

  char              *encoding; /* charset. NULL if unknown. currently only "utf-8" autodetected. */

} demux_sputext_t;

typedef struct demux_sputext_class_s {

  demux_class_t      demux_class;

  int                max_timeout;  /* default timeout of hidding subtitles */

} demux_sputext_class_t;

void *init_sputext_demux_class (xine_t *xine, void *data);

/*
 * Demuxer code start
 */

#define FORMAT_UNKNOWN   -1
#define FORMAT_MICRODVD   0
#define FORMAT_SUBRIP     1
#define FORMAT_SUBVIEWER  2
#define FORMAT_SAMI       3
#define FORMAT_VPLAYER    4
#define FORMAT_RT         5
#define FORMAT_SSA        6 /* Sub Station Alpha */
#define FORMAT_PJS	  7
#define FORMAT_MPSUB      8
#define FORMAT_AQTITLE    9
#define FORMAT_JACOBSUB   10
#define FORMAT_SUBVIEWER2 11
#define FORMAT_SUBRIP09   12
#define FORMAT_MPL2       13 /*Mplayer sub 2 ?*/

static int eol(char p) {
  return (p=='\r' || p=='\n' || p=='\0');
}

static inline void trail_space(char *s) {
  while (isspace(*s)) {
    char *copy = s;
    do {
      copy[0] = copy[1];
      copy++;
    } while(*copy);
  }
  size_t i = strlen(s) - 1;
  while (i > 0 && isspace(s[i]))
    s[i--] = '\0';
}

/*
 * Reimplementation of fgets() using the input->read() method.
 */
static char *read_line_from_input(demux_sputext_t *this, char *line, off_t len) {
  off_t nread = 0;
  char *s;
  int linelen;

  if ((len - this->buflen) > 512 && len < SUB_BUFSIZE) {
    if((nread = this->input->read(this->input,
				  &this->buf[this->buflen], len - this->buflen)) < 0) {
      xprintf(this->stream->xine, XINE_VERBOSITY_DEBUG, "read failed.\n");
      return NULL;
    }
  }

  this->buflen += nread;
  this->buf[this->buflen] = '\0';

  s = strchr(this->buf, '\n');

  if (line && (s || this->buflen)) {

    linelen = s ? (s - this->buf) + 1 : this->buflen;

    memcpy(line, this->buf, linelen);
    line[linelen] = '\0';

    memmove(this->buf, &this->buf[linelen], SUB_BUFSIZE - linelen);
    this->buflen -= linelen;

    return line;
  }

  return NULL;
}


static subtitle_t *sub_read_line_sami(demux_sputext_t *this, subtitle_t *current) {

  static char line[LINE_LEN + 1];
  static char *s = NULL;
  char text[LINE_LEN + 1], *p, *q;
  int state;

  p = NULL;
  current->lines = current->start = 0;
  current->end = -1;
  state = 0;

  /* read the first line */
  if (!s)
    if (!(s = read_line_from_input(this, line, LINE_LEN))) return 0;

  do {
    switch (state) {

    case 0: /* find "START=" */
      s = strstr (s, "Start=");
      if (s) {
	current->start = strtol (s + 6, &s, 0) / 10;
	state = 1; continue;
      }
      break;

    case 1: /* find "<P" */
      if ((s = strstr (s, "<P"))) { s += 2; state = 2; continue; }
      break;

    case 2: /* find ">" */
      if ((s = strchr (s, '>'))) { s++; state = 3; p = text; continue; }
      break;

    case 3: /* get all text until '<' appears */
      if (*s == '\0') { break; }
      else if (*s == '<') { state = 4; }
      else if (!strncasecmp (s, "&nbsp;", 6)) { *p++ = ' '; s += 6; }
      else if (*s == '\r') { s++; }
      else if (!strncasecmp (s, "<br>", 4) || *s == '\n') {
	*p = '\0'; p = text; trail_space (text);
	if (text[0] != '\0')
	  current->text[current->lines++] = strdup (text);
	if (*s == '\n') s++; else s += 4;
      }
      else *p++ = *s++;
      continue;

    case 4: /* get current->end or skip <TAG> */
      q = strstr (s, "Start=");
      if (q) {
	current->end = strtol (q + 6, &q, 0) / 10 - 1;
	*p = '\0'; trail_space (text);
	if (text[0] != '\0')
	  current->text[current->lines++] = strdup (text);
	if (current->lines > 0) { state = 99; break; }
	state = 0; continue;
      }
      s = strchr (s, '>');
      if (s) { s++; state = 3; continue; }
      break;
    }

    /* read next line */
    if (state != 99 && !(s = read_line_from_input (this, line, LINE_LEN)))
      return 0;

  } while (state != 99);

  return current;
}


static char *sub_readtext(char *source, char **dest) {
  int len=0;
  char *p=source;

  while ( !eol(*p) && *p!= '|' ) {
    p++,len++;
  }

  *dest = strndup(source, len);

  while (*p=='\r' || *p=='\n' || *p=='|')
    p++;

  if (*p)  return p;  /* not-last text field */
  else return NULL;   /* last text field     */
}

static subtitle_t *sub_read_line_microdvd(demux_sputext_t *this, subtitle_t *current) {

  char line[LINE_LEN + 1];
  char line2[LINE_LEN + 1];
  char *p, *next;
  int i;

  memset (current, 0, sizeof(subtitle_t));

  current->end=-1;
  do {
    if (!read_line_from_input (this, line, LINE_LEN)) return NULL;
  } while ((sscanf (line, "{%ld}{}%" LINE_LEN_QUOT "[^\r\n]", &(current->start), line2) !=2) &&
           (sscanf (line, "{%ld}{%ld}%" LINE_LEN_QUOT "[^\r\n]", &(current->start), &(current->end),line2) !=3)
	  );

  p=line2;

  next=p, i=0;
  while ((next =sub_readtext (next, &(current->text[i])))) {
    if (current->text[i]==ERR) return ERR;
    i++;
    if (i>=SUB_MAX_TEXT) {
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "Too many lines in a subtitle\n");
      current->lines=i;
      return current;
    }
  }
  current->lines= ++i;

  return current;
}

static subtitle_t *sub_read_line_subviewer(demux_sputext_t *this, subtitle_t *current) {

  char line[LINE_LEN + 1];
  int a1,a2,a3,a4,b1,b2,b3,b4;
  char *p=NULL, *q=NULL;
  int len;

  memset (current, 0, sizeof(subtitle_t));

  while (1) {
    if (!read_line_from_input(this, line, LINE_LEN)) return NULL;
    if (sscanf (line, "%d:%d:%d.%d,%d:%d:%d.%d",&a1,&a2,&a3,&a4,&b1,&b2,&b3,&b4) < 8) {
      if (sscanf (line, "%d:%d:%d,%d,%d:%d:%d,%d",&a1,&a2,&a3,&a4,&b1,&b2,&b3,&b4) < 8)
        continue;
    }
    current->start = a1*360000+a2*6000+a3*100+a4;
    current->end   = b1*360000+b2*6000+b3*100+b4;

    if (!read_line_from_input(this, line, LINE_LEN))
      return NULL;

    p=q=line;
    for (current->lines=1; current->lines <= SUB_MAX_TEXT; current->lines++) {
      for (q=p,len=0; *p && *p!='\r' && *p!='\n' && *p!='|' && strncasecmp(p,"[br]",4); p++,len++);
      current->text[current->lines-1] = strndup(q, len);
      if (!current->text[current->lines-1]) return ERR;
      if (!*p || *p=='\r' || *p=='\n') break;
      if (*p=='[') while (*p++!=']');
      if (*p=='|') p++;
    }
    if (current->lines > SUB_MAX_TEXT) current->lines = SUB_MAX_TEXT;
    break;
  }
  return current;
}

static subtitle_t *sub_read_line_subrip(demux_sputext_t *this,subtitle_t *current) {
  char line[LINE_LEN + 1];
  int a1,a2,a3,a4,b1,b2,b3,b4;
  int i,end_sub;

  memset(current,0,sizeof(subtitle_t));
  do {
    if(!read_line_from_input(this,line,LINE_LEN))
      return NULL;
    i = sscanf(line,"%d:%d:%d%*[,.]%d --> %d:%d:%d%*[,.]%d",&a1,&a2,&a3,&a4,&b1,&b2,&b3,&b4);
  } while(i < 8);
  current->start = a1*360000+a2*6000+a3*100+a4/10;
  current->end   = b1*360000+b2*6000+b3*100+b4/10;
  i=0;
  end_sub=0;
  do {
    char *p; /* pointer to the curently read char */
    char temp_line[SUB_BUFSIZE]; /* subtitle line that will be transfered to current->text[i] */
    int temp_index; /* ... and its index wich 'points' to the first EMPTY place -> last read char is at temp_index-1 if temp_index>0 */
    temp_line[SUB_BUFSIZE-1]='\0'; /* just in case... */
    if(!read_line_from_input(this,line,LINE_LEN)) {
      if(i)
        break; /* if something was read, transmit it */
      else
        return NULL; /* if not, repport EOF */
    }
    for(temp_index=0,p=line;*p!='\0' && !end_sub && temp_index<SUB_BUFSIZE && i<SUB_MAX_TEXT;p++) {
      switch(*p) {
        case '\\':
          if(*(p+1)=='N' || *(p+1)=='n') {
            temp_line[temp_index++]='\0'; /* end of curent line */
            p++;
          } else
            temp_line[temp_index++]=*p;
          break;
        case '\r': /* just ignore '\r's */
          break;
        case '\n':
          temp_line[temp_index++]='\0';
          break;
        default:
          temp_line[temp_index++]=*p;
          break;
      }
      if(temp_index>0) {
        if(temp_index==SUB_BUFSIZE)
          xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "Too many characters in a subtitle line\n");
        if(temp_line[temp_index-1]=='\0' || temp_index==SUB_BUFSIZE) {
          if(temp_index>1) { /* more than 1 char (including '\0') -> that is a valid one */
	    /* temp_index<=SUB_BUFSIZE is always true here */
            current->text[i] = strndup(temp_line, temp_index);
            if(!current->text[i])
              return ERR;
            i++;
            temp_index=0;
          } else
            end_sub=1;
        }
      }
    }
  } while(i<SUB_MAX_TEXT && !end_sub);
  if(i>=SUB_MAX_TEXT)
    xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "Too many lines in a subtitle\n");
  current->lines=i;
  return current;
}

static subtitle_t *sub_read_line_vplayer(demux_sputext_t *this,subtitle_t *current) {
  char line[LINE_LEN + 1];
  int a1,a2,a3,b1,b2,b3;
  char *p=NULL, *next, *p2;
  int i;

  memset (current, 0, sizeof(subtitle_t));

  while (!current->text[0]) {
    if( this->next_line[0] == '\0' ) { /* if the buffer is empty.... */
      if( !read_line_from_input(this, line, LINE_LEN) ) return NULL;
    } else {
      /* ... get the current line from buffer. */
      strncpy( line, this->next_line, LINE_LEN);
      line[LINE_LEN] = '\0'; /* I'm scared. This makes me feel better. */
      this->next_line[0] = '\0'; /* mark the buffer as empty. */
    }
    /* Initialize buffer with next line */
    if( ! read_line_from_input( this, this->next_line, LINE_LEN) ) {
      this->next_line[0] = '\0';
      return NULL;
    }
    if( (sscanf( line,            "%d:%d:%d:", &a1, &a2, &a3) < 3) ||
        (sscanf( this->next_line, "%d:%d:%d:", &b1, &b2, &b3) < 3) )
      continue;
    current->start = a1*360000+a2*6000+a3*100;
    current->end   = b1*360000+b2*6000+b3*100;
    if ((current->end - current->start) > LINE_LEN)
      current->end = current->start + LINE_LEN; /* not too long though.  */
    /* teraz czas na wkopiowanie stringu */
    p=line;
    /* finds the body of the subtitle_t */
    for (i=0; i<3; i++){
      p2=strchr( p, ':');
      if( p2 == NULL ) break;
      p=p2+1;
    }

    next=p;
    i=0;
    while( (next = sub_readtext( next, &(current->text[i]))) ) {
      if (current->text[i]==ERR)
        return ERR;
      i++;
      if (i>=SUB_MAX_TEXT) {
        xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "Too many lines in a subtitle\n");
        current->lines=i;
        return current;
      }
    }
    current->lines=++i;
  }
  return current;
}

static subtitle_t *sub_read_line_rt(demux_sputext_t *this,subtitle_t *current) {
  /*
   * TODO: This format uses quite rich (sub/super)set of xhtml
   * I couldn't check it since DTD is not included.
   * WARNING: full XML parses can be required for proper parsing
   */
  char line[LINE_LEN + 1];
  int a1,a2,a3,a4,b1,b2,b3,b4;
  char *p=NULL,*next=NULL;
  int i,len,plen;

  memset (current, 0, sizeof(subtitle_t));

  while (!current->text[0]) {
    if (!read_line_from_input(this, line, LINE_LEN)) return NULL;
    /*
     * TODO: it seems that format of time is not easily determined, it may be 1:12, 1:12.0 or 0:1:12.0
     * to describe the same moment in time. Maybe there are even more formats in use.
     */
    if ((len=sscanf (line, "<Time Begin=\"%d:%d:%d.%d\" End=\"%d:%d:%d.%d\"",&a1,&a2,&a3,&a4,&b1,&b2,&b3,&b4)) < 8)

      plen=a1=a2=a3=a4=b1=b2=b3=b4=0;
    if (
	((len=sscanf (line, "<%*[tT]ime %*[bB]egin=\"%d:%d\" %*[Ee]nd=\"%d:%d\"%*[^<]<clear/>%n",&a2,&a3,&b2,&b3,&plen)) < 4) &&
	((len=sscanf (line, "<%*[tT]ime %*[bB]egin=\"%d:%d\" %*[Ee]nd=\"%d:%d.%d\"%*[^<]<clear/>%n",&a2,&a3,&b2,&b3,&b4,&plen)) < 5) &&
	/*	((len=sscanf (line, "<%*[tT]ime %*[bB]egin=\"%d:%d.%d\" %*[Ee]nd=\"%d:%d\"%*[^<]<clear/>%n",&a2,&a3,&a4,&b2,&b3,&plen)) < 5) && */
	((len=sscanf (line, "<%*[tT]ime %*[bB]egin=\"%d:%d.%d\" %*[Ee]nd=\"%d:%d.%d\"%*[^<]<clear/>%n",&a2,&a3,&a4,&b2,&b3,&b4,&plen)) < 6) &&
	((len=sscanf (line, "<%*[tT]ime %*[bB]egin=\"%d:%d:%d.%d\" %*[Ee]nd=\"%d:%d:%d.%d\"%*[^<]<clear/>%n",&a1,&a2,&a3,&a4,&b1,&b2,&b3,&b4,&plen)) < 8)
	)
      continue;
    current->start = a1*360000+a2*6000+a3*100+a4/10;
    current->end   = b1*360000+b2*6000+b3*100+b4/10;
    p=line;	p+=plen;i=0;
    /* TODO: I don't know what kind of convention is here for marking multiline subs, maybe <br/> like in xml? */
    next = strstr(line,"<clear/>")+8;i=0;
    while ((next =sub_readtext (next, &(current->text[i])))) {
      if (current->text[i]==ERR)
	return ERR;
      i++;
      if (i>=SUB_MAX_TEXT) {
	xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "Too many lines in a subtitle\n");
	current->lines=i;
	return current;
      }
    }
    current->lines=i+1;
  }
  return current;
}

static subtitle_t *sub_read_line_ssa(demux_sputext_t *this,subtitle_t *current) {
  int comma;
  static int max_comma = 32; /* let's use 32 for the case that the */
  /*  amount of commas increase with newer SSA versions */

  int hour1, min1, sec1, hunsec1, hour2, min2, sec2, hunsec2, nothing;
  int num;
  char line[LINE_LEN + 1], line3[LINE_LEN + 1], *line2;
  char *tmp;

  do {
    if (!read_line_from_input(this, line, LINE_LEN)) return NULL;
  } while (sscanf (line, "Dialogue: Marked=%d,%d:%d:%d.%d,%d:%d:%d.%d,"
		   "%[^\n\r]", &nothing,
		   &hour1, &min1, &sec1, &hunsec1,
		   &hour2, &min2, &sec2, &hunsec2,
		   line3) < 9
	   &&
	   sscanf (line, "Dialogue: %d,%d:%d:%d.%d,%d:%d:%d.%d,"
		   "%[^\n\r]", &nothing,
		   &hour1, &min1, &sec1, &hunsec1,
		   &hour2, &min2, &sec2, &hunsec2,
		   line3) < 9	    );

  line2=strchr(line3, ',');
  if (!line2)
    return NULL;

  for (comma = 4; comma < max_comma; comma ++)
    {
      tmp = line2;
      if(!(tmp=strchr(++tmp, ','))) break;
      if(*(++tmp) == ' ') break;
      /* a space after a comma means we're already in a sentence */
      line2 = tmp;
    }

  if(comma < max_comma)max_comma = comma;
  /* eliminate the trailing comma */
  if(*line2 == ',') line2++;

  current->lines=0;num=0;
  current->start = 360000*hour1 + 6000*min1 + 100*sec1 + hunsec1;
  current->end   = 360000*hour2 + 6000*min2 + 100*sec2 + hunsec2;

  while (((tmp=strstr(line2, "\\n")) != NULL) || ((tmp=strstr(line2, "\\N")) != NULL) ){
    current->text[num] = strndup(line2, tmp-line2);
    line2=tmp+2;
    num++;
    current->lines++;
    if (current->lines >=  SUB_MAX_TEXT) return current;
  }

  current->text[num]=strdup(line2);
  current->lines++;

  return current;
}

/* Sylvain "Skarsnik" Colinet <scolinet@gmail.com>
 * From MPlayer subreader.c :
 *
 * PJS subtitles reader.
 * That's the "Phoenix Japanimation Society" format.
 * I found some of them in http://www.scriptsclub.org/ (used for anime).
 * The time is in tenths of second.
 *
 * by set, based on code by szabi (dunnowhat sub format ;-)
 */

static subtitle_t *sub_read_line_pjs (demux_sputext_t *this, subtitle_t *current) {
  char line[LINE_LEN + 1];
  char text[LINE_LEN + 1];
  char *s, *d;

  memset (current, 0, sizeof(subtitle_t));

  if (!read_line_from_input(this, line, LINE_LEN))
    return NULL;
  for (s = line; *s && isspace(*s); s++);
  if (*s == 0)
    return NULL;
  if (sscanf (line, "%ld,%ld,", &(current->start),
	      &(current->end)) <2)
    return ERR;
  /* the files I have are in tenths of second */
  current->start *= 10;
  current->end *= 10;

  /* walk to the beggining of the string */
  for (; *s; s++) if (*s==',') break;
  if (*s) {
      for (s++; *s; s++) if (*s==',') break;
      if (*s) s++;
  }
  if (*s!='"') {
       return ERR;
  }
  /* copy the string to the text buffer */
  for (s++, d=text; *s && *s!='"'; s++, d++)
      *d=*s;
  *d=0;
  current->text[0] = strdup(text);
  current->lines = 1;

  return current;
}

static subtitle_t *sub_read_line_mpsub (demux_sputext_t *this, subtitle_t *current) {
  char line[LINE_LEN + 1];
  float a,b;
  int num=0;
  char *p, *q;

  do {
    if (!read_line_from_input(this, line, LINE_LEN))
      return NULL;
  } while (sscanf (line, "%f %f", &a, &b) !=2);

  this->mpsub_position += (a*100.0);
  current->start = (int) this->mpsub_position;
  this->mpsub_position += (b*100.0);
  current->end = (int) this->mpsub_position;

  while (num < SUB_MAX_TEXT) {
    if (!read_line_from_input(this, line, LINE_LEN))
      return NULL;

    p=line;
    while (isspace(*p))
      p++;

    if (eol(*p) && num > 0)
      return current;

    if (eol(*p))
      return NULL;

    for (q=p; !eol(*q); q++);
    *q='\0';
    if (strlen(p)) {
      current->text[num]=strdup(p);
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, ">%s<\n",p);
      current->lines = ++num;
    } else {
      if (num)
	return current;
      else
	return NULL;
    }
  }

  return NULL;
}

static subtitle_t *sub_read_line_aqt (demux_sputext_t *this, subtitle_t *current) {
  char line[LINE_LEN + 1];

  memset (current, 0, sizeof(subtitle_t));

  while (1) {
    /* try to locate next subtitle_t */
    if (!read_line_from_input(this, line, LINE_LEN))
      return NULL;
    if (!(sscanf (line, "-->> %ld", &(current->start)) <1))
      break;
  }

  if (!read_line_from_input(this, line, LINE_LEN))
    return NULL;

  sub_readtext((char *) &line,&current->text[0]);
  current->lines = 1;
  current->end = -1;

  if (!read_line_from_input(this, line, LINE_LEN))
    return current;;

  sub_readtext((char *) &line,&current->text[1]);
  current->lines = 2;

  if ((current->text[0][0]==0) && (current->text[1][0]==0)) {
    return NULL;
  }

  return current;
}

static subtitle_t *sub_read_line_jacobsub(demux_sputext_t *this, subtitle_t *current) {
    char line1[LINE_LEN], line2[LINE_LEN], directive[LINE_LEN], *p, *q;
    unsigned a1, a2, a3, a4, b1, b2, b3, b4, comment = 0;
    static unsigned jacoTimeres = 30;
    static int jacoShift = 0;

    memset(current, 0, sizeof(subtitle_t));
    memset(line1, 0, LINE_LEN);
    memset(line2, 0, LINE_LEN);
    memset(directive, 0, LINE_LEN);
    while (!current->text[0]) {
	if (!read_line_from_input(this, line1, LINE_LEN)) {
	    return NULL;
	}
	if (sscanf
	    (line1, "%u:%u:%u.%u %u:%u:%u.%u %" LINE_LEN_QUOT "[^\n\r]", &a1, &a2, &a3, &a4,
	     &b1, &b2, &b3, &b4, line2) < 9) {
	    if (sscanf(line1, "@%u @%u %" LINE_LEN_QUOT "[^\n\r]", &a4, &b4, line2) < 3) {
		if (line1[0] == '#') {
		    int hours = 0, minutes = 0, seconds, delta, inverter =
			1;
		    unsigned units = jacoShift;
		    switch (line1[1]) {
		    case 'S':
		    case 's':
			if (isalpha(line1[2])) {
			    delta = 6;
			} else {
			    delta = 2;
			}
			if (sscanf(&line1[delta], "%d", &hours)) {
			    if (hours < 0) {
				hours *= -1;
				inverter = -1;
			    }
			    if (sscanf(&line1[delta], "%*d:%d", &minutes)) {
				if (sscanf
				    (&line1[delta], "%*d:%*d:%d",
				     &seconds)) {
				    sscanf(&line1[delta], "%*d:%*d:%*d.%d",
					   &units);
				} else {
				    hours = 0;
				    sscanf(&line1[delta], "%d:%d.%d",
					   &minutes, &seconds, &units);
				    minutes *= inverter;
				}
			    } else {
				hours = minutes = 0;
				sscanf(&line1[delta], "%d.%d", &seconds,
				       &units);
				seconds *= inverter;
			    }
			    jacoShift =
				((hours * 3600 + minutes * 60 +
				  seconds) * jacoTimeres +
				 units) * inverter;
			}
			break;
		    case 'T':
		    case 't':
			if (isalpha(line1[2])) {
			    delta = 8;
			} else {
			    delta = 2;
			}
			sscanf(&line1[delta], "%u", &jacoTimeres);
			break;
		    }
		}
		continue;
	    } else {
		current->start =
		    (unsigned long) ((a4 + jacoShift) * 100.0 /
				     jacoTimeres);
		current->end =
		    (unsigned long) ((b4 + jacoShift) * 100.0 /
				     jacoTimeres);
	    }
	} else {
	    current->start =
		(unsigned
		 long) (((a1 * 3600 + a2 * 60 + a3) * jacoTimeres + a4 +
			 jacoShift) * 100.0 / jacoTimeres);
	    current->end =
		(unsigned
		 long) (((b1 * 3600 + b2 * 60 + b3) * jacoTimeres + b4 +
			 jacoShift) * 100.0 / jacoTimeres);
	}
	current->lines = 0;
	p = line2;
	while ((*p == ' ') || (*p == '\t')) {
	    ++p;
	}
	if (isalpha(*p)||*p == '[') {
	    if (sscanf(p, "%s %" LINE_LEN_QUOT "[^\n\r]", directive, line1) < 2)
		return ERR;
	    if ((strcasestr(directive, "RDB") != NULL)
		|| (strcasestr(directive, "RDC") != NULL)
		|| (strcasestr(directive, "RLB") != NULL)
		|| (strcasestr(directive, "RLG") != NULL)) {
		continue;
	    }
	    /* no alignment */
#if 0
	    if (strcasestr(directive, "JL") != NULL) {
		current->alignment = SUB_ALIGNMENT_HLEFT;
	    } else if (strcasestr(directive, "JR") != NULL) {
		current->alignment = SUB_ALIGNMENT_HRIGHT;
	    } else {
		current->alignment = SUB_ALIGNMENT_HCENTER;
	    }
#endif
	    strcpy(line2, line1);
	    p = line2;
	}
	for (q = line1; (!eol(*p)) && (current->lines < SUB_MAX_TEXT); ++p) {
	    switch (*p) {
	    case '{':
		comment++;
		break;
	    case '}':
		if (comment) {
		    --comment;
		    /* the next line to get rid of a blank after the comment */
		    if ((*(p + 1)) == ' ')
			p++;
		}
		break;
	    case '~':
		if (!comment) {
		    *q = ' ';
		    ++q;
		}
		break;
	    case ' ':
	    case '\t':
		if ((*(p + 1) == ' ') || (*(p + 1) == '\t'))
		    break;
		if (!comment) {
		    *q = ' ';
		    ++q;
		}
		break;
	    case '\\':
		if (*(p + 1) == 'n') {
		    *q = '\0';
		    q = line1;
		    current->text[current->lines++] = strdup(line1);
		    ++p;
		    break;
		}
		if ((*(p + 1) == 'C') || (*(p + 1) == 'c') ||
		    (*(p + 1) == 'F') || (*(p + 1) == 'f')) {
		    ++p,++p;
		    break;
		}
		if ((*(p + 1) == 'B') || (*(p + 1) == 'b') ||
		    /* actually this means "insert current date here" */
		    (*(p + 1) == 'D') ||
		    (*(p + 1) == 'I') || (*(p + 1) == 'i') ||
		    (*(p + 1) == 'N') ||
		    /* actually this means "insert current time here" */
		    (*(p + 1) == 'T') ||
		    (*(p + 1) == 'U') || (*(p + 1) == 'u')) {
		    ++p;
		    break;
		}
		if ((*(p + 1) == '\\') ||
		    (*(p + 1) == '~') || (*(p + 1) == '{')) {
		    ++p;
		} else if (eol(*(p + 1))) {
		    if (!read_line_from_input(this, directive, LINE_LEN))
			return NULL;
		    trail_space(directive);
		    strncat(line2, directive,
			    ((LINE_LEN > 511) ? LINE_LEN-1 : 511)
			    - strlen(line2));
		    break;
		}
	    default:
		if (!comment) {
		    *q = *p;
		    ++q;
		}
	    }
	}
	*q = '\0';
        if (current->lines < SUB_MAX_TEXT)
	    current->text[current->lines] = strdup(line1);
        else
            xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "Too many lines in a subtitle\n");
    }
    current->lines++;
    return current;
}

static subtitle_t *sub_read_line_subviewer2(demux_sputext_t *this, subtitle_t *current) {
    char line[LINE_LEN+1];
    int a1,a2,a3,a4;
    char *p=NULL;
    int i,len;

    while (!current->text[0]) {
        if (!read_line_from_input(this, line, LINE_LEN)) return NULL;
	if (line[0]!='{')
	    continue;
        if ((len=sscanf (line, "{T %d:%d:%d:%d",&a1,&a2,&a3,&a4)) < 4)
            continue;
        current->start = a1*360000+a2*6000+a3*100+a4/10;
        for (i=0; i<SUB_MAX_TEXT;) {
            if (!read_line_from_input(this, line, LINE_LEN)) break;
            if (line[0]=='}') break;
            len=0;
            for (p=line; *p!='\n' && *p!='\r' && *p; ++p,++len);
            if (len) {
	      current->text[i] = strndup(line, len);
	      if (!current->text[i]) return ERR;
	      ++i;
            } else {
                break;
            }
        }
        current->lines=i;
    }
    return current;
}

static subtitle_t *sub_read_line_subrip09 (demux_sputext_t *this, subtitle_t *current) {
  char line[LINE_LEN + 1];
  char *next;
  int h, m, s;
  int i;

  memset (current, 0, sizeof(subtitle_t));

  do {
    if (!read_line_from_input (this, line, LINE_LEN)) return NULL;
  } while (sscanf (line, "[%d:%d:%d]", &h, &m, &s) != 3);

  if (!read_line_from_input (this, line, LINE_LEN)) return NULL;

  current->start = 360000 * h + 6000 * m + 100 * s;
  current->end = -1;

  next=line;
  i=0;
  while ((next = sub_readtext (next, &(current->text[i])))) {
    if (current->text[i]==ERR) return ERR;
    i++;
    if (i>=SUB_MAX_TEXT) {
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "Too many lines in a subtitle\n");
      current->lines=i;
      return current;
    }
  }
  current->lines= ++i;

  return current;
}

/* Code from subreader.c of MPlayer
** Sylvain "Skarsnik" Colinet <scolinet@gmail.com>
*/

static subtitle_t *sub_read_line_mpl2(demux_sputext_t *this, subtitle_t *current) {
  char line[LINE_LEN+1];
  char line2[LINE_LEN+1];
  char *p, *next;
  int i;

  memset (current, 0, sizeof(subtitle_t));
  do {
     if (!read_line_from_input (this, line, LINE_LEN)) return NULL;
  } while ((sscanf (line,
		      "[%ld][%ld]%[^\r\n]",
		      &(current->start), &(current->end), line2) < 3));
  current->start *= 10;
  current->end *= 10;
  p=line2;

  next=p, i=0;
  while ((next = sub_readtext (next, &(current->text[i])))) {
      if (current->text[i] == ERR) {return ERR;}
      i++;
      if (i >= SUB_MAX_TEXT) {
        xprintf(this->stream->xine, XINE_VERBOSITY_DEBUG, "Too many lines in a subtitle\n");
	current->lines = i;
	return current;
      }
    }
  current->lines= ++i;

  return current;
}



static int sub_autodetect (demux_sputext_t *this) {

  char line[LINE_LEN + 1];
  int  i, j=0;
  char p;

  while (j < 100) {
    j++;
    if (!read_line_from_input(this, line, LINE_LEN))
      return FORMAT_UNKNOWN;

    if ((sscanf (line, "{%d}{}", &i)==1) ||
        (sscanf (line, "{%d}{%d}", &i, &i)==2)) {
      this->uses_time=0;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "microdvd subtitle format detected\n");
      return FORMAT_MICRODVD;
    }

    if (sscanf (line, "%d:%d:%d%*[,.]%d --> %d:%d:%d%*[,.]%d", &i, &i, &i, &i, &i, &i, &i, &i)==8) {
      this->uses_time=1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "subrip subtitle format detected\n");
      return FORMAT_SUBRIP;
    }

    if (sscanf (line, "%d:%d:%d.%d,%d:%d:%d.%d",     &i, &i, &i, &i, &i, &i, &i, &i)==8){
      this->uses_time=1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "subviewer subtitle format detected\n");
      return FORMAT_SUBVIEWER;
    }

    if (sscanf (line, "%d:%d:%d,%d,%d:%d:%d,%d",     &i, &i, &i, &i, &i, &i, &i, &i)==8){
      this->uses_time=1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "subviewer subtitle format detected\n");
      return FORMAT_SUBVIEWER;
    }

    if (strstr (line, "<SAMI>")) {
      this->uses_time=1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "sami subtitle format detected\n");
      return FORMAT_SAMI;
    }
    if (sscanf (line, "%d:%d:%d:",     &i, &i, &i )==3) {
      this->uses_time=1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "vplayer subtitle format detected\n");
      return FORMAT_VPLAYER;
    }
    /*
     * A RealText format is a markup language, starts with <window> tag,
     * options (behaviour modifiers) are possible.
     */
    if ( !strcasecmp(line, "<window") ) {
      this->uses_time=1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "rt subtitle format detected\n");
      return FORMAT_RT;
    }
    if ((!memcmp(line, "Dialogue: Marked", 16)) || (!memcmp(line, "Dialogue: ", 10))) {
      this->uses_time=1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "ssa subtitle format detected\n");
      return FORMAT_SSA;
    }
    if (sscanf (line, "%d,%d,\"%c", &i, &i, (char *) &i) == 3) {
      this->uses_time=0;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "pjs subtitle format detected\n");
      return FORMAT_PJS;
    }
    if (sscanf (line, "FORMAT=%d", &i) == 1) {
      this->uses_time=0;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "mpsub subtitle format detected\n");
      return FORMAT_MPSUB;
    }
    if (sscanf (line, "FORMAT=TIM%c", &p)==1 && p=='E') {
      this->uses_time=1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "mpsub subtitle format detected\n");
      return FORMAT_MPSUB;
    }
    if (strstr (line, "-->>")) {
      this->uses_time=0;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "aqtitle subtitle format detected\n");
      return FORMAT_AQTITLE;
    }
    if (sscanf(line, "@%d @%d", &i, &i) == 2 ||
	sscanf(line, "%d:%d:%d.%d %d:%d:%d.%d", &i, &i, &i, &i, &i, &i, &i, &i) == 8) {
      this->uses_time = 1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "jacobsub subtitle format detected\n");
      return FORMAT_JACOBSUB;
    }
    if (sscanf(line, "{T %d:%d:%d:%d",&i, &i, &i, &i) == 4) {
      this->uses_time = 1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "subviewer 2.0 subtitle format detected\n");
      return FORMAT_SUBVIEWER2;
    }
    if (sscanf(line, "[%d:%d:%d]", &i, &i, &i) == 3) {
      this->uses_time = 1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "subrip 0.9 subtitle format detected\n");
      return FORMAT_SUBRIP09;
    }

    if (sscanf (line, "[%d][%d]", &i, &i) == 2) {
      this->uses_time = 1;
      xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "mpl2 subtitle format detected\n");
      return FORMAT_MPL2;
    }
  }
  return FORMAT_UNKNOWN;  /* too many bad lines */
}

static int detect_utf8(subtitle_t *subs, int num_subs)
{
  /* return:
     -1: unknown (ASCII?)
      0: not valid utf-8
      1: valid utf-8
  */
  int i, l, utf8 = -1;

  for (i = 0; i < num_subs; i++) {
    for (l = 0; l < subs[i].lines && subs[i].text[l]; l++) {
      const uint8_t *c = subs[i].text[l];
      for (; *c; c++) {
        if (*c & 0x80) {
          if ( (c[0]>=0xC2 && c[0]<=0xDF) && (c[1]>=0x80 && c[1]<=0xBF) ) {
            /* valid 2-byte */
            utf8 = 1;
            c++;
          } else if ( ( c[0]==0xE0 && (c[1]>=0xA0 && c[1]<=0xBF) && (c[2]>=0x80 && c[1]<=0xBF)) ||
                      ( (c[0]>=0xE1 && c[0]<=0xEC) && (c[1]>=0x80 && c[1]<=0xBF) && (c[2]>=0x80 && c[1]<=0xBF)) ||
                      ( c[0]==0xED && (c[1]>=0x80 && c[1]<=0x9F) && (c[2]>=0x80 && c[1]<=0xBF))  ||
                      ( c[0]==0xEE && (c[1]>=0xA4 && c[1]<=0xBF) && (c[2]>=0x80 && c[1]<=0xBF) ) ||
                      ( c[0]==0xEF && (c[1]>=0xA4 && c[1]<=0xBF) && (c[2]>=0x80 && c[1]<=0xBF) )) {
            /* valid 3-byte */
            utf8 = 1;
            c += 2;
          } else {
            /* TODO: 4-byte not checked */
            return 0;
          }
        }
      }
    }
  }

  return utf8;
}

static subtitle_t *sub_read_file (demux_sputext_t *this) {

  int n_max;
  int timeout;
  subtitle_t *first;
  subtitle_t * (*func[])(demux_sputext_t *this,subtitle_t *dest)=
  {
    sub_read_line_microdvd,
    sub_read_line_subrip,
    sub_read_line_subviewer,
    sub_read_line_sami,
    sub_read_line_vplayer,
    sub_read_line_rt,
    sub_read_line_ssa,
    sub_read_line_pjs,
    sub_read_line_mpsub,
    sub_read_line_aqt,
    sub_read_line_jacobsub,
    sub_read_line_subviewer2,
    sub_read_line_subrip09,
    sub_read_line_mpl2,
  };

  /* Rewind (sub_autodetect() needs to read input from the beginning) */
  if(this->input->seek(this->input, 0, SEEK_SET) == -1) {
    xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "seek failed.\n");
    return NULL;
  }
  this->buflen = 0;

  this->format=sub_autodetect (this);
  if (this->format==FORMAT_UNKNOWN) {
    xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "Could not determine file format\n");
    return NULL;
  }

  xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "Detected subtitle file format: %d\n",this->format);

  /* Rewind */
  if(this->input->seek(this->input, 0, SEEK_SET) == -1) {
    xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "seek failed.\n");
    return NULL;
  }
  this->buflen = 0;

  this->num=0;n_max=32;
  first = calloc(n_max, sizeof(subtitle_t));
  if(!first) return NULL;
  timeout = ((demux_sputext_class_t *)
             (this->demux_plugin.demux_class))->max_timeout;
  if (this->uses_time) timeout *= 100;
  else timeout *= 10;

  while(1) {
    subtitle_t *sub;

    if(this->num>=n_max){
      n_max+=16;
      first=realloc(first,n_max*sizeof(subtitle_t));
    }

    sub = func[this->format] (this, &first[this->num]);

    if (!sub)
      break;   /* EOF */

    if (sub==ERR)
      ++this->errs;
    else {
      if (this->num > 0 && first[this->num-1].end == -1) {
	/* end time not defined in the subtitle */
	if (timeout > 0) {
	  /* timeout */
	  if (timeout > sub->start - first[this->num-1].start) {
	    first[this->num-1].end = sub->start;
	  } else
	    first[this->num-1].end = first[this->num-1].start + timeout;
	} else {
	  /* no timeout */
	  first[this->num-1].end = sub->start;
	}
      }
      ++this->num; /* Error vs. Valid */
    }
  }
  /* timeout of last subtitle */
  if (this->num > 0 && first[this->num-1].end == -1)
    if (timeout > 0) {
      first[this->num-1].end = first[this->num-1].start + timeout;
    }

  if (detect_utf8(first, this->num) > 0) {
    xprintf (this->stream->xine, XINE_VERBOSITY_LOG, "detected utf-8 subtitles\n");
    this->encoding = strdup("utf-8");
  }

  if(this->stream->xine->verbosity >= XINE_VERBOSITY_DEBUG) {
    char buffer[1024];

    sprintf(buffer, "Read %i subtitles", this->num);

    if(this->errs)
      sprintf(buffer + strlen(buffer), ", %i bad line(s).\n", this->errs);
    else
      strcat(buffer, "\n");

    xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "%s", buffer);
  }

  return first;
}

static int demux_sputext_next (demux_sputext_t *this_gen) {
  demux_sputext_t *this = (demux_sputext_t *) this_gen;
  buf_element_t *buf;
  uint32_t *val;
  char *str;
  subtitle_t *sub;
  int line;

  if (this->cur >= this->num)
    return 0;

  sub = &this->subtitles[this->cur];

  buf = this->stream->video_fifo->buffer_pool_alloc(this->stream->video_fifo);
  buf->type = BUF_SPU_TEXT;
  buf->pts = 0;

  val = (uint32_t * )buf->content;
  *val++ = sub->lines;
  *val++ = this->uses_time;
  *val++ = (this->uses_time) ? sub->start * 10 : sub->start;
  *val++ = (this->uses_time) ? sub->end * 10 : sub->end;
  str = (char *)val;
  for (line = 0; line < sub->lines; line++, str+=strlen(str)+1) {
    strncpy(str, sub->text[line], SUB_BUFSIZE-1);
    str[SUB_BUFSIZE-1] = '\0';
  }

  if (this->encoding) {
    buf->decoder_flags |= BUF_FLAG_SPECIAL;
    buf->decoder_info[1] = BUF_SPECIAL_CHARSET_ENCODING;
    buf->decoder_info_ptr[2] = this->encoding;
    buf->decoder_info[2] = strlen(buf->decoder_info_ptr[2]);
  }

  this->stream->video_fifo->put(this->stream->video_fifo, buf);
  this->cur++;

  return 1;
}

static void demux_sputext_dispose (demux_plugin_t *this_gen) {
  demux_sputext_t *this = (demux_sputext_t *) this_gen;
  int i, l;

  for (i = 0; i < this->num; i++) {
    for (l = 0; l < this->subtitles[i].lines; l++)
      _x_freep(&this->subtitles[i].text[l]);
  }
  _x_freep(&this->subtitles);
  _x_freep(&this->encoding);
  free(this);
}

static int demux_sputext_get_status (demux_plugin_t *this_gen) {
  demux_sputext_t *this = (demux_sputext_t *) this_gen;
  return this->status;
}

static int demux_sputext_get_stream_length (demux_plugin_t *this_gen) {
  demux_sputext_t   *this = (demux_sputext_t *) this_gen;

  if( this->uses_time && this->num ) {
    return this->subtitles[this->num-1].end * 10;
  } else {
    return 0;
  }
}

static int demux_sputext_send_chunk (demux_plugin_t *this_gen) {
  demux_sputext_t   *this = (demux_sputext_t *) this_gen;

  if (!demux_sputext_next (this)) {
    this->status = DEMUX_FINISHED;
  }

  return this->status;
}

static int demux_sputext_seek (demux_plugin_t *this_gen,
                            off_t start_pos, int start_time, int playing) {
  demux_sputext_t *this = (demux_sputext_t*)this_gen;

  lprintf("seek() called\n");

  /* simple seeking approach: just go back to start.
   * decoder will discard subtitles until the desired position.
   */
  this->cur = 0;
  this->status = DEMUX_OK;

  _x_demux_flush_engine (this->stream);
  _x_demux_control_newpts(this->stream, 0, 0);

  return this->status;
}

static void demux_sputext_send_headers(demux_plugin_t *this_gen) {
  demux_sputext_t *this = (demux_sputext_t*)this_gen;
  buf_element_t *buf;


  lprintf("send_headers() called\n");

  _x_demux_control_start(this->stream);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_HAS_VIDEO, 0);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_HAS_AUDIO, 0);

  /* enable the SPU channel */
  buf = this->stream->video_fifo->buffer_pool_alloc(this->stream->video_fifo);
  buf->type = BUF_CONTROL_SPU_CHANNEL;
  buf->decoder_info[0] = buf->decoder_info[1] = buf->decoder_info[2] = 0;
  this->stream->video_fifo->put(this->stream->video_fifo, buf);

  this->status = DEMUX_OK;
}

static uint32_t demux_sputext_get_capabilities(demux_plugin_t *this_gen) {
  return DEMUX_CAP_NOCAP;
}

static int demux_sputext_get_optional_data(demux_plugin_t *this_gen,
					   void *data, int data_type) {
  int channel = *((int *)data);

  switch (data_type) {
  case DEMUX_OPTIONAL_DATA_SPULANG:
    if (channel == -1 || channel == 0) {
      strcpy(data, "sub");
      return DEMUX_OPTIONAL_SUCCESS;
    }
  default:
    return DEMUX_OPTIONAL_UNSUPPORTED;
  }
}

static demux_plugin_t *open_demux_plugin (demux_class_t *class_gen, xine_stream_t *stream,
                                    input_plugin_t *input_gen) {

  input_plugin_t        *input = (input_plugin_t *) input_gen;
  demux_sputext_t       *this;

  lprintf("open_plugin() called\n");

  this = calloc(1, sizeof (demux_sputext_t));
  this->stream = stream;
  this->input  = input;

  this->demux_plugin.send_headers      = demux_sputext_send_headers;
  this->demux_plugin.send_chunk        = demux_sputext_send_chunk;
  this->demux_plugin.seek              = demux_sputext_seek;
  this->demux_plugin.dispose           = demux_sputext_dispose;
  this->demux_plugin.get_status        = demux_sputext_get_status;
  this->demux_plugin.get_stream_length = demux_sputext_get_stream_length;
  this->demux_plugin.get_capabilities  = demux_sputext_get_capabilities;
  this->demux_plugin.get_optional_data = demux_sputext_get_optional_data;
  this->demux_plugin.demux_class       = class_gen;

  this->buflen = 0;

  switch (stream->content_detection_method) {
  case METHOD_BY_MRL:
    {
      const char *const mrl = input->get_mrl(input);
      const char *const ending = strrchr(mrl, '.');

      if (!ending || (
	  (strncasecmp(ending, ".asc", 4) != 0) &&
	  (strncasecmp(ending, ".txt", 4) != 0) &&
	  (strncasecmp(ending, ".sub", 4) != 0) &&
	  (strncasecmp(ending, ".srt", 4) != 0) &&
	  (strncasecmp(ending, ".smi", 4) != 0) &&
	  (strncasecmp(ending, ".ssa", 4) != 0) &&
	  (strncasecmp(ending, ".ass", 4) != 0))) {
        free (this);
        return NULL;
      }
    }
    /* falling through is intended */

  case METHOD_EXPLICIT:
  /* case METHOD_BY_CONTENT: */

    /* FIXME: for now this demuxer only works when requested explicitly
     * to make sure it does not interfere with others;
     * If this is found too inconvenient, this may be changed after making
     * sure the content detection does not produce any false positives.
     */

    if ((input->get_capabilities(input) & INPUT_CAP_SEEKABLE) != 0) {

      this->subtitles = sub_read_file (this);

      this->cur = 0;

      if (this->subtitles) {
        xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG, "subtitle format %s time.\n",
		 this->uses_time ? "uses" : "doesn't use");
        xprintf (this->stream->xine, XINE_VERBOSITY_DEBUG,
		 "read %i subtitles, %i errors.\n", this->num, this->errs);
        return &this->demux_plugin;
      }
    }
    /* falling through is intended */
  }

  free (this);
  return NULL;
}

static void config_timeout_cb(void *this_gen, xine_cfg_entry_t *entry) {
  demux_sputext_class_t *this = (demux_sputext_class_t *)this_gen;

  this->max_timeout = entry->num_value;
}

void *init_sputext_demux_class (xine_t *xine, void *data) {

  demux_sputext_class_t *this ;

  lprintf("initializing\n");

  this = calloc(1, sizeof (demux_sputext_class_t));

  this->demux_class.open_plugin     = open_demux_plugin;
  this->demux_class.description     = N_("sputext demuxer plugin");
  this->demux_class.identifier      = "sputext";
  /* do not report this mimetype, it might confuse browsers. */
  /* "text/plain: asc txt sub srt: VIDEO subtitles;" */
  this->demux_class.mimetypes       = NULL;
  this->demux_class.extensions      = "asc txt sub srt smi ssa ass";
  this->demux_class.dispose         = default_demux_class_dispose;

  /*
   * Some subtitling formats, namely AQT and Subrip09, define the end of a
   * subtitle as the beginning of the following. From end-user view it's
   * better define timeout of hidding. Setting to zero means "no timeout".
   */
  this->max_timeout = xine->config->register_num(xine->config,
                         "subtitles.separate.timeout", 4,
			 _("default duration of subtitle display in seconds"),
			 _("Some subtitle formats do not explicitly give a duration for each subtitle. "
			   "For these, you can set a default duration here. Setting to zero will result "
			   "in the subtitle being shown until the next one takes over."),
			 20, config_timeout_cb, this);

  return this;
}
