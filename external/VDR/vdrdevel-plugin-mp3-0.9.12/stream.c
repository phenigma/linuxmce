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
#include <unistd.h>
#include <errno.h>

#include "common.h"
#include "setup-mp3.h"
#include "stream.h"
#include "network.h"
#include "menu-async.h"
#include "i18n.h"
#include "version.h"

#define USE_MMAP
#define MAX_MMAP_SIZE   (32*1024*1024)
#define MP3FILE_BUFSIZE (32*1024)

#ifdef USE_MMAP
#include <sys/mman.h>
#endif

#define DEFAULT_PORT 80 // default port for streaming (HTTP)

//#define DUMP_HEAD "/var/tmp/headers"

// --- cIO ---------------------------------------------------------------------

#if 0
cIO::cIO(const char *Filename, bool Log)
:cFileInfo(Filename)
{
  log=Log;
  readpos=0;
}

cIO::~cIO()
{
}

// --- cStreamIO ---------------------------------------------------------------

cStreamIO::cStreamIO(void)
{
  data=0;
}

cStreamIO::~cStreamIO()
{
  StreamClear(true);
}

void cStreamIO::StreamClear(bool all)
{
  if(all) { free(data); data=0; }
  fill=0;
}

unsigned char *cStreamIO::StreamInit(int Size)
{
  StreamClear(true);
  size=Size; data=(unsigned char *)malloc(size);
  return data;
}

int cStreamIO::Stream(const unsigned char *rest)
{
  if(rest && fill) { // copy remaining data to start of buffer
    fill-=(rest-data);
    memmove(data,rest,fill);
    }
  else fill=0;

  unsigned long r=Read(data+fill,size-fill);
  if(r>=0) { 
    fill+=r;
    return fill;
    }
  return -1;    
}

// --- cFileIO -----------------------------------------------------------------

cFileIO::cFileIO(const char *Filename, bool Log)
:cIO(Filename,Log)
{
  fd=-1;
}

cFileIO::~cFileIO()
{
  Close();
}

bool cFileIO::Open(void)
{
  if(fd>=0) return Seek(0,SEEK_SET);
  if(FileInfo(log)) {
    if((fd=open(Filename,O_RDONLY))>=0) {
      StreamClear(false);
      readpos=0;
      return true;
      }
    else if(log) { esyslog("ERROR: open failed on %s: %s",Filename,strerror(errno)); }
    }
  Close();
  return false;
}

void cFileIO::Close(void)
{
  if(fd>=0) { close(fd); fd=-1; }
}

int cFileIO::Read(unsigned char *Data, int Size)
{
  unsigned long r=read(fd,Data,Size);
  if(r>=0) readpos+=r;
  else if(log) { esyslog("ERROR: read failed in %s: %s",Filename,strerror(errno)); }
  return r;
}

bool cFileIO::Seek(unsigned long long pos, int whence)
{
  if(pos>=0 && pos<=Filesize) {
    StreamClear(false);
    if((readpos=lseek64(fd,pos,whence))>=0) {
      if(readpos!=pos) { dsyslog("seek mismatch in %s, wanted %lld, got %lld",Filename,pos,readpos); }
      return true;
      }
    else if(log) { esyslog("ERROR: seek failed in %s: %s",Filename,strerror(errno)); }
    }
  else d(printf("mp3: bad seek call fd=%d pos=%lld name=%s\n",fd,pos,Filename))
  return false;
}
#endif

// --- cStream -----------------------------------------------------------------

cStream::cStream(const char *Filename)
:cFileInfo(Filename)
{
  fd=-1; ismmap=false; buffer=0;
}

cStream::~cStream()
{
  Close();
}

