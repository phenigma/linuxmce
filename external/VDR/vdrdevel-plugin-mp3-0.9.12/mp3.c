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
#include <getopt.h>
#include <strings.h>
#include <typeinfo>

#include "common.h"

#include <vdr/menuitems.h>
#include <vdr/status.h>
#include <vdr/plugin.h>
#if VDRVERSNUM >= 10307
#include <vdr/interface.h>
#include <vdr/skins.h>
#endif

#include "setup.h"
#include "setup-mp3.h"
#include "data-mp3.h"
#include "data-src.h"
#include "player-mp3.h"
#include "menu.h"
#include "menu-async.h"
#include "decoder.h"
#include "i18n.h"
#include "version.h"

#ifdef DEBUG
#include <mad.h>
#endif

cFileSources MP3Sources;

// --- cMenuSetupMP3 --------------------------------------------------------

class cMenuSetupMP3 : public cMenuSetupPage {
private:
  cMP3Setup data;
  //
  const char *cddb[3], *disp[2], *scan[3], *bgr[3];
  const char *aout[AUDIOOUTMODES];
  int amode, amodes[AUDIOOUTMODES];
protected:
  virtual void Store(void);
public:
  cMenuSetupMP3(void);
  };

cMenuSetupMP3::cMenuSetupMP3(void)
{
  static const char allowed[] = { "abcdefghijklmnopqrstuvwxyz0123456789-_" };
  int numModes=0;
  aout[numModes]=tr("DVB"); amodes[numModes]=AUDIOOUTMODE_DVB; numModes++;
#ifdef WITH_OSS
  aout[numModes]=tr("OSS"); amodes[numModes]=AUDIOOUTMODE_OSS; numModes++;
#endif
  data=MP3Setup;
  amode=0;
  for(int i=0; i<numModes; i++)
    if(amodes[i]==data.AudioOutMode) { amode=i; break; }

  SetSection(tr("MP3"));
  Add(new cMenuEditStraItem(tr("Setup.MP3$Audio output mode"),     &amode,numModes,aout));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Audio mode"),            &data.AudioMode, tr("Round"), tr("Dither")));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Use 48kHz mode only"),   &data.Only48kHz));
#if VDRVERSNUM >= 10307
  disp[0]=tr("classic");
  disp[1]=tr("via skin");
  Add(new cMenuEditStraItem(tr("Setup.MP3$Replay display"),        &data.ReplayDisplay, 2, disp));
#endif
  Add(new cMenuEditIntItem( tr("Setup.MP3$Display mode"),          &data.DisplayMode, 1, 3));
  bgr[0]=tr("Black");
  bgr[1]=tr("Live");
  bgr[2]=tr("Images");
  Add(new cMenuEditStraItem(tr("Setup.MP3$Background mode"),       &data.BackgrMode, 3, bgr));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Initial loop mode"),     &data.InitLoopMode));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Initial shuffle mode"),  &data.InitShuffleMode));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Abort player at end of list"),&data.AbortAtEOL));
  scan[0]=tr("disabled");
  scan[1]=tr("ID3 only");
  scan[2]=tr("ID3 & Level");
  Add(new cMenuEditStraItem(tr("Setup.MP3$Background scan"),       &data.BgrScan, 3, scan));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Editor display mode"),   &data.EditorMode, tr("Filenames"), tr("ID3 names")));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Mainmenu mode"),         &data.MenuMode, tr("Playlists"), tr("Browser")));
  Add(new cMenuEditBoolItem(tr("Hide mainmenu entry"),             &data.HideMainMenu));
  Add(new cMenuEditIntItem( tr("Setup.MP3$Normalizer level"),      &data.TargetLevel, 0, MAX_TARGET_LEVEL));
  Add(new cMenuEditIntItem( tr("Setup.MP3$Limiter level"),         &data.LimiterLevel, MIN_LIMITER_LEVEL, 100));
  Add(new cMenuEditBoolItem(tr("Setup.MP3$Use HTTP proxy"),        &data.UseProxy));
  Add(new cMenuEditStrItem( tr("Setup.MP3$HTTP proxy host"),       data.ProxyHost,MAX_HOSTNAME,allowed));
  Add(new cMenuEditIntItem( tr("Setup.MP3$HTTP proxy port"),       &data.ProxyPort,1,65535));
  cddb[0]=tr("disabled");
  cddb[1]=tr("local only");
  cddb[2]=tr("local&remote");
  Add(new cMenuEditStraItem(tr("Setup.MP3$CDDB for CD-Audio"),     &data.UseCddb,3,cddb));
  Add(new cMenuEditStrItem( tr("Setup.MP3$CDDB server"),           data.CddbHost,MAX_HOSTNAME,allowed));
  Add(new cMenuEditIntItem( tr("Setup.MP3$CDDB port"),             &data.CddbPort,1,65535));
}

void cMenuSetupMP3::Store(void)
{
  data.AudioOutMode=amodes[amode];

  MP3Setup=data;
  SetupStore("InitLoopMode",     MP3Setup.InitLoopMode   );
  SetupStore("InitShuffleMode",  MP3Setup.InitShuffleMode);
  SetupStore("AudioMode",        MP3Setup.AudioMode      );
  SetupStore("AudioOutMode",     MP3Setup.AudioOutMode   );
  SetupStore("BgrScan",          MP3Setup.BgrScan        );
  SetupStore("EditorMode",       MP3Setup.EditorMode     );
  SetupStore("DisplayMode",      MP3Setup.DisplayMode    );
  SetupStore("BackgrMode",       MP3Setup.BackgrMode     );
  SetupStore("MenuMode",         MP3Setup.MenuMode       );
  SetupStore("TargetLevel",      MP3Setup.TargetLevel    );
  SetupStore("LimiterLevel",     MP3Setup.LimiterLevel   );
  SetupStore("Only48kHz",        MP3Setup.Only48kHz      );
  SetupStore("UseProxy",         MP3Setup.UseProxy       );
  SetupStore("ProxyHost",        MP3Setup.ProxyHost      );
  SetupStore("ProxyPort",        MP3Setup.ProxyPort      );
  SetupStore("UseCddb",          MP3Setup.UseCddb        );
  SetupStore("CddbHost",         MP3Setup.CddbHost       );
  SetupStore("CddbPort",         MP3Setup.CddbPort       );
  SetupStore("AbortAtEOL",       MP3Setup.AbortAtEOL     );
#if VDRVERSNUM >= 10307
  SetupStore("ReplayDisplay",    MP3Setup.ReplayDisplay  );
#endif
  SetupStore("HideMainMenu",     MP3Setup.HideMainMenu   );
}

// --- cAsyncStatus ------------------------------------------------------------

cAsyncStatus asyncStatus;

cAsyncStatus::cAsyncStatus(void)
{
  text=0;
  changed=false;
}

cAsyncStatus::~cAsyncStatus()
{
  free((void *)text);
}

