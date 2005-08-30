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

#ifdef TEST_MAIN
#define HAVE_SNDFILE
#define REMOTE_LIRC
#define _GNU_SOURCE
#define DEBUG
#endif

#ifdef HAVE_SNDFILE

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include "common.h"
#include "setup-mp3.h"
#include "decoder-snd.h"
#include "data.h"
#include "network.h"
#include "menu-async.h"
#include "i18n.h"
#include "version.h"

#ifndef SNDFILE_1
#error You must use libsndfile version 1.x.x
#endif

#define CDFS_TRACK_OFF 150
#define CDFS_PROC      "/proc/cdfs"
#define CDFS_MARK_ID   "CD (discid=%x) contains %d tracks:"
#define CDFS_MARK_TR   "%*[^[][ %d - %d"
#define CDFS_TRACK     "track-"

#define CDDB_PROTO 5                   // used protocol level
#define CDDB_TOUT  30*1000             // connection timeout (ms)

const char *cddbpath="/var/lib/cddb";  // default local cddb path

#define CDDB_DEBUG  // debug cddb queries
//#define DEBUG_CDFS  // debug cdfs parsing
//#define GUARD_DEBUG // enable framebuffer guard

#ifdef DEBUG_CDFS
#define dc(x) { (x); }
#else
#define dc(x) ; 
#endif

#ifndef TEST_MAIN

// --- cSndDecoder -------------------------------------------------------------

#define SF_SAMPLES (sizeof(pcm->samples[0])/sizeof(mad_fixed_t))

cSndDecoder::cSndDecoder(const char *Filename)
:cDecoder(Filename)
,file(Filename)
,info(&file)
{
  pcm=0; framebuff=0; playing=ready=false;
}

cSndDecoder::~cSndDecoder()
{
  Clean();
}

bool cSndDecoder::Valid(void)
{
  bool res=false;
  if(TryLock()) {
    if(file.Open(false)) res=true;
    cDecoder::Unlock();
    }
  return res;
}

cFileInfo *cSndDecoder::FileInfo(void)
{
  cFileInfo *fi=0;
  if(file.HasInfo()) fi=&file;
  else if(TryLock()){
    if(file.Open()) { fi=&file; file.Close(); }
    cDecoder::Unlock();
    }
  return fi;
}

cSongInfo *cSndDecoder::SongInfo(bool get)
{
  cSongInfo *si=0;
  if(info.HasInfo()) si=&info;
  else if(get && TryLock()) {
    if(info.DoScan(false)) si=&info;
    cDecoder::Unlock();
    }
  return si;
}

cPlayInfo *cSndDecoder::PlayInfo(void)
{
  if(playing) {
    pi.Index=index/info.SampleFreq;
    pi.Total=info.Total;
    return &pi;
    }
  return 0;
}

void cSndDecoder::Init(void)
{
  Clean();
  pcm=new struct mad_pcm;
  framebuff=MALLOC(int,2*SF_SAMPLES+8);
#ifdef GUARD_DEBUG
  for(int i=0; i<8; i++) framebuff[i+(SF_SAMPLES*2)-4]=0xdeadbeaf;
#endif
  index=0;
}

bool cSndDecoder::Clean(void)
{
  playing=false;

  buffMutex.Lock();
  run=false; bgCond.Broadcast();
  buffMutex.Unlock();
  cThread::Cancel(3);

  buffMutex.Lock();
  if(!ready) { deferedN=-1; ready=true; }
  fgCond.Broadcast();
  buffMutex.Unlock();

  delete pcm; pcm=0;
#ifdef GUARD_DEBUG
  if(framebuff) {
    printf("snd: bufferguard");
    for(int i=0; i<8; i++) printf(" %08x",framebuff[i+(SF_SAMPLES*2)-4]);
    printf("\n");
    }
#endif
  free(framebuff); framebuff=0;
  file.Close();
  return false;
}

bool cSndDecoder::Start(void)
{
  cDecoder::Lock(true);
  Init(); playing=true;
  if(file.Open() && info.DoScan(true)) {
    d(printf("snd: open rate=%d frames=%lld channels=%d format=0x%x seek=%d\n",
             file.sfi.samplerate,file.sfi.frames,file.sfi.channels,file.sfi.format,file.sfi.seekable))
    if(file.sfi.channels<=2) {
      ready=false; run=true; softCount=0;
      cThread::Start();
      cDecoder::Unlock();
      return true;
      }
    else esyslog("ERROR: cannot play sound file %s: more than 2 channels",filename);
    }
  cDecoder::Unlock();
  return Clean();
}