bool cStream::Open(bool log)
{
  if(fd>=0) return Seek();

  if(FileInfo(log)) {
    if((fd=open(Filename,O_RDONLY))>=0) {
      buffpos=readpos=0; fill=0;

#ifdef USE_MMAP
      if(Filesize<=MAX_MMAP_SIZE) {
        buffer=(unsigned char *)mmap(0,Filesize,PROT_READ,MAP_SHARED,fd,0);
        if(buffer!=MAP_FAILED) {
          ismmap=true;
          return true;
          }
        else dsyslog("mmap() failed for %s: %s",Filename,strerror(errno));
        }
#endif

        buffer = new unsigned char[MP3FILE_BUFSIZE];
        if(buffer) return true;
        else { esyslog("ERROR: not enough memory for buffer: %s",Filename); }
      }
    else if(log) { esyslog("ERROR: failed to open file %s: %s",Filename,strerror(errno)); }
    }

  Close();
  return false;
}

void cStream::Close(void)
{
#ifdef USE_MMAP
  if(ismmap) { 
    munmap(buffer,Filesize); buffer=0; ismmap=false;
    }
  else {
#endif
    delete buffer; buffer=0;
#ifdef USE_MMAP
    }
#endif
  if(fd>=0) { close(fd); fd=-1; }
}

bool cStream::Seek(unsigned long long pos)
{
  if(fd>=0 && pos>=0 && pos<=Filesize) {
    buffpos=0; fill=0;
    if(ismmap) {
      readpos=pos;
      return true;
      }
    else {
      if((readpos=lseek64(fd,pos,SEEK_SET))>=0) {
        if(readpos!=pos) { dsyslog("seek mismatch in %s, wanted %lld, got %lld",Filename,pos,readpos); }
        return true;
        }
      else { esyslog("ERROR: seeking failed in %s: %d,%s",Filename,errno,strerror(errno)); }
      }
    }
  else d(printf("mp3: bad seek call fd=%d pos=%lld name=%s\n",fd,pos,Filename))
  return false;
}

bool cStream::Stream(unsigned char * &data, unsigned long &len, const unsigned char *rest)
{
  if(fd>=0) {
    if(readpos<Filesize) {
      if(ismmap) {
        if(rest && fill) readpos=(rest-buffer);   // take care of remaining data
        fill=Filesize-readpos;
        data=buffer+readpos; len=fill;
        buffpos=readpos; readpos+=fill;
        return true;
        }
      else {
        if(rest && fill) {       // copy remaining data to start of buffer
          fill-=(rest-buffer);   // remaing bytes
          memmove(buffer,rest,fill);
          }
        else fill=0;

        int r;
        do { 
          r=read(fd,buffer+fill,MP3FILE_BUFSIZE-fill);
          } while(r==-1 && errno==EINTR);

        if(r>=0) {
          buffpos=readpos-fill; readpos+=r; fill+=r;
          data=buffer; len=fill;
          return true;
          }
        else { esyslog("ERROR: read failed in %s: %d,%s",Filename,errno,strerror(errno)); }
        }
      }
    else {
      len=0;
      return true;
      }
    }
  return false;
}

// -----------------------------------------------------------------------------

#ifdef DUMP_HEAD
void Dump(const char *name, char *buffer)
{
  FILE *f=fopen(name,"a");
  if(f) {
    fprintf(f,"<<<< %s\n",buffer);
/*
    int n=strlen(buffer);
    for(int i=0 ; i<n ; i+=8) {
      fprintf(f,"%04x: ",i);
      for(int l=0 ; l<8 && i+l<n ; l++) fprintf(f,"%02x ",buffer[i+l]);
      fprintf(f,"\n");
      }
*/
    fclose(f);
    }
}
#endif

// --- cNetStream -----------------------------------------------------------------

cNetStream::cNetStream(const char *Filename)
:cStream(Filename)
{
  net=0; host=path=0; cc=0;
  icyName=icyUrl=icyTitle=0; icyChanged=false;
  metaInt=0;
  InfoDone();
}

cNetStream::~cNetStream()
{
  free(host); free(path);
  free(icyName); free(icyUrl); free(icyTitle);
}