void cAsyncStatus::Set(const char *Text)
{
  Lock();
  free((void *)text);
  text=Text ? strdup(Text) : 0;
  changed=true;
  Unlock();
}

const char *cAsyncStatus::Begin(void)
{
  Lock();
  return text;
}

void cAsyncStatus::Finish(void)
{
  changed=false;
  Unlock();
}

// --- cMP3Control --------------------------------------------------------

#if VDRVERSNUM >= 10307
#define clrBackground clrGray50
#define eDvbColor int
#define MAXROWS 120
#define INLINE
#else
#define MAXROWS MAXOSDHEIGHT
#define INLINE inline
#endif

class cMP3Control : public cControl {
private:
#if VDRVERSNUM >= 10307
  cOsd *osd;
  const cFont *font;
  cSkinDisplayReplay *disp;
#else
  bool statusInterfaceOpen;
#endif
  int bw, bh, bwc, fw, fh;
  //
  cMP3Player *player;
  bool visible, shown, bigwin, statusActive;
  time_t timeoutShow, greentime, oktime;
  int lastkeytime, number;
  bool selecting, selecthide;
  //
  cMP3PlayInfo *lastMode;
  time_t fliptime, listtime;
  int hashlist[MAXROWS];
  int flip, flipint, top, rows;
  int lastIndex, lastTotal, lastTop;
  int framesPerSecond;
  //
  bool jumpactive, jumphide, jumpsecs;
  int jumpmm;
  //
  void ShowTimed(int Seconds=0);
  void ShowProgress(bool open=false, bool bigWin=false);
  void ShowStatus(bool force);
  void HideStatus(void);
  void DisplayInfo(const char *s=0);
  void JumpDisplay(void);
  void JumpProcess(eKeys Key);
  void Jump(void);
  void Stop(void);
  INLINE void Write(int x, int y, int w, const char *text, eDvbColor fg=clrWhite, eDvbColor bg=clrBackground);
  INLINE void Fill(int x, int y, int w, int h, eDvbColor fg);
  inline void Flush(void);
public:
  cMP3Control(void);
  virtual ~cMP3Control();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Show(void) { ShowTimed(); }
  virtual void Hide(void);
  bool Visible(void) { return visible; }
  static bool SetPlayList(cPlayList *plist);
  };

cMP3Control::cMP3Control(void)
:cControl(player=new cMP3Player)
{
  visible=shown=bigwin=selecting=selecthide=jumpactive=jumphide=statusActive=false;
  timeoutShow=greentime=oktime=0;
  lastkeytime=number=0;
  lastMode=0;
  framesPerSecond=SecondsToFrames(1);
#if VDRVERSNUM >= 10307
  osd=0; disp=0;
  font=cFont::GetFont(fontOsd);
#else
  statusInterfaceOpen=false;
#endif
  cStatus::MsgReplaying(this,"MP3");
}

cMP3Control::~cMP3Control()
{
  delete lastMode;
  Hide();
  Stop();
}

void cMP3Control::Stop(void)
{
  cStatus::MsgReplaying(this,0);
  delete player; player=0;
  mgr->Halt();
  mgr->Flush(); //XXX remove later
}

bool cMP3Control::SetPlayList(cPlayList *plist)
{
  bool res;
  cControl *control=cControl::Control();
  // is there a running MP3 player?
  if(control && typeid(*control)==typeid(cMP3Control)) {
    // add songs to running playlist
    mgr->Add(plist);
    res=true;
    }
  else {
    mgr->Flush();
    mgr->Add(plist);
    cControl::Launch(new cMP3Control);
    res=false;
    }
  delete plist;
  return res;
}

void cMP3Control::ShowTimed(int Seconds)
{
  if(!visible) {
    ShowProgress(true);
    if(Seconds>0) timeoutShow=time(0)+Seconds;
    }
}

void cMP3Control::Hide(void)
{
  HideStatus();
  if(visible) {
#if VDRVERSNUM >= 10307
    delete osd; osd=0;
    delete disp; disp=0;
#else
    Interface->Close();
#endif
    needsFastResponse=visible=bigwin=false;
    }
}

void cMP3Control::ShowStatus(bool force)
{
  if((asyncStatus.Changed() || (force && !statusActive)) && !jumpactive) {
    const char *text=asyncStatus.Begin();
    if(text) {
#if VDRVERSNUM >= 10307
      if(MP3Setup.ReplayDisplay || !osd) {
        if(statusActive) Skins.Message(mtStatus,0);
        Skins.Message(mtStatus,text);
        }
      else {
        if(!statusActive) osd->SaveRegion(0,bh-2*fh,bw-1,bh-fh-1);
        osd->DrawText(0,bh-2*fh,text,clrBlack,clrCyan,font,bw,fh,taCenter);
        osd->Flush();
        }
#else
      if(!Interface->IsOpen()) {
        Interface->Open(0,-1);
        statusInterfaceOpen=true;
        }
      Interface->Status(text);
      Interface->Flush();
#endif
      statusActive=true;
      }
    else
      HideStatus();
    asyncStatus.Finish();
    }
}

void cMP3Control::HideStatus(void)
{
  if(statusActive) {
#if VDRVERSNUM >= 10307
    if(MP3Setup.ReplayDisplay || !osd)
      Skins.Message(mtStatus,0);
    else {
      osd->RestoreRegion();
      osd->Flush();
      }
#else
    if(statusInterfaceOpen) {
      Interface->Close();
      statusInterfaceOpen=false;
      }
    else {
      Interface->Status(0);
      Interface->Flush();
      }
#endif
    }
  statusActive=false;
}

#define CTAB    11 // some tabbing values for the progress display
#define CTAB2   5

void cMP3Control::Write(int x, int y, int w, const char *text, eDvbColor fg, eDvbColor bg)
{
#if VDRVERSNUM >= 10307
  if(osd) {
    //d(printf("write x=%d y=%d w=%d ->",x,y,w))
    x*=fw; if(x<0) x+=bw;
    y*=fh; if(y<0) y+=bh;
    osd->DrawText(x,y,text,fg,bg,font,w*fw);
    //d(printf(" x=%d y=%d w=%d\n",x,y,w*fw))
    }
#else
  if(w>0) Fill(x,y,w,1,bg);
  Interface->Write(x,y,text,fg,bg);
#endif
}

void cMP3Control::Fill(int x, int y, int w, int h, eDvbColor fg)
{
#if VDRVERSNUM >= 10307
  if(osd) {
    //d(printf("fill x=%d y=%d w=%d h=%d ->",x,y,w,h))
    x*=fw; if(x<0) x+=bw;
    y*=fh; if(y<0) y+=bh;
    osd->DrawRectangle(x,y,x+w*fw-1,y+h*fh-1,fg);
    //d(printf(" x=%d y=%d x2=%d y2=%d\n",x,y,x+h*fh-1,y+w*fw-1))
    }
#else
  Interface->Fill(x,y,w,h,fg);
#endif
}

