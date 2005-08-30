/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "data-mp3.h"
#include "decoder-mp3.h"
#include "stream.h"

#define MAX_FRAME_ERR 10

// ----------------------------------------------------------------

int MadStream(struct mad_stream *stream, cStream *str)
{
  unsigned char *data;
  unsigned long len;
  if(str->Stream(data,len,stream->next_frame)) {
    if(len>0) mad_stream_buffer(stream, data, len);
    return len;
    }
  return -1;
}

// --- cMP3Decoder -------------------------------------------------------------

cMP3Decoder::cMP3Decoder(const char *Filename, bool preinit)
:cDecoder(Filename)
{
  str=0; scan=0; isStream=false;
  if(preinit) {
    //d(printf("mp3: preinit\n"))
    str=new cStream(filename);
    scan=new cScanID3(str,&urgentLock);
    }
  fi=0; stream=0; frame=0; synth=0;
}

cMP3Decoder::~cMP3Decoder()
{
  Clean();
  delete scan;
  delete str;
}

bool cMP3Decoder::Valid(void)
{
  bool res=false;
  if(TryLock()) {
    struct mad_stream stream;
    struct mad_header header;
    mad_stream_init(&stream);
    mad_stream_options(&stream,MAD_OPTION_IGNORECRC);
    mad_header_init(&header);
    if(str->Open() && str->Seek()) {
      int count=10;
      do {
        if(mad_header_decode(&header,&stream)<0) {
          if(stream.error==MAD_ERROR_BUFLEN || stream.error==MAD_ERROR_BUFPTR) {
            if(MadStream(&stream,str)<=0) break;
            }
          else if(!MAD_RECOVERABLE(stream.error)) break;
          count++;
          }
        } while(--count);
      if(!count) res=true;
      }
    mad_header_finish(&header);
    mad_stream_finish(&stream);
    str->Close();
    Unlock();
    }
  return res;
}

cFileInfo *cMP3Decoder::FileInfo(void)
{
  cFileInfo *fi=0;
  if(str->HasInfo()) fi=str;
  else if(TryLock()){
    if(str->Open()) { fi=str; str->Close(); }
    Unlock();
    }
  return fi;
}

cSongInfo *cMP3Decoder::SongInfo(bool get)
{
  cSongInfo *si=0;
  if(scan->HasInfo()) si=scan;
  else if(get && TryLock()) {
    if(scan->DoScan()) si=scan;
    Unlock();
    }
  return si;
}

cPlayInfo *cMP3Decoder::PlayInfo(void)
{
  if(playing) {
    pi.Index=mad_timer_count(playtime,MAD_UNITS_SECONDS);
    pi.Total=scan->Total;
    return &pi;
    }
  return 0;
}

void cMP3Decoder::Init(void)
{
  Clean();
  stream=new struct mad_stream;
  mad_stream_init(stream);
  mad_stream_options(stream,MAD_OPTION_IGNORECRC);
  frame=new struct mad_frame;
  mad_frame_init(frame);
  synth=new struct mad_synth;
  mad_synth_init(synth);
  playtime=mad_timer_zero; skiptime=mad_timer_zero;
  framenum=framemax=0; mute=errcount=0;
}

void cMP3Decoder::Clean(void)
{
  playing=false;
  if(synth) { mad_synth_finish(synth); delete synth; synth=0; }
  if(frame) { mad_frame_finish(frame); delete frame; frame=0; }
  if(stream) { mad_stream_finish(stream); delete stream; stream=0; }
  delete[] fi; fi=0;
}

bool cMP3Decoder::Start(void)
{
  Lock(true);
  Init(); playing=true;
  if(str->Open() && scan->DoScan(true)) {
    if(!isStream) {
      str->Seek();
      framemax=scan->Frames+20;
      fi=new struct FrameInfo[framemax];
      if(!fi) esyslog("ERROR: no memory for frame index, rewinding disabled");
      }
    Unlock();
    return true;
    }
  str->Close();
  Clean();
  Unlock();
  return false;
}

bool cMP3Decoder::Stop(void)
{
  Lock();
  if(playing) {
    str->Close();
    Clean();
    }
  Unlock();
  return true;
}

struct Decode *cMP3Decoder::Done(eDecodeStatus status)
{
  ds.status=status;
  ds.index=mad_timer_count(playtime,MAD_UNITS_MILLISECONDS);
  ds.pcm=&synth->pcm;
  Unlock(); // release the lock from Decode()
  return &ds;
}

