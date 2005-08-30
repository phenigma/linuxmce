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

#ifndef ___DVB_MP3_H
#define ___DVB_MP3_H

#include <vdr/thread.h>
#include <vdr/player.h>

// -------------------------------------------------------------------

class cRingBufferFrame;
class cFrame;

class cPlayList;
class cSong;
class cSongInfo;
class cBackgroundScan;
class cDecoder;
class cOutputDvb;
class cShuffle;

// -------------------------------------------------------------------

class cMP3PlayInfo {
public:
  char Title[64], Artist[64], Album[64], SMode[32];
  int Year, SampleFreq, Bitrate, MaxBitrate;
  int Num, MaxNum;
  // not in hash
  bool Loop, Shuffle;
  int Hash;
  };

// -------------------------------------------------------------------

class cPlayManager : public cThread {
private:
  cMutex listMutex;
  cCondVar fgCond, bgCond;
  cList<cSong> list;
  cSong *curr;
  int currIndex, maxIndex;
  //
  cSong *play;
  bool playNew, eol;
  //
  cShuffle *shuffle;
  bool shuffleMode, loopMode;
  //
  cSong *scan;
  bool stopscan, throttle, pass2, release;
  //
  virtual void Action(void);
  void NoScan(cSong *nono);
  void NoPlay(cSong *nono);
  void ThrottleWait(void);
public:
  cPlayManager(void);
  ~cPlayManager();
  // Control interface (to be called from frontend thread only!)
  void Flush(void);
  void Add(cPlayList *pl);
  bool Next(void);
  bool Prev(void);
  void Goto(int num);
  void ToggleShuffle(void);
  void ToggleLoop(void);
  bool Info(int num, cMP3PlayInfo *info);
  void Halt(void);
  // Player interface (to be called from player thread only!)
  cSong *Current(void);
  bool NewCurrent(void);
  bool NextCurrent(void);
  void Release(void);
  void Throttle(bool thr);
  };

extern cPlayManager *mgr;

// -------------------------------------------------------------------

class cMP3Player : public cPlayer, cThread {
friend class cOutputDvb;
private:
  bool active, started;
  cRingBufferFrame *ringBuffer;
  cMutex playModeMutex;
  cCondVar playModeCond;
//
  int playindex, total;
  cDecoder *decoder;
  cFrame *rframe, *pframe;
  enum ePlayMode { pmPlay, pmStopped, pmPaused, pmStartup };
  ePlayMode playMode;
  enum eState { msStart, msStop, msDecode, msNormalize, msResample, msOutput, msError, msEof, msWait, msRestart };
  eState state;
  bool levelgood, isStream;
  unsigned int dvbSampleRate;
//
  void Empty(void);
  void StopPlay(void);
  void SetPlayMode(ePlayMode mode);
  void WaitPlayMode(ePlayMode mode, bool inv);
protected:
  virtual void Activate(bool On);
  virtual void Action(void);
public:
  cMP3Player(void);
  virtual ~cMP3Player();
  void Pause(void);
  void Play(void);
  bool PrevCheck(void);
  void SkipSeconds(int secs);
  virtual bool GetIndex(int &Current, int &Total, bool SnapToIFrame=false);
  virtual bool GetReplayMode(bool &Play, bool &Forward, int &Speed);
  bool Active(void) { return active; }
  bool IsStream(void) { return isStream; }
  };

#endif //___DVB_MP3_H