void cMP3Control::Flush(void)
{
#if VDRVERSNUM >= 10307
  if(MP3Setup.ReplayDisplay) Skins.Flush();
  else if(osd) osd->Flush();
#else
  Interface->Flush();
#endif
}

void cMP3Control::ShowProgress(bool open, bool bigWin)
{
  int index, total;

  if(player->GetIndex(index,total) && total>=0) {
    if(!visible && open) {
      HideStatus();
#if VDRVERSNUM >= 10307
      if(MP3Setup.ReplayDisplay) {
        disp=Skins.Current()->DisplayReplay(false);
        if(!disp) return;
        bigWin=false;
        }
      else {
        int bt, bp;
        fw=font->Width(' ')*2;
        fh=font->Height();
        if(bigWin) {
          bw=Setup.OSDWidth;
          bh=Setup.OSDHeight;
          bt=0;
          bp=2*fh;
          rows=(bh-bp-fh/3)/fh;
          }
        else {
          bw=Setup.OSDWidth;
          bh=bp=2*fh;
          bt=Setup.OSDHeight-bh;
          rows=0;
          }
        bwc=bw/fw+1;
        //d(printf("mp3: bw=%d bh=%d bt=%d bp=%d bwc=%d rows=%d fw=%d fh=%d\n",
        //  bw,bh,bt,bp,bwc,rows,fw,fh))
        osd=cOsdProvider::NewOsd(Setup.OSDLeft,Setup.OSDTop+bt);
        if(!osd) return;
        if(bigWin) {
          tArea Areas[] = { { 0,0,bw-1,bh-bp-1,2 }, { 0,bh-bp,bw-1,bh-1,4 } };
          osd->SetAreas(Areas,sizeof(Areas)/sizeof(tArea));
          }
        else {
          tArea Areas[] = { { 0,0,bw-1,bh-1,4 } };
          osd->SetAreas(Areas,sizeof(Areas)/sizeof(tArea));
          }
        osd->DrawRectangle(0,0,bw-1,bh-1,clrGray50);
        osd->Flush();
        }
#else
      fw=cOsd::CellWidth();
      fh=cOsd::LineHeight();
      bh=bigWin ? Setup.OSDheight : -2;
      bw=bwc=Setup.OSDwidth;
      rows=Setup.OSDheight-3;
      Interface->Open(bw,bh);
      Interface->Clear();
#endif
      ShowStatus(true);
      bigwin=bigWin;
      needsFastResponse=visible=true;
      fliptime=listtime=0; flipint=0; flip=-1; top=lastTop=-1; lastIndex=lastTotal=-1;
      delete lastMode; lastMode=0;
      }

    cMP3PlayInfo *mode=new cMP3PlayInfo;
    bool valid=mgr->Info(-1,mode);
    bool changed=(!lastMode || mode->Hash!=lastMode->Hash);
    char buf[256];

    if(valid) { // send progress to status monitor
      if(changed || mode->Loop!=lastMode->Loop || mode->Shuffle!=lastMode->Shuffle) {
        snprintf(buf,sizeof(buf),mode->Artist[0]?"[%c%c] (%d/%d) %s - %s":"[%c%c] (%d/%d) %s",
                  mode->Loop?'L':'.',mode->Shuffle?'S':'.',mode->Num,mode->MaxNum,mode->Title,mode->Artist);
        cStatus::MsgReplaying(this,buf);
        }
      }

    if(visible) { // refresh the OSD progress display
      bool flush=false;

#if VDRVERSNUM >= 10307
      if(MP3Setup.ReplayDisplay) {
        if(!statusActive) {
          disp->SetProgress(index,total);
          disp->SetCurrent(IndexToHMSF(index));
          disp->SetTotal(IndexToHMSF(total));
          bool Play, Forward;
          int Speed;
          if(GetReplayMode(Play,Forward,Speed)) 
            disp->SetMode(Play, Forward, Speed);
          flush=true;
          }
        }
      else {
#endif
        if(!selecting && changed && !statusActive) {
          snprintf(buf,sizeof(buf),"(%d/%d)",mode->Num,mode->MaxNum);
          Write(0,-2,CTAB,buf);
          flush=true;
          }

        if(!lastMode || mode->Loop!=lastMode->Loop) {
          if(mode->Loop) Write(-4,-1,0,"L",clrBlack,clrYellow);
          else Fill(-4,-1,2,1,clrBackground);
          flush=true;
          }
        if(!lastMode || mode->Shuffle!=lastMode->Shuffle) {
          if(mode->Shuffle) Write(-2,-1,0,"S",clrWhite,clrRed);
          else Fill(-2,-1,2,1,clrBackground);
          flush=true;
          }

        index/=framesPerSecond; total/=framesPerSecond;
        if(index!=lastIndex || total!=lastTotal) {
          if(total>0) {
#if VDRVERSNUM >= 10307
            cProgressBar ProgressBar(bw-(CTAB+CTAB2)*fw,fh,index,total);
            osd->DrawBitmap(CTAB*fw,bh-fh,ProgressBar);
#else
            cProgressBar ProgressBar((bw-CTAB-CTAB2)*fw,fh,index,total);
            Interface->SetBitmap(CTAB*fw,(abs(bh)-1)*fh,ProgressBar);
#endif
            }
          snprintf(buf,sizeof(buf),total?"%02d:%02d/%02d:%02d":"%02d:%02d",index/60,index%60,total/60,total%60);
          Write(0,-1,11,buf);
          flush=true;
          }
#if VDRVERSNUM >= 10307
        }
#endif

      if(!jumpactive) {
        bool doflip=false;
        if(MP3Setup.ReplayDisplay && (!lastMode || mode->Loop!=lastMode->Loop || mode->Shuffle!=lastMode->Shuffle))
          doflip=true;
        if(!valid || changed) {
          fliptime=time(0); flip=0;
	  doflip=true;
	  }
        else if(time(0)>fliptime+flipint) {
	  fliptime=time(0);
	  flip++; if(flip>=MP3Setup.DisplayMode) flip=0;
          doflip=true;
	  }
        if(doflip) {
          buf[0]=0;
          switch(flip) {
	    default:
	      flip=0;
	      // fall through
	    case 0:
	      snprintf(buf,sizeof(buf),mode->Artist[0]?"%s - %s":"%s%s",mode->Title,mode->Artist);
	      flipint=6;
	      break;
	    case 1:
              if(mode->Album[0]) {
      	        snprintf(buf,sizeof(buf),mode->Year>0?"from: %s (%d)":"from: %s",mode->Album,mode->Year);
	        flipint=4;
	        }
              else fliptime=0;
              break;
	    case 2:
              if(mode->MaxBitrate>0)
                snprintf(buf,sizeof(buf),"%.1f kHz, %d-%d kbps, %s",mode->SampleFreq/1000.0,mode->Bitrate/1000,mode->MaxBitrate/1000,mode->SMode);
              else
                snprintf(buf,sizeof(buf),"%.1f kHz, %d kbps, %s",mode->SampleFreq/1000.0,mode->Bitrate/1000,mode->SMode);
	      flipint=3;
	      break;
	    }
          if(buf[0]) {
#if VDRVERSNUM >= 10307
            if(MP3Setup.ReplayDisplay) {
              char buf2[256];
              snprintf(buf2,sizeof(buf2),"[%c%c] (%d/%d) %s",
                       mode->Loop?'L':'.',mode->Shuffle?'S':'.',mode->Num,mode->MaxNum,buf);
              disp->SetTitle(buf2);
              flush=true;
              }
            else {
#endif
              if(!statusActive) {
                DisplayInfo(buf);
                flush=true;
                }
#if VDRVERSNUM >= 10307
              }
#endif
            }
          }
        }

      if(bigwin) {
        bool all=(top!=lastTop || changed);
        if(all || time(0)>listtime+2) {
          int num=(top>0 && mode->Num==lastMode->Num) ? top : mode->Num - rows/2;
          if(num+rows>mode->MaxNum) num=mode->MaxNum-rows+1;
          if(num<1) num=1;
          top=num;
          for(int i=0 ; i<rows && i<MAXROWS && num<=mode->MaxNum ; i++,num++) {
            cMP3PlayInfo pi;
            mgr->Info(num,&pi); if(!pi.Title[0]) break;
            snprintf(buf,sizeof(buf),pi.Artist[0]?"%d.\t%s - %s":"%d.\t%s",num,pi.Title,pi.Artist);
            eDvbColor fg=clrWhite, bg=clrBackground;
            int hash=MakeHash(buf);
            if(num==mode->Num) { fg=clrBlack; bg=clrCyan; hash=(hash^77) + 23; }
            if(all || hash!=hashlist[i]) {
              char *s=rindex(buf,'\t');
              if(s) {
                *s++=0;
                Write(0,i,5,buf,fg,bg);
                Write(5,i,bwc-5,s,fg,bg);
                }
              else
                Write(0,i,bwc,buf,fg,bg);
              flush=true;
              hashlist[i]=hash;
              }
            }
          listtime=time(0); lastTop=top;
          }
        }

      if(flush) Flush();
      }

    lastIndex=index; lastTotal=total;
    delete lastMode; lastMode=mode;
    }
}