bool cNetStream::ParseURL(const char *line, bool log)
{
  char pr[32], h[512], p[512];
  int r=sscanf(line," %31[^:]://%511[^/]%511[^\r\n]",pr,h,p);
  if(r==2) {
    d(printf("netstream: adding default path '/'\n"))
    strcpy(p,"/");
    r++;
    }
  if(r==3) {
    d(printf("netstream: parsed proto='%s' host='%s' path='%s'\n",pr,h,p))
    if(!strcasecmp(pr,"http")) {
      int pp=DEFAULT_PORT;
      char *s=rindex(h,':');
      if(s) { *s++=0; pp=atoi(s); }

      free(host); host=strdup(h);
      free(path); path=strdup(p);
      port=pp;
      return true;
      }
    else if(log) esyslog("Unsupported protocol %s in: %s",pr,line);
    }
  else if(log) esyslog("Bad URL line: %s",line);
  return false;    
}

bool cNetStream::ParseURLFile(const char *name, bool log)
{
  bool res=false;
  FILE *f=fopen(name,"r");
  if(f) {
    char line[2048];
    if(fgets(line,sizeof(line),f)) {
      res=ParseURL(line,log);
      }
    else if(log) esyslog("Nothing to read from URL file %s. File empty?",name);
    fclose(f);
    }
  else if(log) esyslog("fopen() failed on URL file %s: %s",name,strerror(errno));
  return res;
}

bool cNetStream::SendRequest(void)
{
  bool res=false;
  char buff[2048];

  char *h, *p;
  asprintf(&h,port!=DEFAULT_PORT ? "%s:%d":"%s",host,port);
  if(MP3Setup.UseProxy) asprintf(&p,"http://%s%s",h,path);
  else asprintf(&p,"%s",path);

  snprintf(buff,sizeof(buff),
           "GET %s HTTP/1.0\r\n"
           "User-Agent: %s/%s\r\n"
           "Host: %s\r\n"
           "Accept: audio/mpeg\r\n"   //XXX audio/x-mpegurl, */*
           "Icy-MetaData: 1\r\n"
           "\r\n",
           p,PLUGIN_NAME,PLUGIN_VERSION,h);
  free(p); free(h);  

  if(++cc==1) asyncStatus.Set(tr("Connecting to stream server ..."));

  if(net->Connect(MP3Setup.UseProxy ? MP3Setup.ProxyHost:host , MP3Setup.UseProxy ? MP3Setup.ProxyPort:port)) {
    d(printf("netstream: -> %s",buff))
    if(net->Puts(buff)>0) res=GetHTTPResponse();
    }

  if(cc--==1) asyncStatus.Set(0);
  return res;
}

bool cNetStream::ParseHeader(const char *buff, const char *name, char **value)
{
  char *s=index(buff,':');
  if(s && !strncasecmp(buff,name,s-buff)) {
    s=skipspace(s+1);
    d(printf("netstream: found header '%s' contents '%s'\n",name,s))
    free(*value); *value=strdup(s);
    return true;
    }
  return false;
}

bool cNetStream::GetHTTPResponse(void)
{
  bool res=false;
  char buff[1024], text[128], *newurl=0;
  int code=-1, hcount=0;
  while(net->Gets(buff,sizeof(buff))>0) {
    stripspace(buff);
#ifdef DUMP_HEAD
    Dump(DUMP_HEAD,buff);
#endif
    d(printf("netstream: <- %s\n",buff))
    hcount++;
    if(hcount==1) {   // parse status line
      if(sscanf(buff,"%*[^ ] %d %128s",&code,text)!=2) {
        esyslog("Bad HTTP response '%s' from %s:%d",buff,host,port);
        goto out;
        }
      }
    else {            // parse header lines
      if(buff[0]==0) { // headers finish if we receive a empty line
        switch(code) {
          case 200: // OK
             res=true;
             goto out;
          case 300: // MULTIPLE_CHOICES
          case 301: // MOVED_PERMANENTLY
          case 302: // MOVED_TEMPORARILY
             if(newurl) {
               if(ParseURL(newurl,true)) res=SendRequest();
               }
             else esyslog("No location header for redirection from %s:%d",host,port);
             goto out;
          default:
             esyslog("Unhandled HTTP response '%d %s' from %s:%d",code,text,host,port);
             goto out;
          }
        }

      ParseHeader(buff,"Location",&newurl);
      ParseHeader(buff,"icy-name",&icyName);
      ParseHeader(buff,"icy-url",&icyUrl);
      char *meta=0;
      if(ParseHeader(buff,"icy-metaint",&meta)) {
        metaInt=metaCnt=atol(meta);
        d(printf("netstream: meta interval set to %d\n",metaInt));
        }
      free(meta);
      }
    }
out:
  free(newurl);
  return res;
}