eDecodeStatus cMP3Decoder::DecodeError(bool hdr)
{
  if(stream->error==MAD_ERROR_BUFLEN || stream->error==MAD_ERROR_BUFPTR) {
    int s=MadStream(stream,str);
    if(s<0) return dsError;
    if(s==0) return dsEof;
    }
  else if(!MAD_RECOVERABLE(stream->error)) {
    d(printf("mad: decode %sfailed, frame=%d: %s\n",hdr?"hdr ":"",framenum,mad_stream_errorstr(stream)))
    return dsError;
    }
  else { 
    errcount+=hdr?1:100;
    d(printf("mad: decode %serror, frame=%d count=%d: %s\n",hdr?"hdr ":"",framenum,errcount,mad_stream_errorstr(stream)))
    }
  return dsOK;
}

struct Decode *cMP3Decoder::Decode(void)
{
  Lock(); // this is released in Done()
  eDecodeStatus r;
  while(playing) {
    if(errcount>=MAX_FRAME_ERR*100) {
      esyslog("ERROR: excessive decoding errors, aborting file %s",filename);
      return Done(dsError);
      }

    if(mad_header_decode(&frame->header,stream)<0) {
      if((r=DecodeError(true))) return Done(r);
      }
    else {
      if(!isStream) {
#ifdef DEBUG
        if(framenum>=framemax) printf("mp3: framenum >= framemax!!!!\n");
#endif
        if(fi && framenum<framemax) {
          fi[framenum].Pos=str->BufferPos() + (stream->this_frame-stream->buffer);
          fi[framenum].Time=playtime;
          }
        }

      mad_timer_add(&playtime,frame->header.duration); framenum++;

      if(mad_timer_compare(playtime,skiptime)>=0) skiptime=mad_timer_zero;
      else return Done(dsSkip);  // skipping, decode next header

      if(mad_frame_decode(frame,stream)<0) {
        if((r=DecodeError(false))) return Done(r);
        }
      else {
        errcount=0;
        scan->InfoHook(&frame->header);
        mad_synth_frame(synth,frame);
        if(mute) { mute--; return Done(dsSkip); }
        return Done(dsPlay);
        }
      }
    }
  return Done(dsError);
}

void cMP3Decoder::MakeSkipTime(mad_timer_t *skiptime, mad_timer_t playtime, int secs, float bsecs)
{
  mad_timer_t time;
  *skiptime=playtime;
  mad_timer_set(&time,abs(secs),0,0);
  if(secs<0) mad_timer_negate(&time);
  mad_timer_add(skiptime,time);
  int full=(int)bsecs; bsecs-=(float)full;
  mad_timer_set(&time,full,(int)(bsecs*1000.0),1000);
  mad_timer_negate(&time);
  mad_timer_add(skiptime,time);
  d(printf("mp3: skip: playtime=%ld secs=%d full=%d bsecs=%f skiptime=%ld\n",
           mad_timer_count(playtime,MAD_UNITS_MILLISECONDS),secs,full,bsecs,mad_timer_count(*skiptime,MAD_UNITS_MILLISECONDS)))
}

bool cMP3Decoder::Skip(int Seconds, float bsecs)
{
  Lock();
  bool res=false;
  if(playing && !isStream) {
    if(!mad_timer_compare(skiptime,mad_timer_zero)) { // allow only one skip at any time
      mad_timer_t time;
      MakeSkipTime(&time,playtime,Seconds,bsecs);

      if(mad_timer_compare(playtime,time)<=0) { // forward skip
#ifdef DEBUG
        int i=mad_timer_count(time,MAD_UNITS_SECONDS);
        printf("mp3: forward skipping to %02d:%02d\n",i/60,i%60);
#endif
        skiptime=time; mute=1;
        res=true;
        }
      else {                                    // backward skip
        if(fi) {
#ifdef DEBUG
          int i=mad_timer_count(time,MAD_UNITS_SECONDS);
          printf("mp3: rewinding to %02d:%02d\n",i/60,i%60);
#endif
          while(framenum && mad_timer_compare(time,fi[--framenum].Time)<0) ;
          mute=2; if(framenum>=2) framenum-=2;
          playtime=fi[framenum].Time;
          str->Seek(fi[framenum].Pos);
          mad_stream_finish(stream); // reset stream buffer
          mad_stream_init(stream);
#ifdef DEBUG
          i=mad_timer_count(playtime,MAD_UNITS_MILLISECONDS);
          printf("mp3: new playtime=%d framenum=%d filepos=%lld\n",i,framenum,fi[framenum].Pos);
#endif
          res=true;
          }
        }
      }
    }
  Unlock();
  return res;
}

// --- cScanID3 ----------------------------------------------------------------

// This function was adapted from mad_timer, from the 
// libmad distribution

#define MIN_SCAN_FRAMES 200 // min. number of frames to scan

cScanID3::cScanID3(cStream *Str, bool *Urgent)
{
  str=Str;
  urgent=Urgent;
}

bool cScanID3::Abort(bool result)
{
  if(!keepOpen) str->Close();
  return result;
}