void cMP3Control::DisplayInfo(const char *s)
{
  if(s) Write(CTAB,-2,bwc-CTAB,s);
  else Fill(CTAB,-2,bwc-CTAB,1,clrBackground);
}

void cMP3Control::JumpDisplay(void)
{
  char buf[64];
  const char *j=tr("Jump: "), u=jumpsecs?'s':'m';
  if(!jumpmm) sprintf(buf,"%s- %c",  j,u);
  else        sprintf(buf,"%s%d- %c",j,jumpmm,u);
#if VDRVERSNUM >= 10307
  if(MP3Setup.ReplayDisplay) {
    disp->SetJump(buf);
    }
  else {
#endif
    DisplayInfo(buf);
#if VDRVERSNUM >= 10307
    }
#endif
}

void cMP3Control::JumpProcess(eKeys Key)
{
 int n=Key-k0, d=jumpsecs?1:60;
  switch (Key) {
    case k0 ... k9:
      if(jumpmm*10+n <= lastTotal/d) jumpmm=jumpmm*10+n;
      JumpDisplay();
      break;
    case kBlue:
      jumpsecs=!jumpsecs;
      JumpDisplay();
      break;
    case kPlay:
    case kUp:
      jumpmm-=lastIndex/d;
      // fall through
    case kFastRew:
    case kFastFwd:
    case kLeft:
    case kRight:
      player->SkipSeconds(jumpmm*d * ((Key==kLeft || Key==kFastRew) ? -1:1));
      // fall through
    default:
      jumpactive=false;
      break;
    }

  if(!jumpactive) {
    if(jumphide) Hide();
#if VDRVERSNUM >= 10307
    else if(MP3Setup.ReplayDisplay) disp->SetJump(0);
#endif
    }
}

void cMP3Control::Jump(void)
{
  jumpmm=0; jumphide=jumpsecs=false;
  if(!visible) {
    ShowTimed(); if(!visible) return;
    jumphide=true;
    }
  JumpDisplay();
  jumpactive=true; fliptime=0; flip=-1;
}

eOSState cMP3Control::ProcessKey(eKeys Key)
{
  if(!player->Active()) return osEnd;

  if(visible && timeoutShow && time(0)>timeoutShow) { Hide(); timeoutShow=0; }
  ShowProgress();
#if VDRVERSNUM >= 10307
  ShowStatus(Key==kNone && !Skins.IsOpen());
#else
  ShowStatus(Key==kNone && !Interface->IsOpen());
#endif

  if(jumpactive && Key!=kNone) { JumpProcess(Key); return osContinue; }

  switch(Key) {
    case kUp:
      mgr->Next(); player->Play();
      break;
    case kDown:
      if(!player->PrevCheck()) mgr->Prev();
      player->Play();
      break;
    case kLeft:
      if(bigwin) {
        if(top>0) { top-=rows; if(top<1) top=1; }
        break;
        }
      // fall through
    case kLeft|k_Repeat:
      if(bigwin) break;
      // fall through
    case kFastRew:
    case kFastRew|k_Repeat:
      if(!player->IsStream()) player->SkipSeconds(-JUMPSIZE);
      break;
    case kRight:
      if(bigwin) {
        if(top>0) top+=rows;
        break;
        }
      // fall through
    case kRight|k_Repeat:
      if(bigwin) break;
      // fall through
    case kFastFwd:
    case kFastFwd|k_Repeat:
      if(!player->IsStream()) player->SkipSeconds(JUMPSIZE);
      break;
    case kRed:
      if(!player->IsStream()) Jump();
      break;
    case kGreen:
      if(lastMode) {
        if(time(0)>greentime) {
          if(lastMode->Loop || (!lastMode->Loop && !lastMode->Shuffle)) mgr->ToggleLoop();
          if(lastMode->Shuffle) mgr->ToggleShuffle();
          }
        else {
          if(!lastMode->Loop) mgr->ToggleLoop();
          else if(!lastMode->Shuffle) mgr->ToggleShuffle();
          else mgr->ToggleLoop();
          }
        greentime=time(0)+MULTI_TIMEOUT;
        }
      break;
    case kPause:
    case kYellow:
      if(!player->IsStream()) player->Pause();
      break;
    case kStop:
    case kBlue:
      Hide();
      Stop();
      return osEnd;

    case k0 ... k9:
      number=number*10+Key-k0;
      if(lastMode && number>0 && number<=lastMode->MaxNum) {
        if(!visible) { ShowTimed(); selecthide=true; }
        selecting=true; lastkeytime=time_ms();
        char buf[32];
#if VDRVERSNUM >= 10307
        if(MP3Setup.ReplayDisplay) {
          snprintf(buf,sizeof(buf),"%s%d-/%d",tr("Jump: "),number,lastMode->MaxNum);
          disp->SetJump(buf);
          }
        else {
#endif
          snprintf(buf,sizeof(buf),"(%d-/%d)",number,lastMode->MaxNum);
          Write(0,-2,CTAB,buf);
#if VDRVERSNUM >= 10307
          }
#endif
        Flush();
        break;
        }
      number=0; lastkeytime=0;
      // fall through
    case kNone:
      if(selecting && time_ms()-lastkeytime>SELECT_TIMEOUT) {
        if(number>0) { mgr->Goto(number); player->Play();  }
        if(selecthide) timeoutShow=time(0)+SELECTHIDE_TIMEOUT;
        if(lastMode) lastMode->Hash=-1;
        number=0; selecting=selecthide=false;
#if VDRVERSNUM >= 10307
        if(MP3Setup.ReplayDisplay) disp->SetJump(0);
#endif
        }
      break;
    case kOk:
      if(time(0)>oktime || MP3Setup.ReplayDisplay) {
        visible ? Hide() : ShowTimed();
        }
      else {
        if(visible && !bigwin) { Hide(); ShowProgress(true,true); }
        else { Hide(); ShowTimed(); }
        }
      oktime=time(0)+MULTI_TIMEOUT;
      ShowStatus(true);
      break;
    case kBack:
      Hide(); return osEnd; //XXX osMP3;
    default:
      return osUnknown;
    }
  return osContinue;
}

