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

#include <getopt.h>
#include <malloc.h>
#include <stdlib.h>

#include "common.h"

#include <vdr/plugin.h>
#include <vdr/player.h>
#include <vdr/status.h>
#include <vdr/font.h>
#include <vdr/osdbase.h>
#include <vdr/menuitems.h>
#ifdef HAVE_BEAUTYPATCH
#include <vdr/fontsym.h>
#endif
#if VDRVERSNUM >= 10307
#include <vdr/skins.h>
#endif

#include "setup.h"
#include "setup-mplayer.h"
#include "menu.h"
#include "player-mplayer.h"
#include "data.h"
#include "data-src.h"
#include "i18n.h"
#include "version.h"

cFileSources MPlaySources;

// --- cMenuSetupMPlayer --------------------------------------------------------

class cMenuSetupMPlayer : public cMenuSetupPage {
private:
  cMPlayerSetup data;
  const char *res[3];
protected:
  virtual void Store(void);
public:
  cMenuSetupMPlayer(void);
  };

cMenuSetupMPlayer::cMenuSetupMPlayer(void)
{
  data=MPlayerSetup;
  SetSection(tr("MPlayer"));
  Add(new cMenuEditBoolItem(tr("Setup.MPlayer$Control mode"),  &data.SlaveMode, tr("Traditional"), tr("Slave")));
  res[0]=tr("disabled");
  res[1]=tr("global only");
  res[2]=tr("local first");
  Add(new cMenuEditStraItem(tr("Setup.MPlayer$Resume mode"),   &data.ResumeMode, 3, res));
  Add(new cMenuEditBoolItem(tr("Hide mainmenu entry"),         &data.HideMainMenu));
}

void cMenuSetupMPlayer::Store(void)
{
  MPlayerSetup=data;
  SetupStore("ControlMode", MPlayerSetup.SlaveMode);
  SetupStore("HideMainMenu",MPlayerSetup.HideMainMenu);
  SetupStore("ResumeMode",  MPlayerSetup.ResumeMode);
}

// --- cMPlayerControl ---------------------------------------------------------

class cMPlayerControl : public cControl {
private:
  static cFileObj *file;
  static bool rewind;
  cMPlayerPlayer *player;
#if VDRVERSNUM >= 10307
  cSkinDisplayReplay *display;
#endif
  bool visible, modeOnly, haveBeauty;
  time_t timeoutShow;
  int lastCurrent, lastTotal, osdPos;
  //
  bool jumpactive, jumphide, jumpmode;
  int jumpval;
  //
  void Stop(void);
  void ShowTimed(int Seconds=0);
  void DisplayAtBottom(const char *s);
  void ShowProgress(void);
  void ShowMode(void);
  void Jump(void);
  void JumpProcess(eKeys Key);
  void JumpDisplay(void);
public:
  cMPlayerControl(void);
  virtual ~cMPlayerControl();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Show(void) { ShowTimed(); }
  virtual void Hide(void);
  static void SetFile(const cFileObj *File, bool Rewind);
  };

cFileObj *cMPlayerControl::file=0;
bool cMPlayerControl::rewind=false;

cMPlayerControl::cMPlayerControl(void)
:cControl(player=new cMPlayerPlayer(file,rewind))
{
  visible=modeOnly=jumpactive=haveBeauty=false; osdPos=0;
  cStatus::MsgReplaying(this,file->Path());
#if VDRVERSNUM >= 10307
  display=0;
#else
#ifdef HAVE_BEAUTYPATCH
#if VDRVERSNUM >= 10300
  const cFont *sym=cFont::GetFont(fontSym);
  const cFont *osd=cFont::GetFont(fontOsd);
  const cFont::tCharData *symD=sym->CharData(32);
  const cFont::tCharData *osdD=osd->CharData(32);
#else //VDRVERSNUM >= 10300
  cFont *sym=new cFont(fontSym);
  cFont *osd=new cFont(fontOsd);
  const cFont::tCharData *symD=sym->CharData(32);
  const cFont::tCharData *osdD=osd->CharData(32);
  delete sym;
  delete osd;
#endif //VDRVERSNUM >= 10300
  if(symD != osdD) haveBeauty=true;
  d(printf("mplayer: beauty patch %sdetected\n",haveBeauty?"":"NOT "))
#endif //HAVE_BEAUTYPATCH
#endif //VDRVERSNUM >= 10307
}

