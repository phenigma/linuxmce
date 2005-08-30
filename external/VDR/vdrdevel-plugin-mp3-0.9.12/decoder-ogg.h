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

#ifndef ___DECODER_OGG_H
#define ___DECODER_OGG_H

#define DEC_OGG     DEC_ID('O','G','G',' ')
#define DEC_OGG_STR "OGG"

#ifdef HAVE_VORBISFILE

#include <mad.h>
#include <vorbis/vorbisfile.h>

#include "decoder.h"
#include "decoder-core.h"

// ----------------------------------------------------------------

class cOggInfo;

class cOggFile : public cFileInfo {
friend class cOggInfo;
private:
  bool opened, canSeek;
  OggVorbis_File vf;
  //
  void Error(const char *action, const int err);
public:
  cOggFile(const char *Filename);
  ~cOggFile();
  bool Open(bool log=true);
  void Close(void);
  long long cOggFile::Seek(long long posMs=0, bool relativ=false);
  int Stream(short *buffer, int samples);
  bool CanSeek(void) { return canSeek; }
  long long IndexMs(void);
  };

// ----------------------------------------------------------------

class cOggInfo : public cSongInfo {
private:
  cOggFile *file;
  bool keepOpen;
  //
  bool Abort(bool result);
public:
  cOggInfo(cOggFile *File);
  bool DoScan(bool KeepOpen=false);
  };

// ----------------------------------------------------------------

class cOggDecoder : public cDecoder {
private:
  cOggFile file;
  cOggInfo info;
  struct Decode ds;
  struct mad_pcm *pcm;
  unsigned long long index;
  //
  void Init(void);
  bool Clean(void);
  bool GetInfo(bool keepOpen);
  struct Decode *Done(eDecodeStatus status);
public:
  cOggDecoder(const char *Filename);
  ~cOggDecoder();
  virtual bool Valid(void);
  virtual cFileInfo *FileInfo(void);
  virtual cSongInfo *SongInfo(bool get);
  virtual cPlayInfo *PlayInfo(void);
  virtual bool Start(void);
  virtual bool Stop(void);
  virtual bool Skip(int Seconds, float bsecs);
  virtual struct Decode *Decode(void);
  };

// ----------------------------------------------------------------

#endif //HAVE_VORBISFILE
#endif //___DECODER_OGG_H