bool cSndDecoder::Stop(void)
{
  cDecoder::Lock();
  if(playing) Clean();
  cDecoder::Unlock();
  return true;
}

void cSndDecoder::Action(void)
{
  buffMutex.Lock();
  while(run) {
    if(ready) bgCond.Wait(buffMutex);
    if(!ready) {
      buffMutex.Unlock();
      deferedN=file.Stream(framebuff,SF_SAMPLES);
      buffMutex.Lock();
      ready=true; fgCond.Broadcast();
      }
    }
  buffMutex.Unlock();
}

struct Decode *cSndDecoder::Done(eDecodeStatus status)
{
  ds.status=status;
  ds.index=index*1000/info.SampleFreq;
  ds.pcm=pcm;
  cDecoder::Unlock(); // release the lock from Decode()
  return &ds;
}

struct Decode *cSndDecoder::Decode(void)
{
  cDecoder::Lock(); // this is released in Done()
  if(playing) {
    cMutexLock lock(&buffMutex);
    while(!ready)
      if(!softCount || !fgCond.TimedWait(buffMutex,softCount*5)) {
        if(softCount<20) softCount++;
        return Done(dsSoftError);
        }
    softCount=0;
    ready=false; bgCond.Broadcast();

    int n=deferedN;
    if(n<0) return Done(dsError);
    if(n==0) return Done(dsEof);

    pcm->samplerate=file.sfi.samplerate;
    pcm->channels=file.sfi.channels;
    pcm->length=n;
    index+=n;

    int *data=framebuff;
    mad_fixed_t *sam0=pcm->samples[0], *sam1=pcm->samples[1]; 
    const int s=(sizeof(int)*8)-1-MAD_F_FRACBITS; // shift value for mad_fixed conversion
    if(pcm->channels>1) {
      for(; n>0 ; n--) {
        *sam0++=(*data++) >> s;
        *sam1++=(*data++) >> s;
        }
      }
    else {
      for(; n>0 ; n--)
        *sam0++=(*data++) >> s;
      }
    return Done(dsPlay);
    }
  return Done(dsError);
}

bool cSndDecoder::Skip(int Seconds, float bsecs)
{
  cDecoder::Lock();
  bool res=false;
  if(playing && file.sfi.seekable) {
    float fsecs=(float)Seconds-bsecs;
    sf_count_t frames=(sf_count_t)(fsecs*(float)file.sfi.samplerate);
    sf_count_t newpos=file.Seek(0,true)+frames;
    if(newpos>file.sfi.frames) newpos=file.sfi.frames-1;
    if(newpos<0) newpos=0;
    d(printf("snd: skip: secs=%d fsecs=%f frames=%lld current=%lld new=%lld\n",Seconds,fsecs,frames,file.Seek(0,true),newpos))

    buffMutex.Lock();
    frames=file.Seek(newpos,false);
    ready=false; bgCond.Broadcast();
    buffMutex.Unlock();
    if(frames>=0) {
      index=frames;
#ifdef DEBUG
      int i=frames/file.sfi.samplerate;
      printf("snd: skipping to %02d:%02d (frame %lld)\n",i/60,i%60,frames);
#endif
      res=true;
      }
    }
  cDecoder::Unlock();
  return res;
}

#endif //TEST_MAIN

// --- cDiscID -------------------------------------------------------------------

class cDiscID {
public:
  int discid, ntrks, nsecs;
  int *offsets;
  //
  cDiscID(void);
  ~cDiscID();
  bool Get(void);
  };

cDiscID::cDiscID(void)
{
  offsets=0; discid=ntrks=0;
}

cDiscID::~cDiscID()
{
  delete offsets;
}

bool cDiscID::Get(void)
{
  bool res=false;
  FILE *f=fopen(CDFS_PROC,"r");
  if(f) {
    char line[256];
    bool state=false;
    int tr=0;
    while(fgets(line,sizeof(line),f)) {
      if(!state) {
        int id, n;
        if(sscanf(line,CDFS_MARK_ID,&id,&n)==2) {
          d(printf("discid: found id=%08x n=%d\n",id,n))
          if(discid==id && ntrks==n) {
            res=true;
            break;
            }
          else {
            discid=id; ntrks=n;
            delete offsets; offsets=new int[ntrks];
            state=true;
            }
          }
        }
      else {
        int off, end;
        if(sscanf(line,CDFS_MARK_TR,&off,&end)==2) {
          dc(printf("discid: found offset=%d end=%d for track %d\n",off,end,tr+1))
          offsets[tr]=off+CDFS_TRACK_OFF;
          if(++tr==ntrks) {
            nsecs=(end+1)/75;
            dc(printf("discid: nsecs=%d / 0x%x\n",nsecs,nsecs))
            res=true;
            break;
            }
          }
        }
      }
    fclose(f);
    }
  return res;
}