cMPlayerControl::~cMPlayerControl()
{
  Stop();
  cStatus::MsgReplaying(this, NULL);
}

void cMPlayerControl::SetFile(const cFileObj *File, bool Rewind)
{
  delete file;
  file=File ? new cFileObj(File) : 0;
  rewind=Rewind;
}

void cMPlayerControl::Stop(void)
{
  delete player; player=0;
}

void cMPlayerControl::ShowTimed(int Seconds)
{
  if(modeOnly) Hide();
  if(!visible) {
    ShowProgress();
    timeoutShow = Seconds>0 ? time(0)+Seconds : 0;
    }
}

void cMPlayerControl::Hide(void)
{
  if(visible) {
#if VDRVERSNUM >= 10307
    delete display; display=0;
#else
    Interface->Close();
#endif
    needsFastResponse=visible=modeOnly=false;
    }
}

void cMPlayerControl::DisplayAtBottom(const char *s)
{
#if VDRVERSNUM < 10307
  const int p=modeOnly ? 0 : 2;
  if(s) {
    const int d=max(Width()-cOsd::WidthInCells(s),0) / 2;
    if(modeOnly) Interface->Fill(0, p, Interface->Width(), 1, clrTransparent);
    Interface->Write(d, p, s);
    }
  else
    Interface->Fill(12, p, Width() - 22, 1, clrBackground);
#endif
}

void cMPlayerControl::ShowProgress(void)
{
  int Current, Total;

  if(GetIndex(Current,Total) && Total>0) {
    bool flush=false;
    if(!visible) {
#if VDRVERSNUM >= 10307
      display=Skins.Current()->DisplayReplay(false);
#else
      Interface->Open(Setup.OSDwidth,osdPos-3);
      Interface->Clear();
      if(osdPos<0) Interface->Fill(0,3,Interface->Width(),-osdPos,clrTransparent);
#endif
      needsFastResponse=visible=true; modeOnly=false;
      if(file) {
#if VDRVERSNUM >= 10307
        display->SetTitle(file->Path());
#else
        const char *fileName=file->Path();
        int n=strlen(fileName);
        if(n>Width()) {
          n=n-Width()+4; if(n<0) n=0;
          char str[72];
          snprintf(str,sizeof(str),"... %s",fileName+n);
          Interface->Write(0,0,str);
          }
        else Interface->Write(0,0,fileName);
#endif
        }
      lastCurrent=lastTotal=-1;
      flush=true;
      }

    if(Current!=lastCurrent) {
#if VDRVERSNUM >= 10307
       display->SetProgress(Current, Total);
       display->SetCurrent(IndexToHMSF(Current));
       display->SetTotal(IndexToHMSF(Total));
       bool Play, Forward;
       int Speed;
       if(GetReplayMode(Play,Forward,Speed)) 
         display->SetMode(Play, Forward, Speed);
#else
       cProgressBar ProgressBar(Width() * cOsd::CellWidth(), cOsd::LineHeight(), Current, Total);
       Interface->SetBitmap(0, cOsd::LineHeight(), ProgressBar);
       Interface->Write(0,2,IndexToHMSF(Current));
       Interface->Write(-7,2,IndexToHMSF(Total));
#endif
       flush=true;
       lastCurrent=Current; lastTotal=Total;
       }
    if(flush) 
#if VDRVERSNUM >= 10307
      Skins.Flush();
#else
      Interface->Flush();
#endif
    ShowMode();
    }
}

#if VDRVERSNUM < 10307
#ifdef HAVE_BEAUTYPATCH
int forwSym[] = { FSYM_FORW,FSYM_FORW1,FSYM_FORW2,FSYM_FORW3 };
int backSym[] = { FSYM_BACK,FSYM_BACK1,FSYM_BACK2,FSYM_BACK3 };
#endif
#endif