// --- cMenuID3Info ------------------------------------------------------------

class cMenuID3Info : public cOsdMenu {
private:
  cOsdItem *Item(const char *name, const char *text);
  cOsdItem *Item(const char *name, const char *format, const float num);
  void Build(cSongInfo *info, const char *name);
public:
  cMenuID3Info(cSong *song);
  cMenuID3Info(cSongInfo *si, const char *name);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuID3Info::cMenuID3Info(cSong *song)
:cOsdMenu(tr("ID3 information"),12)
{
  Build(song->Info(),song->Name());
}

cMenuID3Info::cMenuID3Info(cSongInfo *si, const char *name)
:cOsdMenu(tr("ID3 information"),12)
{
  Build(si,name);
}

void cMenuID3Info::Build(cSongInfo *si, const char *name)
{
  if(si) {
    Item(tr("Filename"),name);
    if(si->HasInfo() && si->Total>0) {
      char *buf=0;
      asprintf(&buf,"%02d:%02d",si->Total/60,si->Total%60);
      Item(tr("Length"),buf);
      free(buf);
      Item(tr("Title"),si->Title);
      Item(tr("Artist"),si->Artist);
      Item(tr("Album"),si->Album);
      Item(tr("Year"),0,(float)si->Year);
      Item(tr("Samplerate"),"%.1f kHz",si->SampleFreq/1000.0);
      Item(tr("Bitrate"),"%.f kbit/s",si->Bitrate/1000.0);
      Item(tr("Channels"),0,(float)si->Channels);
      }
    Display();
    }
}

cOsdItem *cMenuID3Info::Item(const char *name, const char *format, const float num)
{
  cOsdItem *item;
  if(num>=0.0) {
    char *buf=0;
    asprintf(&buf,format?format:"%.f",num);
    item=Item(name,buf);
    free(buf);
    }
  else item=Item(name,"");
  return item;
}

cOsdItem *cMenuID3Info::Item(const char *name, const char *text)
{
  char *buf=0;
  asprintf(&buf,"%s:\t%s",name,text?text:"");
  cOsdItem *item = new cOsdItem(buf,osBack);
#if VDRVERSNUM >= 10307
  item->SetSelectable(false);
#else
  item->SetColor(clrWhite, clrBackground);
#endif
  free(buf);
  Add(item); return item;
}

eOSState cMenuID3Info::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(state==osUnknown) {
     switch(Key) {
       case kRed:
       case kGreen:
       case kYellow:
       case kBlue:   return osContinue;
       case kMenu:   return osEnd;
       default: break;
       }
     }
  return state;
}

// --- cMenuInstantBrowse -------------------------------------------------------