// --- cCDDBSong ---------------------------------------------------------------

// The CDDB code is loosely based on the implementation in mp3c 0.27 which is
// (C) 1999-2001 WSPse, Matthias Hensler, <matthias@wspse.de>

class cCDDBSong : public cListObject {
public:
  cCDDBSong(void);
  ~cCDDBSong();
  //
  int Track;
  char *TTitle, *ExtT;
  char *Title, *Artist;
  };

cCDDBSong::cCDDBSong(void)
{
  Title=Artist=0; TTitle=ExtT=0;
}

cCDDBSong::~cCDDBSong()
{
  free(Title);
  free(Artist);
  free(TTitle);
  free(ExtT);
}

// --- cCDDBDisc ---------------------------------------------------------------

const char *sampler[] = { // some artist names to identify sampler discs
  "various",
  "varios",
  "variété",
  "compilation",
  "sampler",
  "mixed",
  "divers",
  "v.a.",
  "VA",
  "misc",
  "none",
  0 };

class cCDDBDisc : public cList<cCDDBSong> {
private:
  int DiscID;
  //
  bool isSampler;
  char *DTitle, *ExtD;
  //
  cCDDBSong *GetTrack(const char *name, unsigned int pos);
  cCDDBSong *FindTrack(int tr);
  void Strcat(char * &store, char *value);
  bool Split(const char *source, char div, char * &first, char * &second, bool only3=false);
  void Put(const char *from, char * &to);
  void Clean(void);
public:
  cCDDBDisc(void);
  ~cCDDBDisc();
  bool Load(cDiscID *id, const char *filename);
  bool Cached(cDiscID *id) { return DiscID==id->discid; }
  bool TrackInfo(int tr, cSongInfo *si);
  //
  char *Album, *Artist;
  int Year;
  };

cCDDBDisc::cCDDBDisc(void)
{
  Album=Artist=0; DTitle=ExtD=0; DiscID=0;
}

cCDDBDisc::~cCDDBDisc()
{
  Clean();
}

void cCDDBDisc::Clean(void)
{
  free(DTitle); DTitle=0;
  free(ExtD); ExtD=0;
  free(Artist); Artist=0;
  free(Album); Album=0;
  Year=-1; DiscID=0; isSampler=false;
}

bool cCDDBDisc::TrackInfo(int tr, cSongInfo *si)
{
  cCDDBSong *s=FindTrack(tr);
  if(s) {
    Put(s->Title,si->Title);
    if(s->Artist) Put(s->Artist,si->Artist); else Put(Artist,si->Artist);
    Put(Album,si->Album);
    if(Year>0) si->Year=Year;
    return true;
    }
  return false;
}

void cCDDBDisc::Put(const char *from, char * &to)
{
  free(to);
  to=from ? strdup(from):0;
}