void cMPlayerControl::ShowMode(void)
{
  if(Setup.ShowReplayMode && !jumpactive) {
    bool Play, Forward;
    int Speed;
    if(GetReplayMode(Play, Forward, Speed)) {
       bool NormalPlay = (Play && Speed == -1);

       if(!visible) {
         if(NormalPlay) return;
#if VDRVERSNUM >= 10307
         display = Skins.Current()->DisplayReplay(true);
#else
         Interface->Open(0,osdPos-1);
#endif
         visible=modeOnly=true;
         }

       if(modeOnly && !timeoutShow && NormalPlay) timeoutShow=time(0)+SELECTHIDE_TIMEOUT;

#if VDRVERSNUM >= 10307
       display->SetMode(Play, Forward, Speed);
#else
       char buf[16];
       eDvbFont OldFont;
#ifdef HAVE_BEAUTYPATCH
       if(haveBeauty) {
         int i=0;
         if(!(Width()&1)) buf[i++]=' ';
         buf[i]=FSYM_EMPTY; if(Speed>=0 && !Forward) buf[i]=backSym[Speed];
         i++;
         buf[i++]=Play?(Speed==-1?FSYM_PLAY:FSYM_EMPTY):FSYM_PAUSE;
         buf[i]=FSYM_EMPTY; if(Speed>=0 && Forward) buf[i]=forwSym[Speed];
         i++;
         if(!(Width()&1)) buf[i++]=' ';
         buf[i]=0;
         OldFont = Interface->SetFont(fontSym);
         }
       else {
#endif //HAVE_BEAUTYPATCH
         const char *Mode;
         if (Speed == -1) Mode = Play    ? "  >  " : " ||  ";
         else if (Play)   Mode = Forward ? " X>> " : " <<X ";
         else             Mode = Forward ? " X|> " : " <|X ";
         strn0cpy(buf, Mode, sizeof(buf));
         char *p = strchr(buf, 'X');
         if(p) *p = Speed > 0 ? '1' + Speed - 1 : ' ';
         OldFont = Interface->SetFont(fontFix);
#ifdef HAVE_BEAUTYPATCH
         }
#endif //HAVE_BEAUTYPATCH
       DisplayAtBottom(buf);
       Interface->SetFont(OldFont);
#endif //VDRVERSNUM >= 10307
       }
    }
}

void cMPlayerControl::JumpDisplay(void)
{
  char buf[64];
  const char *j=tr("Jump: "), u=jumpmode?'%':'m';
  if(!jumpval) sprintf(buf,"%s- %c",  j,u);
  else         sprintf(buf,"%s%d- %c",j,jumpval,u);
#if VDRVERSNUM >= 10307
  display->SetJump(buf);
#else
  DisplayAtBottom(buf);
#endif
}

void cMPlayerControl::JumpProcess(eKeys Key)
{
  const int n=Key-k0;
  switch (Key) {
    case k0 ... k9:
      {
      const int max=jumpmode?100:lastTotal;
      if(jumpval*10+n <= max) jumpval=jumpval*10+n;
      JumpDisplay();
      }
      break;
    case kBlue:
      jumpmode=!jumpmode; jumpval=0;
      DisplayAtBottom(0); JumpDisplay();
      break;
    case kPlay:
    case kUp:
      player->Goto(jumpval*(jumpmode?1:60),jumpmode,false);
      jumpactive=false;
      break;
    case kFastRew:
    case kFastFwd:
    case kLeft:
    case kRight:
      if(!jumpmode) {
        player->SkipSeconds(jumpval*60 * ((Key==kLeft || Key==kFastRew) ? -1:1));
        jumpactive=false;
        }
      break;
    default:
      jumpactive=false;
      break;
    }

  if(!jumpactive) {
    if(jumphide) Hide();
    else 
#if VDRVERSNUM >= 10307
      display->SetJump(0);
#else
      DisplayAtBottom(0);
#endif
    }
}

void cMPlayerControl::Jump(void)
{
  jumpval=0; jumphide=jumpmode=false;
  if(!visible) {
    ShowTimed(); if(!visible) return;
    jumphide=true;
    }
  JumpDisplay();
  jumpactive=true;
}

