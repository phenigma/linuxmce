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

#ifndef ___DECODER_MP3_H
#define ___DECODER_MP3_H

#define DEC_MP3     DEC_ID('M','P','3',' ')
#define DEC_MP3_STR "MP3"

#include <mad.h>
#include <id3tag.h>

#include "decoder.h"
#include "decoder-core.h"

#if MAD_F_FRACBITS != 28
#warning libmad with MAD_F_FRACBITS != 28 not tested
#endif

class cStream;

// ----------------------------------------------------------------

class cScanID3 : public cSongInfo {
private:
  bool keepOpen, *urgent;
  //
  bool Abort(bool result);
  struct id3_tag *GetID3(struct mad_stream *stream, id3_length_t tagsize);
  void ParseID3(const struct id3_tag *tag);
  void ParseStr(const struct id3_tag *tag, const char *id, char * &data);
  int ParseXing(struct mad_bitptr *ptr, unsigned int bitlen);
protected:
  cStream *str;
public:
  cScanID3(cStream *Str, bool *Urgent);
  virtual ~cScanID3() {}
  virtual bool DoScan(bool KeepOpen=false);
  virtual void InfoHook(struct mad_header *header) {}
  };

// ----------------------------------------------------------------

class cMP3Decoder : public cDecoder {
private:
  struct Decode ds;
  //
  struct mad_stream *stream;
  struct mad_frame *frame;
  struct mad_synth *synth;
  mad_timer_t playtime, skiptime;
  //
  struct FrameInfo {
    unsigned long long Pos;
    mad_timer_t Time;
    } *fi;
  int framenum, framemax, errcount, mute;
  //
  void Init(void);
  void Clean(void);
  struct Decode *Done(eDecodeStatus status);
  eDecodeStatus DecodeError(bool hdr);
  void MakeSkipTime(mad_timer_t *skiptime, mad_timer_t playtime, int secs, float bsecs);
protected:
  cStream *str;
  cScanID3 *scan;
  bool isStream;
public:
  cMP3Decoder(const char *Filename, bool preinit=true);
  virtual ~cMP3Decoder();
  virtual bool Valid(void);
  virtual cFileInfo *FileInfo(void);
  virtual cSongInfo *SongInfo(bool get);
  virtual cPlayInfo *PlayInfo(void);
  virtual bool Start(void);
  virtual bool Stop(void);
  virtual bool Skip(int Seconds, float bsecs);
  virtual struct Decode *Decode(void);
  };

#endif //___DECODER_MP3_H