bool cScanID3::DoScan(bool KeepOpen)
{
  mad_timer_t duration=mad_timer_zero;
  unsigned int bitrate=0, minrate=~0, maxrate=0;
  int xframes=0;
  bool is_vbr=false, has_id3=false;

  keepOpen=KeepOpen;
  if(!str->Open()) return Abort(false);
  if(HasInfo()) return Abort(true);

  // check the infocache
  cCacheData *dat=InfoCache.Search(str);
  if(dat) {
    Set(dat); dat->Unlock();
    if(!DecoderID) {
      DecoderID=DEC_MP3;
      InfoCache.Cache(this,str,true);
      }
    return Abort(true);
    }

  Clear();

  // do a initial check for a ID3v1 tag at the end of the file
  // to speed up the following scan
  if(str->Filesize>=128 && str->Seek(str->Filesize-128)) {
    unsigned char *data;
    unsigned long len;
    if(str->Stream(data,len)) {
      struct id3_tag *tag=id3_tag_parse(data,len);
      if(tag) {
        d(printf("mad: initialy found id3 tag at EOF\n"))
        has_id3=true;
        ParseID3(tag);
        id3_tag_delete(tag);
        }
      }
    }
  if(!str->Seek()) return Abort(false);

  // There are three ways of calculating the length of an mp3:
  // 1) Constant bitrate: One frame can provide the information
  //    needed: # of frames and duration. Just see how long it
  //    is and do the division.
  // 2) Variable bitrate: Xing tag. It provides the number of 
  //    frames. Each frame has the same number of samples, so
  //    just use that.
  // 3) All: Count up the frames and duration of each frames
  //    by decoding each one. We do this if we've no other
  //    choice, i.e. if it's a VBR file with no Xing tag.

  struct mad_stream stream;
  struct mad_header header;
  mad_stream_init(&stream);
  mad_stream_options(&stream,MAD_OPTION_IGNORECRC);
  mad_header_init(&header);
  bool res=true;
  int errcount=0;
  while(1) {
    if(*urgent) {
      d(printf("id3-scan: urgent request, aborting!\n"))
      res=false; break; // abort scan if there is an urgent request for the decoder lock
      }

    if(mad_header_decode(&header,&stream)<0) {
      if(stream.error==MAD_ERROR_BUFLEN || stream.error==MAD_ERROR_BUFPTR) {
        int s=MadStream(&stream,str);
        if(s>0) continue;
        if(s<0) res=false;
        break;
        }
      else if(stream.error==MAD_ERROR_LOSTSYNC && !has_id3) { // check for ID3 tags
#ifdef DEBUG
        char buf[10];
        int buf2[3];
        memcpy(buf,stream.this_frame,8); buf[8]=0;
        memcpy(buf2,stream.this_frame,8);
        printf("mad: id3-scan, lost sync %08x %08x %s\n",buf2[0],buf2[1],buf);
#endif
        id3_length_t tagsize=id3_tag_query(stream.this_frame,stream.bufend-stream.this_frame);
        if(tagsize>0) {
	  struct id3_tag *tag=GetID3(&stream,tagsize);
	  if(tag) {
            d(printf("mad: found id3 tag\n"))
            has_id3=true;
            ParseID3(tag);
	    id3_tag_delete(tag);
	    }
          }
        continue;
        }
      else {
        d(printf("mad: id3-scan, decode header error (frame %d): %s\n",Frames,mad_stream_errorstr(&stream)))
        errcount++;
        if(errcount<MAX_FRAME_ERR*100 && MAD_RECOVERABLE(stream.error)) continue;
        res=false; break;
        }
      }
    errcount=0;
    if(header.bitrate>maxrate) maxrate=header.bitrate;
    if(header.bitrate<minrate) minrate=header.bitrate;

    // Limit xing testing to the first frame header
    if(!Frames) {
      if((xframes=ParseXing(&stream.anc_ptr, stream.anc_bitlen))>=0) {
        is_vbr=true;
        }
      }                
    // Test the first n frames to see if this is a VBR file
    if(!is_vbr && Frames<MIN_SCAN_FRAMES) {
      if(bitrate && header.bitrate!=bitrate) is_vbr=true;
      else bitrate=header.bitrate;
      }
    // We have to assume it's not a VBR file if it hasn't already been
    // marked as one and we've checked n frames for different bitrates
    else if(!is_vbr && has_id3)
      {
      break;
      }

    Frames++;
    mad_timer_add(&duration,header.duration);
    }
  mad_header_finish(&header);
  mad_stream_finish(&stream);

  if(res) {
    d(printf("mad: scanned %d frames%s\n",Frames,Frames?"":"(is this really a mp3?)"))
    if(Frames) {
      SampleFreq=header.samplerate;
      Channels=MAD_NCHANNELS(&header);
      ChMode=header.mode;
      DecoderID=DEC_MP3;
      InfoDone();

      if(!is_vbr) {
        d(printf("mad: constant birate\n"))
        double time = (str->Filesize * 8.0) / (header.bitrate);  // time in seconds
        long nsamples = 32 * MAD_NSBSAMPLES(&header);   // samples per frame

        Frames = (long)(time * header.samplerate / nsamples);
        Total  = (long)time;
        Bitrate= (int)bitrate;
        }
      else if(xframes>0) {
        d(printf("mad: vbr, but has Xing frame\n"))
        mad_timer_multiply(&header.duration, xframes);
        Frames = xframes;
        Total  = mad_timer_count(header.duration,MAD_UNITS_SECONDS);
        }
      else {
        // the durations have been added up, and the number of frames counted. We do nothing here.
        d(printf("mad: vbr detected\n"))
        Total      = mad_timer_count(duration,MAD_UNITS_SECONDS);
        Bitrate    = (int)minrate;
        MaxBitrate = (int)maxrate;
        }

      if(!has_id3 || !Title) FakeTitle(str->Filename,".mp3");
      InfoCache.Cache(this,str);
      }
    }
  return Abort(res);
}