eOSState cMPlayerControl::ProcessKey(eKeys Key)
{
  if(!player->Active()) { Hide(); Stop(); return osEnd; }

  if(!player->SlaveMode()) {
    if(Key==kBlue) { Hide(); Stop(); return osEnd; }
    }
  else {
    if(visible) {
      if(timeoutShow && time(0)>timeoutShow) {
        Hide(); ShowMode();
        timeoutShow = 0;
        }
      else {
        if(modeOnly) ShowMode();
        else ShowProgress();
        }
      }

    if(jumpactive && Key != kNone) {
      JumpProcess(Key);
      return osContinue;
      }

    bool DoShowMode = true;
    switch (Key) {
      case kPlay:
      case kUp:      player->Play(); break;

      case kPause:
      case kDown:    player->Pause(); break;

      case kFastRew|k_Repeat:
      case kFastRew:
      case kLeft|k_Repeat:
      case kLeft:    player->SkipSeconds(-10); break;

      case kFastFwd|k_Repeat:
      case kFastFwd:
      case kRight|k_Repeat:
      case kRight:   player->SkipSeconds(10); break;

      case kRed:     Jump(); break;

      case kGreen|k_Repeat:                      // temporary use
      case kGreen:   player->SkipSeconds(-60); break;
      case kYellow|k_Repeat:
      case kYellow:  player->SkipSeconds(60); break;
  //    case kGreen|k_Repeat:                      // reserved for future use
  //    case kGreen:   player->SkipPrev(); break;
  //    case kYellow|k_Repeat:
  //    case kYellow:  player->SkipNext(); break;

      case kStop:
      case kBlue:
      case kBack:    Hide(); Stop(); return osEnd;

      default:
        DoShowMode = false;
        switch(Key) {
          case kOk: if(visible && !modeOnly) { Hide(); DoShowMode=true; }
                    else ShowTimed();
                    break;
          case k0:  player->DvdNav(navMenu); break;
          case k1:  player->AudioDelay(1); break;
          case k2:  player->DvdNav(navUp); break;
          case k3:  if(visible && !modeOnly) {
                      Hide();
                      osdPos--; if(osdPos<-6) osdPos=-6;
                      ShowTimed();
                      }
                    break;
          case k4:  player->DvdNav(navLeft); break;
          case k5:  player->DvdNav(navSelect); break;
          case k6:  player->DvdNav(navRight); break;
          case k7:  player->AudioDelay(-1); break;
          case k8:  player->DvdNav(navDown); break;
          case k9:  if(visible && !modeOnly) {
                      Hide();
                      osdPos++; if(osdPos>0) osdPos=0;
                      ShowTimed();
                      }
                    break;
          default:  break;
          }
        break;
      }

    if(DoShowMode) ShowMode();
    }
  return osContinue;
}

// --- cMenuMPlayAid -----------------------------------------------------------