bool cCDDBDisc::Load(cDiscID *id, const char *filename)
{
  char *p;
  Clean(); Clear();

  d(printf("cddb: loading discid %08x from %s\n",id->discid,filename))
  DiscID=id->discid;
  FILE *f=fopen(filename,"r");
  if(f) {
    char buff[1024];
    while(fgets(buff,sizeof(buff),f)) {
      int i=strlen(buff);
      while(i && (buff[i-1]=='\n' || buff[i-1]=='\r')) buff[--i]=0;

      if(buff[0]=='#') { // special comment line handling
        }
      else {
        p=strchr(buff,'=');
        if(p) {
           *p=0;
           char *name =compactspace(buff);
           char *value=compactspace(p+1);
           if(*name && *value) {
             if(!strcasecmp(name,"DTITLE")) Strcat(DTitle,value);
             else if(!strcasecmp(name,"EXTD")) Strcat(ExtD,value);
             else if(!strcasecmp(name,"DYEAR")) Year=atoi(value);
             else if(!strncasecmp(name,"TTITLE",6)) {
               cCDDBSong *s=GetTrack(name,6);
               if(s) Strcat(s->TTitle,value);
               }
             else if(!strncasecmp(name,"EXTT",4)) {
               cCDDBSong *s=GetTrack(name,4);
               if(s) Strcat(s->ExtT,value);
               }
             }
           }
        }
      }
    fclose(f);

    // read all data, now post-processing
    if(Count()>0) {
      if(DTitle) {
        if(Split(DTitle,'/',Artist,Album)) {
          for(int n=0 ; sampler[n] ; n++)
            if(!strncasecmp(Artist,sampler[n],strlen(sampler[n]))) {
              isSampler=true;
              break;
              }
          }
        else {
          Album=strdup(DTitle);
          isSampler=true;
          }
        }
      d(printf("cddb: found artist='%s' album='%s' isSampler=%d\n",Artist,Album,isSampler))
      free(DTitle); DTitle=0;

      if(!isSampler && Artist && Album && !strncmp(Album,Artist,strlen(Artist))) {
        d(printf("cddb: detecting sampler from Artist==Album\n"))
        isSampler=true;
        }

      if(!isSampler) {
        int nofail1=0, nofail2=0;
        cCDDBSong *s=First();
        while(s) {
          if(s->TTitle) {
            if(strstr(s->TTitle," / ")) nofail1++;
            //if(strstr(s->TTitle," - ")) nofail2++;
            }
          s=Next(s);
          }
        if(nofail1==Count() || nofail2==Count()) {
          d(printf("cddb: detecting sampler from nofail\n"))
          isSampler=true;
          }
        }

      if(Year<0 && ExtD && (p=strstr(ExtD,"YEAR:"))) Year=atoi(p+5);
      free(ExtD); ExtD=0;
      d(printf("cddb: found year=%d\n",Year))

      cCDDBSong *s=First();
      while(s) {
        if(s->TTitle) {
          if(isSampler) {
            if(!Split(s->TTitle,'/',s->Artist,s->Title) && 
               !Split(s->TTitle,'-',s->Artist,s->Title,true)) {
              s->Title=compactspace(strdup(s->TTitle));
              if(s->ExtT) s->Artist=compactspace(strdup(s->ExtT));
              }
            }
          else {
            s->Title=compactspace(strdup(s->TTitle));
            if(Artist) s->Artist=strdup(Artist);
            }
          }
        else s->Title=strdup(tr("unknown"));

        free(s->TTitle); s->TTitle=0;
        free(s->ExtT); s->ExtT=0;
        d(printf("cddb: found track %d title='%s' artist='%s'\n",s->Track,s->Title,s->Artist))
        s=Next(s);
        }
      return true;
      }
    }
  return false;
}

bool cCDDBDisc::Split(const char *source, char div, char * &first, char * &second, bool only3)
{
  int pos=-1, n=0;
  char *p, l[4]={ ' ',div,' ',0 };
  if ((p=strstr(source,l))) { pos=p-source; n=3; }
  else if(!only3 && (p=strchr(source,div)))  { pos=p-source; n=1; }
  if(pos>=0) {
    free(first); first=strdup(source); first[pos]=0; compactspace(first);
    free(second); second=strdup(source+pos+n); compactspace(second);
    return true;
    }
  return false;
}

void cCDDBDisc::Strcat(char * &store, char *value)
{
  if(store) {
    char *n=MALLOC(char,strlen(store)+strlen(value)+1);
    if(n) {
      strcpy(n,store);
      strcat(n,value);
      free(store); store=n;
      }
    }
  else store=strdup(value);
}

cCDDBSong *cCDDBDisc::GetTrack(const char *name, unsigned int pos)
{
  cCDDBSong *s=0;
  if(strlen(name)>pos) {
    int tr=atoi(&name[pos]);
    s=FindTrack(tr);
    if(!s) {
      s=new cCDDBSong;
      Add(s);
      s->Track=tr;
      }
    }
  return s;
}

cCDDBSong *cCDDBDisc::FindTrack(int tr)
{
  cCDDBSong *s=First();
  while(s) {
    if(s->Track==tr) break;
    s=Next(s);
    }
  return s;
}

#ifndef TEST_MAIN

// --- cCDDB -------------------------------------------------------------------