class cMenuInstantBrowse : public cMenuBrowse {
private:
  const char *selecttext, *alltext;
  virtual void SetButtons(void);
  virtual eOSState ID3Info(void);
public:
  cMenuInstantBrowse(cFileSource *Source, const char *Selecttext, const char *Alltext);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuInstantBrowse::cMenuInstantBrowse(cFileSource *Source, const char *Selecttext, const char *Alltext)
:cMenuBrowse(Source,true,true,tr("Directory browser"))
{
  excl=excl_br; // defined in data-mp3.c
  selecttext=Selecttext; alltext=Alltext;
  SetButtons();
}

void cMenuInstantBrowse::SetButtons(void)
{
  SetHelp(selecttext, currentdir?tr("Parent"):0, currentdir?0:alltext, tr("ID3 info"));
  Display();
}

eOSState cMenuInstantBrowse::ID3Info(void)
{
  cFileObj *item=CurrentItem();
  if(item && item->Type()==otFile) {
    cSong *song=new cSong(item);
    cSongInfo *si;
    if(song && (si=song->Info())) {
      AddSubMenu(new cMenuID3Info(si,item->Path()));
      }
    delete song;
    }
  return osContinue;
}

eOSState cMenuInstantBrowse::ProcessKey(eKeys Key)
{
  eOSState state=cOsdMenu::ProcessKey(Key);
  if(state==osUnknown) {
     switch (Key) {
       case kYellow: lastselect=new cFileObj(source,0,0,otBase);
                     return osBack;
       default: break;
       }
     }
  if(state==osUnknown) state=cMenuBrowse::ProcessStdKey(Key,state);
  return state;
}

// --- cMenuPlayListItem -------------------------------------------------------

class cMenuPlayListItem : public cOsdItem {
  private:
  bool showID3;
  cSong *song;
public:
  cMenuPlayListItem(cSong *Song, bool showid3);
  cSong *Song(void) { return song; }
  virtual void Set(void);
  void Set(bool showid3);
  };

cMenuPlayListItem::cMenuPlayListItem(cSong *Song, bool showid3)
{
  song=Song;
  Set(showid3);
}

void cMenuPlayListItem::Set(bool showid3)
{
  showID3=showid3;
  Set();
}

void cMenuPlayListItem::Set(void)
{
  char *buffer=0;
  cSongInfo *si=song->Info(false);
  if(showID3 && !si) si=song->Info();
  if(showID3 && si && si->Title)
    asprintf(&buffer, "%d.\t%s%s%s",song->Index()+1,si->Title,si->Artist?" - ":"",si->Artist?si->Artist:"");
  else
    asprintf(&buffer, "%d.\t<%s>",song->Index()+1,song->Name());
  SetText(buffer,false);
}

// --- cMenuPlayList ------------------------------------------------------

class cMenuPlayList : public cOsdMenu {
private:
  cPlayList *playlist;
  bool browsing, showid3;
  void Buttons(void);
  void Refresh(bool all = false);
  void Add(void);
  virtual void Move(int From, int To);
  eOSState Remove(void);
  eOSState ShowID3(void);
  eOSState ID3Info(void);
public:
  cMenuPlayList(cPlayList *Playlist);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuPlayList::cMenuPlayList(cPlayList *Playlist)
:cOsdMenu(tr("Playlist editor"),4)
{
  browsing=showid3=false;
  playlist=Playlist;
  if(MP3Setup.EditorMode) showid3=true;

  cSong *mp3 = playlist->First();
  while(mp3) {
    cOsdMenu::Add(new cMenuPlayListItem(mp3,showid3));
    mp3 = playlist->cList<cSong>::Next(mp3);
    }
  Buttons(); Display();
}

void cMenuPlayList::Buttons(void)
{
  SetHelp(tr("Add"), showid3?tr("Filenames"):tr("ID3 names"), tr("Remove"), tr("Mark"));
}

void cMenuPlayList::Refresh(bool all)
{
  cMenuPlayListItem *cur=(cMenuPlayListItem *)((all || Count()<2) ? First() : Get(Current()));
  while(cur) {
    cur->Set(showid3);
    cur=(cMenuPlayListItem *)Next(cur);
    }
}

void cMenuPlayList::Add(void)
{
  cFileObj *item=cMenuInstantBrowse::GetSelected();
  if(item) {
    Status(tr("Scanning directory..."));
    cInstantPlayList *newpl=new cInstantPlayList(item);
    if(newpl->Load()) {
      if(newpl->Count()) {
        if(newpl->Count()==1 || Interface->Confirm(tr("Add recursivly?"))) {
          cSong *mp3=newpl->First();
          while(mp3) {
            cSong *n=new cSong(mp3);
            if(Count()>0) {
              cMenuPlayListItem *current=(cMenuPlayListItem *)Get(Current());
              playlist->Add(n,current->Song());
              cOsdMenu::Add(new cMenuPlayListItem(n,showid3),true,current);
              }
            else {
              playlist->Add(n);
              cOsdMenu::Add(new cMenuPlayListItem(n,showid3),true);
              }
            mp3=newpl->cList<cSong>::Next(mp3);
            }
          playlist->Save();
          Refresh(); Display();
          }
        }
      else Error(tr("Empty directory!"));
      }
    else Error(tr("Error scanning directory!"));
    delete newpl;
    Status(0);
    }
}

void cMenuPlayList::Move(int From, int To)
{
  playlist->Move(From,To); playlist->Save();
  cOsdMenu::Move(From,To);
  Refresh(true); Display();
}

eOSState cMenuPlayList::ShowID3(void)
{
  showid3=!showid3;
  Buttons(); Refresh(true); Display();
  return osContinue;
}

eOSState cMenuPlayList::ID3Info(void)
{
  if(Count()>0) {
    cMenuPlayListItem *current = (cMenuPlayListItem *)Get(Current());
    AddSubMenu(new cMenuID3Info(current->Song()));
    }
  return osContinue;
}

eOSState cMenuPlayList::Remove(void)
{
  if(Count()>0) {
    cMenuPlayListItem *current = (cMenuPlayListItem *)Get(Current());
    if(Interface->Confirm(tr("Remove entry?"))) {
      playlist->Del(current->Song()); playlist->Save();
      cOsdMenu::Del(Current());
      Refresh(); Display();
      }
    }
  return osContinue;
}

eOSState cMenuPlayList::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(browsing && !HasSubMenu() && state==osContinue) { Add(); browsing=false; }

  if(state==osUnknown) {
     switch(Key) {
       case kOk:     return ID3Info();
       case kRed:    browsing=true;
                     return AddSubMenu(new cMenuInstantBrowse(MP3Sources.GetSource(),tr("Add"),tr("Add all")));
       case kGreen:  return ShowID3();
       case kYellow: return Remove();
       case kBlue:   Mark(); return osContinue;
       case kMenu:   return osEnd;
       default: break;
       }
     }
  return state;
}

// --- cPlaylistRename --------------------------------------------------------

class cPlaylistRename : public cOsdMenu {
private:
  static char *newname;
  const char *oldname;
  char data[64];
public:
  cPlaylistRename(const char *Oldname);
  virtual eOSState ProcessKey(eKeys Key);
  static const char *GetNewname(void) { return newname; }
  };

char *cPlaylistRename::newname = NULL;

cPlaylistRename::cPlaylistRename(const char *Oldname)
:cOsdMenu(tr("Rename playlist"), 15)
{
  free(newname); newname=0;

  oldname=Oldname;
  char *buf=NULL;
  asprintf(&buf,"%s\t%s",tr("Old name:"),oldname);
  cOsdItem *old = new cOsdItem(buf,osContinue);
#if VDRVERSNUM >= 10307
  old->SetSelectable(false);
#else
  old->SetColor(clrWhite, clrBackground);
#endif
  Add(old);
  free(buf);

  data[0]=0;
  Add(new cMenuEditStrItem( tr("New name"), data, sizeof(data)-1, tr(FileNameChars)),true);
}

eOSState cPlaylistRename::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:     if(data[0] && strcmp(data,oldname)) newname=strdup(data);
                     return osBack;
       case kRed:
       case kGreen:
       case kYellow:
       case kBlue:   return osContinue;
       default: break;
       }
     }
  return state;
}

// --- cMenuMP3Item -----------------------------------------------------

class cMenuMP3Item : public cOsdItem {
  private:
  cPlayList *playlist;
  virtual void Set(void);
public:
  cMenuMP3Item(cPlayList *PlayList);
  cPlayList *List(void) { return playlist; }
  };

cMenuMP3Item::cMenuMP3Item(cPlayList *PlayList)
{
  playlist=PlayList;
  Set();
}

void cMenuMP3Item::Set(void)
{
  char *buffer=0;
  asprintf(&buffer," %s",playlist->BaseName());
  SetText(buffer,false);
}

// --- cMenuMP3 --------------------------------------------------------