// This function was adapted from player.c, from the 
// libmad distribution

struct id3_tag *cScanID3::GetID3(struct mad_stream *stream, id3_length_t tagsize)
{
  struct id3_tag *tag=0;
  const id3_byte_t *data;
  id3_byte_t *allocated=0;

  id3_length_t count=stream->bufend-stream->this_frame;

  if(count>=tagsize) {
    data=stream->this_frame;
    mad_stream_skip(stream,tagsize);
    }
  else {
    if(!(allocated=(id3_byte_t *)malloc(tagsize))) {
      esyslog("ERROR: not enough memory for id3 tag buffer");
      return 0;
      }
    memcpy(allocated,stream->this_frame,count);
    mad_stream_skip(stream,count);

    while(count<tagsize) {
      unsigned char *sdata;
      unsigned long len, slen;

      if(!str->Stream(sdata,slen) || !slen) {
         d(printf("mad: error or eof on ID3 tag parse\n"))
         free(allocated);
         return 0;
         }
      len=tagsize-count; if(len>slen) len=slen;
      memcpy(allocated+count,sdata,len);
      count+=len;
      sdata+=len; slen-=len;
      if(slen) mad_stream_buffer(stream,sdata,slen);
      }
    data=allocated;
    }

  tag=id3_tag_parse(data,tagsize);
  if(allocated) free(allocated);
  return tag;
}

void cScanID3::ParseID3(const struct id3_tag *tag)
{
  ParseStr(tag,ID3_FRAME_TITLE,Title);
  ParseStr(tag,ID3_FRAME_ARTIST,Artist);
  ParseStr(tag,ID3_FRAME_ALBUM,Album);
  char *data=0;
  ParseStr(tag,ID3_FRAME_YEAR,data);
  if(data) Year=atol(data);
  free(data);
  //ParseStr(tag,ID3_FRAME_TRACK,Track);
  //ParseStr(tag,ID3_FRAME_GENRE,Genre);
}

// This function was adapted from player.c, from the 
// libmad distribution 

void cScanID3::ParseStr(const struct id3_tag *tag, const char *id, char * &data)
{
  const struct id3_frame *frame=id3_tag_findframe(tag,id,0);
  if(!frame) return;

  free(data); data=0;
  const union id3_field *field=&frame->fields[1];
  if(id3_field_getnstrings(field)>0) {
    const id3_ucs4_t *ucs4=id3_field_getstrings(field,0);
    if(!ucs4) return;
    if(!strcmp(id,ID3_FRAME_GENRE)) ucs4=id3_genre_name(ucs4);

    id3_latin1_t *latin1=id3_ucs4_latin1duplicate(ucs4);
    if(!latin1) return;

    data=strdup((char *)latin1);
    free(latin1);
    }
}

// XING parsing was adapted from the MAD winamp input plugin,
// from the libmad distribution

#define XING_MAGIC (('X'<<24) | ('i'<<16) | ('n'<<8) | 'g')
#define XING_FRAMES 0x0001
// #define XING_BYTES  0x0002
// #define XING_TOC    0x0004
// #define XING_SCALE  0x0008

int cScanID3::ParseXing(struct mad_bitptr *ptr, unsigned int bitlen)
{
  if(bitlen>=64 && mad_bit_read(ptr,32)==XING_MAGIC) {
    int flags=mad_bit_read(ptr, 32);
    bitlen-=64;
    return (bitlen>=32 && (flags & XING_FRAMES)) ? mad_bit_read(ptr,32) : 0;
    }
  return -1;
}