class cCDDB : public cScanDir, cMutex {
private:
  cCDDBDisc cache;
  cFileSource *src;
  cFileObj *file;
  cNet *net;
  char searchID[10], cddbstr[256];
  //
  virtual void DoItem(cFileSource *src, const char *subdir, const char *name);
  bool LocalQuery(cDiscID *id);
  bool RemoteGet(cDiscID *id);
  bool GetLine(char *buff, int size, bool log=true);
  int GetCddbResponse(void);
  int DoCddbCmd(char *format, ...);
public:
  cCDDB(void);
  virtual ~cCDDB();
  bool Lookup(cDiscID *id, int track, cSongInfo *si);
  };

cCDDB cddb;

cCDDB::cCDDB(void)
{
  src=0; file=0; net=0;
}

cCDDB::~cCDDB()
{
  delete file;
  delete src;
  delete net;
}

bool cCDDB::Lookup(cDiscID *id, int track, cSongInfo *si)
{
  bool res=false;
  Lock();
  if(!cache.Cached(id)) {
    if(LocalQuery(id) || (MP3Setup.UseCddb>1 && RemoteGet(id) && LocalQuery(id)))
      cache.Load(id,file->FullPath());
    }
  if(cache.Cached(id) && cache.TrackInfo(track,si)) res=true;
  Unlock();
  return res;
}

bool cCDDB::LocalQuery(cDiscID *id)
{
  bool res=false;
  delete file; file=0;
  if(!src) src=new cFileSource(cddbpath,"CDDB database",false);
  if(src) {
    snprintf(searchID,sizeof(searchID),"%08x",id->discid);
    if(ScanDir(src,0,stDir,0,0,false) && file) res=true;
    }
  return res;
}

void cCDDB::DoItem(cFileSource *src, const char *subdir, const char *name)
{
  if(!file) {
    file=new cFileObj(src,name,searchID,otFile);
    if(access(file->FullPath(),R_OK)) { delete file; file=0; }
    }
}

bool cCDDB::RemoteGet(cDiscID *id)
{
  bool res=false;
  asyncStatus.Set(tr("Remote CDDB lookup..."));

  delete net; net=new cNet(16*1024,CDDB_TOUT,CDDB_TOUT);
  if(net->Connect(MP3Setup.CddbHost,MP3Setup.CddbPort)) {
    int code=GetCddbResponse();
    if(code/100==2) {
      char *host=getenv("HOSTNAME"); if(!host) host="unknown";
      char *user=getenv("USER"); if(!user) user="nobody";
      code=DoCddbCmd("cddb hello %s %s %s %s\n",user,host,PLUGIN_NAME,PLUGIN_VERSION);
      if(code/100==2) {
        code=DoCddbCmd("proto %d\n",CDDB_PROTO);
        if(code>0) {
          char off[1024];
          off[0]=0; for(int i=0 ; i<id->ntrks ; i++) sprintf(&off[strlen(off)]," %d",id->offsets[i]);

          code=DoCddbCmd("cddb query %08x %d %s %d\n",id->discid,id->ntrks,off,id->nsecs);
          if(code/100==2) {
            char *cat=0;
            if(code==200) cat=strdup(cddbstr);
            else if(code==210) {
              if(GetLine(off,sizeof(off))) {
                cat=strdup(off);
                while(GetLine(off,sizeof(off)) && off[0]!='.');
                }
              }

            if(cat) {
              char *s=index(cat,' '); if(s) *s=0;
              code=DoCddbCmd("cddb read %s %08x\n",cat,id->discid);
              if(code==210) {
                char *name=0;
                asprintf(&name,"%s/%s/%08x",cddbpath,cat,id->discid);
                if(MakeDirs(name,false)) {
                  FILE *out=fopen(name,"w");
                  if(out) {
                    while(GetLine(off,sizeof(off),false) && off[0]!='.') fputs(off,out);
                    fclose(out);
                    res=true;
                    }
                  else esyslog("fopen() failed: %s",strerror(errno));
                  }
                free(name);
                }
              else if(code>0) esyslog("server read error: %d %s",code,cddbstr);
              free(cat);
              }
            }
          else if(code>0) esyslog("server query error: %d %s",code,cddbstr);
          }
        else esyslog("server proto error: %d %s",code,cddbstr);
        }
      else if(code>0) esyslog("server hello error: %d %s",code,cddbstr);
      }
    else if(code>0) esyslog("server sign-on error: %d %s",code,cddbstr);
    }

  delete net; net=0;
  asyncStatus.Set(0);
  return res;
}

bool cCDDB::GetLine(char *buff, int size, bool log)
{
  if(net->Gets(buff,size)>0) {
#ifdef CDDB_DEBUG
    if(log) printf("cddb: <- %s",buff);
#endif
    return true;
    }
  return false;
}

