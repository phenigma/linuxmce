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

#ifndef ___DECODER_SND_H
#define ___DECODER_SND_H

#define DEC_SND     DEC_ID('S','N','D',' ')
#define DEC_SND_STR "SND"

#ifdef HAVE_SNDFILE

#include <time.h>
#include <mad.h>
#include <sndfile.h>

#include <vdr/thread.h>

#include "decoder.h"
#include "decoder-core.h"

#define CDFS_MAGIC 0xCDDA // cdfs filesystem-ID

class cDiscID;

// ----------------------------------------------------------------

class cSndFile : public cFileInfo {
private:
  SNDFILE *sf;
  //
  void Error(const char *action);
public:
  SF_INFO sfi;
  //
  cSndFile(const char *Filename);
  ~cSndFile();
  bool Open(bool log=true);
  void Close(void);
  sf_count_t Seek(sf_count_t frames=0, bool relativ=false);
  sf_count_t Stream(int *buffer, sf_count_t frames);
  };

// ----------------------------------------------------------------

class cSndInfo : public cSongInfo {
private:
  cSndFile *file;
  cDiscID *id;
  bool keepOpen;
  //
  bool Abort(bool result);
  bool CDDBLookup(const char *filename);
public:
  cSndInfo(cSndFile *File);
  ~cSndInfo();
  bool DoScan(bool KeepOpen=false);
  };

// ----------------------------------------------------------------

class cSndDecoder : public cDecoder, public cThread {
private:
  cSndFile file;
  cSndInfo info;
  struct Decode ds;
  struct mad_pcm *pcm;
  unsigned long long index;
  //
  cMutex buffMutex;
  cCondVar fgCond, bgCond;
  bool run, ready;
  int *framebuff, deferedN, softCount;
  //
  void Init(void);
  bool Clean(void);
  bool GetInfo(bool keepOpen);
  struct Decode *Done(eDecodeStatus status);
protected:
  virtual void Action(void);
public:
  cSndDecoder(const char *Filename);
  ~cSndDecoder();
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

#endif //HAVE_SNDFILE
#endif //___DECODER_SND_H