class cMenuMPlayAid : public cOsdMenu {
public:
  cMenuMPlayAid(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuMPlayAid::cMenuMPlayAid(void)
:cOsdMenu(tr("MPlayer Audio ID"),20)
{
  Add(new cMenuEditIntItem(tr("Audiostream ID"),&MPlayerAid,0,255));
  Display();
}

eOSState cMenuMPlayAid::ProcessKey(eKeys Key)
{
  eOSState state=cOsdMenu::ProcessKey(Key);
  if(state==osUnknown) {
    switch(Key) {
      case kOk: state=osBack; break;
      default:  break;
      }
    }
  return state;
}

// --- cMenuMPlayBrowse ---------------------------------------------------------

class cMenuMPlayBrowse : public cMenuBrowse {
private:
  bool sourcing, aidedit;
  eOSState Source(bool second);
protected:
  virtual void SetButtons(void);
public:
  cMenuMPlayBrowse(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuMPlayBrowse::cMenuMPlayBrowse(void)
:cMenuBrowse(MPlaySources.GetSource(),false,false,tr("MPlayer browser"))
{
  sourcing=aidedit=false;
  SetButtons();
}

void cMenuMPlayBrowse::SetButtons(void)
{
  static char blue[12];
  snprintf(blue,sizeof(blue),"AID:%d",MPlayerAid);
  SetHelp(tr("Play"), MPlayerSetup.ResumeMode ? tr("Rewind"):0, tr("Source"), blue);
  Display();
}

eOSState cMenuMPlayBrowse::Source(bool second)
{
  if(HasSubMenu()) return osContinue;

  if(!second) {
    sourcing=true;
    return AddSubMenu(new cMenuSource(&MPlaySources,tr("MPlayer source")));
    }
  sourcing=false;
  cFileSource *src=cMenuSource::GetSelected();
  if(src) {
    MPlaySources.SetSource(src);
    SetSource(src);
    NewDir(0);
    }
  return osContinue;
}

eOSState cMenuMPlayBrowse::ProcessKey(eKeys Key)
{
  eOSState state=cOsdMenu::ProcessKey(Key);
  if(state==osContinue && !HasSubMenu()) {
    if(sourcing) return Source(true);
    if(aidedit) { aidedit=false; SetButtons(); }
    }
  bool rew=false;
  if(state==osUnknown) {
    switch(Key) {
      case kGreen:
        {
        cFileObj *item=CurrentItem();
        if(item && item->Type()==otFile) {
          lastselect=new cFileObj(item);
          state=osBack;
          rew=true;
          } 
        else state=osContinue;
        break;
        }
      case kYellow:
        state=Source(false);
        break;
      case kBlue:
        aidedit=true;
        state=AddSubMenu(new cMenuMPlayAid);
        break;
      default:
        break;
      }
    }
  if(state==osUnknown) state=cMenuBrowse::ProcessStdKey(Key,state);
  if(state==osBack && lastselect) {
    cMPlayerControl::SetFile(lastselect,rew);
    cControl::Launch(new cMPlayerControl);
    return osEnd;
    }
  return state;
}

// --- cPluginMPlayer ----------------------------------------------------------

static const char *VERSION        = PLUGIN_VERSION;
static const char *DESCRIPTION    = "Media replay via MPlayer";
static const char *MAINMENUENTRY  = "MPlayer";

class cPluginMPlayer : public cPlugin {
public:
  cPluginMPlayer(void);
  virtual ~cPluginMPlayer();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
#if VDRVERSNUM >= 10131
  virtual bool Initialize(void);
#else
  virtual bool Start(void);
#endif
  virtual const char *MainMenuEntry(void);
  virtual cOsdMenu *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginMPlayer::cPluginMPlayer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  status=0;
}

cPluginMPlayer::~cPluginMPlayer()
{
  delete status;
}

const char *cPluginMPlayer::CommandLineHelp(void)
{
  static char *help_str=0;
  
  free(help_str);    //                                     for easier orientation, this is column 80|
  asprintf(&help_str,"  -m CMD,   --mount=CMD    use CMD to mount/unmount/eject mp3 sources\n"
                     "                           (default: %s)\n"
                     "  -M CMD,   --mplayer=CMD  use CMD when calling MPlayer\n"
                     "                           (default: %s)\n",
                     mountscript,
                     MPlayerCmd
                     );
  return help_str;
}

bool cPluginMPlayer::ProcessArgs(int argc, char *argv[])
{
  static struct option long_options[] = {
      { "mount",    required_argument, NULL, 'm' },
      { "mplayer",  required_argument, NULL, 'M' },
      { NULL }
    };

  int c, option_index = 0;
  while((c=getopt_long(argc,argv,"m:M:",long_options,&option_index))!=-1) {
    switch (c) {
      case 'm': mountscript=optarg; break;
      case 'M': MPlayerCmd=optarg; break;
      default:  return false;
      }
    }
  return true;
}

#if VDRVERSNUM >= 10131
bool cPluginMPlayer::Initialize(void)
#else
bool cPluginMPlayer::Start(void)
#endif
{
  if(!CheckVDRVersion(1,1,16,"mplayer")) return false;
  i18n_name=Name();
  MPlaySources.Load(AddDirectory(ConfigDirectory(),"mplayersources.conf"));
  if(MPlaySources.Count()<1) {
    esyslog("ERROR: you must have defined at least one source in mplayersources.conf");
    fprintf(stderr,"No source(s) defined in mplayersources.conf\n");
    return false;
    }
  RegisterI18n(Phrases);
  if(!(status=new cMPlayerStatus)) return false;
  return true;
}

const char *cPluginMPlayer::MainMenuEntry(void)
{
  return MPlayerSetup.HideMainMenu ? 0 : tr(MAINMENUENTRY);
}

cOsdMenu *cPluginMPlayer::MainMenuAction(void)
{
  return new cMenuMPlayBrowse;
}

cMenuSetupPage *cPluginMPlayer::SetupMenu(void)
{
  return new cMenuSetupMPlayer;
}

bool cPluginMPlayer::SetupParse(const char *Name, const char *Value)
{
  if(      !strcasecmp(Name, "ControlMode"))  MPlayerSetup.SlaveMode    = atoi(Value);
  else if (!strcasecmp(Name, "HideMainMenu")) MPlayerSetup.HideMainMenu = atoi(Value);
  else if (!strcasecmp(Name, "ResumeMode"))   MPlayerSetup.ResumeMode   = atoi(Value);
  else return false;
  return true;
}

VDRPLUGINCREATOR(cPluginMPlayer); // Don't touch this!