bool cNetStream::Open(bool log)
{
  if(net && net->Connected()) return true;

  if(!net) net=new cNet(0,0,0);
  net->Disconnect();
  
  if(ParseURLFile(Filename,log)) {
    buffpos=readpos=0; fill=0;
    buffer = new unsigned char[MP3FILE_BUFSIZE];
    if(buffer) {
      if(SendRequest()) {
        return true;
        }
      }
    else esyslog("Not enough memory for buffer");
    }

  Close();
  return false;
}

void cNetStream::Close(void)
{
  delete buffer; buffer=0;
  delete net; net=0;
}

bool cNetStream::Seek(unsigned long long pos)
{
  return false;
}

bool cNetStream::Stream(unsigned char * &data, unsigned long &len, const unsigned char *rest)
{
  if(net && net->Connected()) {
    if(rest && fill) {       // copy remaining data to start of buffer
      fill-=(rest-buffer);   // remaing bytes
      memmove(buffer,rest,fill);
      }
    else fill=0;

    int r=MP3FILE_BUFSIZE-fill;
    if(metaInt && r>metaCnt) r=metaCnt;
    r=net->Read(buffer+fill,r);
    if(r>=0) {
      fill+=r; data=buffer; len=fill;
      metaCnt-=r;
      if(metaInt && metaCnt<=0) {
        ParseMetaData();
        metaCnt=metaInt;
        }
      return true;
      }
    }
  return false;
}

char *cNetStream::ParseMetaString(const char *buff, const char *name, char **value)
{
  char *s=index(buff,'=');
  if(s && !strncasecmp(buff,name,s-buff)) {
    char *end=index(s+2,'\'');
    if(s[1]=='\'' && end) {
      *end=0;
      s=stripspace(skipspace(s+2));
      if(strlen(s)>0) {
        d(printf("netstream: found metadata '%s' contents '%s'\n",name,s))
        free(*value); *value=strdup(s);
        }
      //else d(printf("netstream: found empty metadata '%s'\n",name))
      return end+1;
      }
    else d(printf("netstream: bad metadata format\n"))
    }
  return 0;
}

bool cNetStream::ParseMetaData(void)
{
  unsigned char byte;
  int r=net->Read(&byte,1);
  if(r<=0) return false;
  int metalen=byte*16;
  if(metalen>0) {
    char data[metalen+1];
    data[metalen]=0;
    int cnt=0;
    do {
      r=net->Read((unsigned char *)data+cnt,metalen-cnt);
      if(r<=0) return false;
      cnt+=r;
      } while(cnt<metalen);

#ifdef DUMP_HEAD
    Dump(DUMP_HEAD,data);
#endif

    char *p=data;
    while(*p && p-data<metalen) {
      char *n;
      if((n=ParseMetaString(p,"StreamTitle",&icyTitle)) ||
         (n=ParseMetaString(p,"StreamUrl",&icyUrl))) {
        p=n;
        icyChanged=true;
        }
      else  p++;
      }
    }
  return true;
}

bool cNetStream::IcyChanged(void)
{
  bool c=icyChanged;
  icyChanged=false;
  return c;
}