class cMenuMP3 : public cOsdMenu {
private:
  cPlayLists *lists;
  bool renaming, sourcing, instanting;
  int buttonnum;
  eOSState Play(void);
  eOSState Edit(void);
  eOSState New(void);
  eOSState Delete(void);
  eOSState Rename(bool second);
  eOSState Source(bool second);
  eOSState Instant(bool second);
  void ScanLists(void);
  eOSState SetButtons(int num);
public:
  cMenuMP3(void);
  ~cMenuMP3(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuMP3::cMenuMP3(void)
:cOsdMenu(tr("MP3"))
{
  renaming=sourcing=instanting=false;
  lists=new cPlayLists;
  ScanLists(); SetButtons(1);
  if(MP3Setup.MenuMode) Instant(false);
}

cMenuMP3::~cMenuMP3(void)
{
  delete lists;
}

eOSState cMenuMP3::SetButtons(int num)
{
  switch(num) {
    case 1:
      SetHelp(tr("Edit"), tr("Source"), tr("Browse"), ">>");
      break;
    case 2:
      SetHelp("<<", tr("New"), tr("Delete"), tr("Rename"));
      break;
    }
  buttonnum=num; Display();
  return osContinue;
}

void cMenuMP3::ScanLists(void)
{
  Clear();
  Status(tr("Scanning playlists..."));
  bool res=lists->Load(MP3Sources.GetSource());
  Status(0);
  if(res) {
    cPlayList *plist=lists->First();
    while(plist) {
      Add(new cMenuMP3Item(plist));
      plist=lists->Next(plist);
      }
    }
  else Error(tr("Error scanning playlists!"));
}

eOSState cMenuMP3::Delete(void)
{
  if(Count()>0) {
    if(Interface->Confirm(tr("Delete playlist?")) &&
       Interface->Confirm(tr("Are you sure?")) ) {
      cPlayList *plist = ((cMenuMP3Item *)Get(Current()))->List();
      if(plist->Delete()) {
        lists->Del(plist);
        cOsdMenu::Del(Current());
        Display();
        }
      else Error(tr("Error deleting playlist!"));
      }
    }
  return osContinue;
}

eOSState cMenuMP3::New(void)
{
  cPlayList *plist=new cPlayList(MP3Sources.GetSource(),0,0);
  char name[32];
  int i=0;
  do {
    if(i) sprintf(name,"%s%d",tr("unnamed"),i++);
    else { strcpy(name,tr("unnamed")); i++; }
    } while(plist->TestName(name));

  if(plist->Create(name)) {
    lists->Add(plist);
    Add(new cMenuMP3Item(plist), true);

    isyslog("MP3: playlist %s added", plist->Name());
    return AddSubMenu(new cMenuPlayList(plist));
    }
  Error(tr("Error creating playlist!"));
  delete plist;
  return osContinue;
}

eOSState cMenuMP3::Rename(bool second)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  cPlayList *plist = ((cMenuMP3Item *)Get(Current()))->List();
  if(!second) {
    renaming=true;
    return AddSubMenu(new cPlaylistRename(plist->BaseName()));
    }
  renaming=false;
  const char *newname=cPlaylistRename::GetNewname();
  if(newname) {
    if(plist->Rename(newname)) {
      RefreshCurrent();
      DisplayCurrent(true);
      }
    else Error(tr("Error renaming playlist!"));
    }
  return osContinue;
}

eOSState cMenuMP3::Edit(void)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  cPlayList *plist = ((cMenuMP3Item *)Get(Current()))->List();
  if(!plist->Load()) Error(tr("Error loading playlist!"));
  else if(!plist->IsWinAmp()) {
    isyslog("MP3: editing playlist %s", plist->Name());
    return AddSubMenu(new cMenuPlayList(plist));
    }
  else Error(tr("Can't edit a WinAmp playlist!"));
  return osContinue;
}

eOSState cMenuMP3::Play(void)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  Status(tr("Loading playlist..."));
  cPlayList *newpl=new cPlayList(((cMenuMP3Item *)Get(Current()))->List());
  if(newpl->Load() && newpl->Count()) {
    isyslog("mp3: playback started with playlist %s", newpl->Name());
    cMP3Control::SetPlayList(newpl);
    return osEnd;
    }
  Status(0);
  delete newpl;
  Error(tr("Error loading playlist!"));
  return osContinue;
}

eOSState cMenuMP3::Source(bool second)
{
  if(HasSubMenu()) return osContinue;

  if(!second) {
    sourcing=true;
    return AddSubMenu(new cMenuSource(&MP3Sources,tr("MP3 source")));
    }
  sourcing=false;
  cFileSource *src=cMenuSource::GetSelected();
  if(src) {
    MP3Sources.SetSource(src);
    ScanLists();
    Display();
    }
  return osContinue;
}

eOSState cMenuMP3::Instant(bool second)
{
  if(HasSubMenu()) return osContinue;

  if(!second) {
    instanting=true;
    return AddSubMenu(new cMenuInstantBrowse(MP3Sources.GetSource(),tr("Play"),tr("Play all")));
    }
  instanting=false;
  cFileObj *item=cMenuInstantBrowse::GetSelected();
  if(item) {
    Status(tr("Building playlist..."));
    cInstantPlayList *newpl = new cInstantPlayList(item);
    if(newpl->Load() && newpl->Count()) {
      isyslog("mp3: playback started with instant playlist %s", newpl->Name());
      cMP3Control::SetPlayList(newpl);
      return osEnd;
      }
    Status(0);
    delete newpl;
    Error(tr("Error building playlist!"));
    }
  return osContinue;
}

eOSState cMenuMP3::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(!HasSubMenu() && state==osContinue) { // eval the return value from submenus
    if(renaming) return Rename(true);
    if(sourcing) return Source(true);
    if(instanting) return Instant(true);
    }

  if(state == osUnknown) {
    switch(Key) {
      case kOk:     return Play();
      case kRed:    return (buttonnum==1 ? Edit() : SetButtons(1)); 
      case kGreen:  return (buttonnum==1 ? Source(false) : New());
      case kYellow: return (buttonnum==1 ? Instant(false) : Delete());
      case kBlue:   return (buttonnum==1 ? SetButtons(2) : Rename(false));
      case kMenu:   return osEnd;
      default:      break;
      }
    }
  return state;
}

// --- PropagateImage ----------------------------------------------------------

void PropagateImage(const char *image)
{
  cPlugin *graphtft=cPluginManager::GetPlugin("graphtft");
  if(graphtft) graphtft->SetupParse("CoverImage",image ? image:"");
}

// --- cPluginMP3 --------------------------------------------------------------

static const char *VERSION        = PLUGIN_VERSION;
static const char *DESCRIPTION    = "A versatile audio player";
static const char *MAINMENUENTRY  = "MP3";

