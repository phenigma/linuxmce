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

#ifndef ___DECODER_H
#define ___DECODER_H

#include <vdr/thread.h>
#include <vdr/tools.h>

// ----------------------------------------------------------------

class cCacheData;
class cFileObj;

extern int MakeHashBuff(const char *buff, int len);
#define MakeHash(str) MakeHashBuff((str),strlen(str))

// ----------------------------------------------------------------

class cSongInfo {
private:
  bool infoDone;
protected:
  void Clear(void);
  void Set(cSongInfo *si);
  void FakeTitle(const char *filename, const char *extention=0);
  void InfoDone(void) { infoDone=true; }
public:
  cSongInfo(void);
  ~cSongInfo();
  bool HasInfo(void) { return infoDone; }
  // Song
  char *Title, *Artist, *Album;
  int Year, Frames, Total;
  int SampleFreq, Channels, Bitrate, MaxBitrate, ChMode;
  // Normalize
  double Level, Peak;
  // Decoder
  int DecoderID;
  };

// ----------------------------------------------------------------

class cFileInfo {
private:
  bool infoDone;
  int removable;
protected:
  void Clear(void);
  void Set(cFileInfo *fi);
  void InfoDone(void) { infoDone=true; }
public:
  cFileInfo(void);
  cFileInfo(const char *Name);
  ~cFileInfo();
  bool FileInfo(bool log=true);
  bool HasInfo(void) { return infoDone; }
  bool Removable(void);
  //
  char *Filename, *FsID;
  unsigned long long Filesize;
  time_t CTime;
  long FsType;
  };

// ----------------------------------------------------------------

class cPlayInfo {
public:
  int Index, Total;
  };

// ----------------------------------------------------------------

class cDecoder {
protected:
  char *filename;
  cMutex lock, locklock;
  int locked;
  bool urgentLock;
  //
  cPlayInfo pi;
  bool playing;
  //
  void Lock(bool urgent=false);
  void Unlock(void);
  bool TryLock(void);
public:
  cDecoder(const char *Filename);
  virtual ~cDecoder();
  //
  virtual cFileInfo *FileInfo(void) { return 0; }
  virtual cSongInfo *SongInfo(bool get) { return 0; }
  virtual cPlayInfo *PlayInfo(void) { return 0; }
  //
  virtual bool Valid(void)=0;
  virtual bool IsStream(void) { return false; }
  virtual bool Start(void)=0;
  virtual bool Stop(void)=0;
  virtual bool Skip(int Seconds, float bsecs) { return false; }
  virtual struct Decode *Decode(void)=0;
  };
  
// ----------------------------------------------------------------

class cDecoders {
public:
  static cDecoder *FindDecoder(cFileObj *Obj);
  static const char *ID2Str(int id);
  static int Str2ID(const char *str);
  };

// ----------------------------------------------------------------

#define CACHELINES 32

class cInfoCache {
private:
  cMutex lock;
  cList<cCacheData> lists[CACHELINES];
  time_t lasttime, lastpurge;
  bool modified;
  //
  char *CacheFile(void);
  void AddEntry(cCacheData *dat);
  void DelEntry(cCacheData *dat);
  cCacheData *FirstEntry(int hash);
  void Modified(void) { modified=true; }
public:
  cInfoCache(void);
  void Save(bool force=false);
  void Load(void);
  void Purge(void);
  void Cache(cSongInfo *info, cFileInfo *file, bool update=false);
  cCacheData *Search(cFileInfo *file);
  };

extern cInfoCache InfoCache;
extern char *cachedir;

#endif //___DECODER_H