int cCDDB::GetCddbResponse(void)
{
  char buf[1024];
  if(GetLine(buf,sizeof(buf))) {
    int code;
    if(sscanf(buf,"%d %255[^\n]",&code,cddbstr)==2) return code;
    else esyslog("Unexpected server response: %s",buf);
    }
  return -1;
}

int cCDDB::DoCddbCmd(char *format, ...)
{
  va_list ap;
  va_start(ap,format);
  char *buff=0;
  vasprintf(&buff,format,ap);
  va_end(ap);
#ifdef CDDB_DEBUG
  printf("cddb: -> %s",buff);
#endif
  int r=net->Puts(buff);
  free(buff);
  if(r<0) return -1;
  return GetCddbResponse();
}

// --- cSndInfo ----------------------------------------------------------------

cSndInfo::cSndInfo(cSndFile *File)
{
  file=File;
  id=new cDiscID;
}

cSndInfo::~cSndInfo()
{
  delete id;
}

bool cSndInfo::Abort(bool result)
{
  if(!keepOpen) file->Close();
  return result;
}

bool cSndInfo::DoScan(bool KeepOpen)
{
  keepOpen=KeepOpen;
  if(!file->Open()) return Abort(false);
  if(HasInfo()) return Abort(true);

  // check the infocache
  cCacheData *dat=InfoCache.Search(file);
  if(dat) {
    Set(dat); dat->Unlock();
    if(!DecoderID) {
      DecoderID=DEC_SND;
      InfoCache.Cache(this,file,true);
      }
    return Abort(true);
    }

  Clear();

  if(file->FsType!=CDFS_MAGIC || !MP3Setup.UseCddb || !CDDBLookup(file->Filename))
    FakeTitle(file->Filename);

  Frames=file->sfi.frames;
  SampleFreq=file->sfi.samplerate;
  Channels=file->sfi.channels;
  ChMode=Channels>1 ? 3:0;
  Total=Frames/SampleFreq;
  Bitrate=file->Filesize*8/Total; //XXX SampleFreq*Channels*file->sfi.pcmbitwidth;
  DecoderID=DEC_SND;

  InfoDone();
  InfoCache.Cache(this,file);
  return Abort(true);  
}

bool cSndInfo::CDDBLookup(const char *filename)
{
  if(id->Get()) {
    int tr;
    char *s=strstr(filename,CDFS_TRACK);
    if(s && sscanf(s+strlen(CDFS_TRACK),"%d",&tr)==1) {
      d(printf("snd: looking up disc id %08x track %d\n",id->discid,tr))
      return cddb.Lookup(id,tr-1,this);
      }
    }
  return false;
}

// --- cSndFile ----------------------------------------------------------------

cSndFile::cSndFile(const char *Filename)
:cFileInfo(Filename)
{
  sf=0;
}

cSndFile::~cSndFile()
{
  Close();
}

bool cSndFile::Open(bool log)
{
  if(sf) return (Seek()>=0);

  if(FileInfo(log)) {
    sf=sf_open(Filename,SFM_READ,&sfi);
    if(!sf && log) Error("open");
    }
  return (sf!=0);
}

void cSndFile::Close(void)
{
  if(sf) { sf_close(sf); sf=0; } 
}

void cSndFile::Error(const char *action)
{
  char buff[128];
  sf_error_str(sf,buff,sizeof(buff));
  esyslog("ERROR: sndfile %s failed on %s: %s",action,Filename,buff);
}

sf_count_t cSndFile::Seek(sf_count_t frames, bool relativ)
{
  int dir=SEEK_CUR;
  if(!relativ) dir=SEEK_SET;
  int n=sf_seek(sf,frames,dir);
  if(n<0) Error("seek");
  return n;
}

sf_count_t cSndFile::Stream(int *buffer, sf_count_t frames)
{
  sf_count_t n=sf_readf_int(sf,buffer,frames);
  if(n<0) Error("read");
  return n;
}

#endif //TEST_MAIN
#endif //HAVE_SNDFILE

#ifdef TEST_MAIN
//
// to compile:
// g++ -g -DTEST_MAIN -o test mp3-decoder-snd.c tools.o thread.o -lpthread
//
// calling:
// test <cddb-file>
//

extern const char *tr(const char *test)
{
  return test;
}

int main (int argc, char *argv[])
{
  cCDDBDisc cddb;
  
  cddb.Load(1,argv[1]);
  return 0;
}
#endif