class cPluginMp3 : public cPlugin {
public:
  cPluginMp3(void);
  virtual ~cPluginMp3();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
#if VDRVERSNUM >= 10131
  virtual bool Initialize(void);
#else
  virtual bool Start(void);
#endif
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginMp3::cPluginMp3(void)
{
  // Initialize any member varaiables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginMp3::~cPluginMp3()
{
  InfoCache.Save();
  delete mgr;
}

const char *cPluginMp3::CommandLineHelp(void)
{
  static char *help_str=0;
  
  free(help_str);    //                                     for easier orientation, this is column 80|
  asprintf(&help_str,"  -m CMD,   --mount=CMD    use CMD to mount/unmount/eject mp3 sources\n"
                     "                           (default: %s)\n"
                     "  -n CMD,   --network=CMD  execute CMD before & after network access\n"
                     "                           (default: %s)\n"
                     "  -C DIR,   --cache=DIR    store ID3 cache file in DIR\n"
                     "                           (default: video dir)\n"
                     "  -B DIR,   --cddb=DIR     search CDDB files in DIR\n"
                     "                           (default: %s)\n"
                     "  -D DIR,   --dsp=DIR      device for OSS output\n"
                     "                           (default: %s)\n"
                     "  -i CMD,   --iconv=CMD    use CMD to convert background images\n"
                     "                           (default: %s)\n"
                     "  -c DIR,   --icache=DIR   cache converted images in DIR\n"
                     "                           (default: %s)\n",
                     
                     mountscript,netscript?netscript:"none",
#ifdef HAVE_SNDFILE
                     cddbpath,
#else
                     "none",
#endif
#ifdef WITH_OSS
                     dspdevice,
#else
                     "none",
#endif
                     imageconv,imagecache
                     );
  return help_str;
}

bool cPluginMp3::ProcessArgs(int argc, char *argv[])
{
  static struct option long_options[] = {
      { "mount",    required_argument, NULL, 'm' },
      { "network",  required_argument, NULL, 'n' },
      { "cddb",     required_argument, NULL, 'B' },
      { "dsp",      required_argument, NULL, 'D' },
      { "cache",    required_argument, NULL, 'C' },
      { "icache",   required_argument, NULL, 'c' },
      { "iconv",    required_argument, NULL, 'i' },
      { NULL }
    };

  int c, option_index = 0;
  while((c=getopt_long(argc,argv,"c:i:m:n:B:C:D:",long_options,&option_index))!=-1) {
    switch (c) {
      case 'i': imageconv=optarg; break;
      case 'c': imagecache=optarg; break;
      case 'm': mountscript=optarg; break;
      case 'n': netscript=optarg; break;
      case 'C': cachedir=optarg; break;
      case 'B':
#ifdef HAVE_SNDFILE
                cddbpath=optarg; break;
#else
                fprintf(stderr, "mp3: libsndfile support has not been compiled in!\n"); return false;
#endif
      case 'D':
#ifdef WITH_OSS
                dspdevice=optarg; break;
#else
                fprintf(stderr, "mp3: OSS output has not been compiled in!\n"); return false;
#endif
      default:  return false;
      }
    }
  return true;
}

#if VDRVERSNUM >= 10131
bool cPluginMp3::Initialize(void)
#else
bool cPluginMp3::Start(void)
#endif
{
  if(!CheckVDRVersion(1,1,29,"mp3")) return false;
  i18n_name=Name();
  MP3Sources.Load(AddDirectory(ConfigDirectory(),"mp3sources.conf"));
  if(MP3Sources.Count()<1) {
     esyslog("ERROR: you should have defined at least one source in mp3sources.conf");
     fprintf(stderr,"No source(s) defined in mp3sources.conf\n");
     return false;
     }
  InfoCache.Load();
  RegisterI18n(Phrases);
  mgr=new cPlayManager;
  if(!mgr) {
    esyslog("ERROR: creating playmanager failed");
    fprintf(stderr,"Creating playmanager failed\n");
    return false;
    }
  d(printf("mp3: using %s\n",mad_version))
  d(printf("mp3: compiled with %s\n",MAD_VERSION))
  return true;
}

void cPluginMp3::Housekeeping(void)
{
  InfoCache.Save();
}

const char *cPluginMp3::MainMenuEntry(void)
{
  return MP3Setup.HideMainMenu ? 0 : tr(MAINMENUENTRY);
}

cOsdObject *cPluginMp3::MainMenuAction(void)
{
  return new cMenuMP3;
}

cMenuSetupPage *cPluginMp3::SetupMenu(void)
{
  return new cMenuSetupMP3;
}

bool cPluginMp3::SetupParse(const char *Name, const char *Value)
{
  if      (!strcasecmp(Name, "InitLoopMode"))     MP3Setup.InitLoopMode    = atoi(Value);
  else if (!strcasecmp(Name, "InitShuffleMode"))  MP3Setup.InitShuffleMode = atoi(Value);
  else if (!strcasecmp(Name, "AudioMode"))        MP3Setup.AudioMode       = atoi(Value);
  else if (!strcasecmp(Name, "BgrScan"))          MP3Setup.BgrScan         = atoi(Value);
  else if (!strcasecmp(Name, "EditorMode"))       MP3Setup.EditorMode      = atoi(Value);
  else if (!strcasecmp(Name, "DisplayMode"))      MP3Setup.DisplayMode     = atoi(Value);
  else if (!strcasecmp(Name, "BackgrMode"))       MP3Setup.BackgrMode      = atoi(Value);
  else if (!strcasecmp(Name, "MenuMode"))         MP3Setup.MenuMode        = atoi(Value);
  else if (!strcasecmp(Name, "TargetLevel"))      MP3Setup.TargetLevel     = atoi(Value);
  else if (!strcasecmp(Name, "LimiterLevel"))     MP3Setup.LimiterLevel    = atoi(Value);
  else if (!strcasecmp(Name, "Only48kHz"))        MP3Setup.Only48kHz       = atoi(Value);
  else if (!strcasecmp(Name, "UseProxy"))         MP3Setup.UseProxy        = atoi(Value);
  else if (!strcasecmp(Name, "ProxyHost"))        strn0cpy(MP3Setup.ProxyHost,Value,MAX_HOSTNAME);
  else if (!strcasecmp(Name, "ProxyPort"))        MP3Setup.ProxyPort       = atoi(Value);
  else if (!strcasecmp(Name, "UseCddb"))          MP3Setup.UseCddb         = atoi(Value);
  else if (!strcasecmp(Name, "CddbHost"))         strn0cpy(MP3Setup.CddbHost,Value,MAX_HOSTNAME);
  else if (!strcasecmp(Name, "CddbPort"))         MP3Setup.CddbPort        = atoi(Value);
  else if (!strcasecmp(Name, "AbortAtEOL"))       MP3Setup.AbortAtEOL      = atoi(Value);
  else if (!strcasecmp(Name, "AudioOutMode")) {
    MP3Setup.AudioOutMode = atoi(Value);
#ifndef WITH_OSS
    if(MP3Setup.AudioOutMode==AUDIOOUTMODE_OSS) {
      esyslog("WARNING: AudioOutMode OSS not supported, falling back to DVB");
      MP3Setup.AudioOutMode=AUDIOOUTMODE_DVB;
      }
#endif
    }
#if VDRVERSNUM >= 10307
  else if (!strcasecmp(Name, "ReplayDisplay"))      MP3Setup.ReplayDisplay = atoi(Value);
#endif
  else if (!strcasecmp(Name, "HideMainMenu"))       MP3Setup.HideMainMenu  = atoi(Value);
  else return false;
  return true;
}

VDRPLUGINCREATOR(cPluginMp3); // Don't touch this!
