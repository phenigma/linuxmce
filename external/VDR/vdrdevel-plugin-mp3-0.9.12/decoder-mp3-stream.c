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
#include <errno.h>
#include <sys/types.h>

#include "common.h"
#include "decoder-mp3-stream.h"
#include "stream.h"

// --- cNetScanID3 -------------------------------------------------------------

class cNetScanID3 : public cScanID3 {
private:
  cNetStream *nstr;
  //
  void IcyInfo(void);
public:
  cNetScanID3(cNetStream *Str, bool *Urgent);
  virtual bool DoScan(bool KeepOpen=false);
  virtual void InfoHook(struct mad_header *header);
  };

cNetScanID3::cNetScanID3(cNetStream *Str, bool *Urgent)
:cScanID3(Str,Urgent)
{
  nstr=Str;
}

bool cNetScanID3::DoScan(bool KeepOpen)
{
  Clear();
  IcyInfo();
  if(!Title) FakeTitle(nstr->Filename);
  Total=0;
  ChMode=3;
  DecoderID=DEC_MP3S;
  InfoDone();
  return true;
}

void cNetScanID3::InfoHook(struct mad_header *header)
{
  if(nstr->IcyChanged()) IcyInfo();
  SampleFreq=header->samplerate;
  Channels=MAD_NCHANNELS(header);
  ChMode=header->mode;
 
  int br=header->bitrate;
  if(Bitrate<0) Bitrate=br;
  else if(Bitrate!=br) {
    if(MaxBitrate<0) {
      if(Bitrate<br) MaxBitrate=br;
      else { MaxBitrate=Bitrate; Bitrate=br; }
      }
    else {
      if(br>MaxBitrate) MaxBitrate=br;
      if(br<Bitrate)    Bitrate=br;
      }
    }
}

void cNetScanID3::IcyInfo(void)
{
  const char *t=nstr->IcyTitle();
  const char *a;
  if(t) {
    a=nstr->IcyName();
    if(!a) a=nstr->IcyUrl();
    }
  else {
    t=nstr->IcyName();
    a=nstr->IcyUrl();
    }
  if(t && (!Title || strcmp(t,Title))) {
    free(Title);
    Title=strdup(t);
    }
  if(a && (!Album || strcmp(a,Album))) {
    free(Album);
    Album=strdup(a);
    }
}

// --- cMP3StreamDecoder -------------------------------------------------------

cMP3StreamDecoder::cMP3StreamDecoder(const char *Filename)
:cMP3Decoder(Filename,false)
{
  nstr=new cNetStream(filename);
  str=nstr;
  nscan=new cNetScanID3(nstr,&urgentLock);
  scan=nscan;
  isStream=true;
}

bool cMP3StreamDecoder::Valid(void)
{
  bool res=false;
  if(TryLock()) {
    if(nstr->Valid()) res=true;
    Unlock();
    }
  return res;
}
