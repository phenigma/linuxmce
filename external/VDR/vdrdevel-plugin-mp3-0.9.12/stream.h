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

#ifndef ___STREAM_H
#define ___STREAM_H

#include "decoder.h"

class cNet;

// ----------------------------------------------------------------

#if 0
class cIO : public cFileInfo {
protected:
  bool log;
  unsigned long long readpos;
public:
  cIO(const char *Filename, bool Log);
  virtual ~cIO();
  virtual bool Open(void)=0;
  virtual void Close(void)=0;
  virtual int Read(unsigned char *Data, int Size)=0;
  virtual unsigned char *StreamInit(int Size) { return 0; }
  virtual int Stream(const unsigned char *rest) { return -1; }
  virtual bool Seek(unsigned long long pos, int whence) { return -1; }
  virtual unsigned long long Tell(void) { return readpos; }
  };

// ----------------------------------------------------------------

class cStreamIO {
private:
  int size, fill;
  unsigned char *data;
protected:
  void StreamClear(bool all);
public:
  cStreamIO(void);
  virtual ~cStreamIO();
  virtual unsigned char *StreamInit(int Size);
  virtual int Stream(const unsigned char *rest);
  virtual int Read(unsigned char *Data, int Size)=0;
  };

// ----------------------------------------------------------------

class cFileIO : public cIO, public cStreamIO {
protected:
  int fd;
public:
  cFileIO(const char *Filename, bool Log);
  virtual ~cFileIO();
  virtual bool Open(void);
  virtual void Close(void);
  virtual int Read(unsigned char *Data, int Size);
  virtual bool Seek(unsigned long long pos, int whence);
  };
#endif

// ----------------------------------------------------------------

class cStream : public cFileInfo {
private:
  int fd;
  bool ismmap;
protected:
  unsigned char *buffer;
  unsigned long long readpos, buffpos;
  unsigned long fill;
public:
  cStream(const char *Filename);
  virtual ~cStream();
  virtual bool Open(bool log=true);
  virtual void Close(void);
  virtual bool Stream(unsigned char *&data, unsigned long &len, const unsigned char *rest=NULL);
  virtual bool Seek(unsigned long long pos=0);
  virtual unsigned long long BufferPos(void) { return buffpos; }
  };

// ----------------------------------------------------------------

class cNetStream : public cStream {
private:
  cNet *net;
  char *host, *path;
  int port, cc;
  //
  char *icyName, *icyUrl, *icyTitle;
  int metaInt, metaCnt;
  bool icyChanged;
  //
  bool ParseURL(const char *line, bool log);
  bool ParseURLFile(const char *name, bool log);
  bool SendRequest(void);
  bool GetHTTPResponse(void);
  bool ParseHeader(const char *buff, const char *name, char **value);
  bool ParseMetaData(void);
  char *ParseMetaString(const char *buff, const char *name, char **value);
public:
  cNetStream(const char *Filename);
  virtual ~cNetStream();
  virtual bool Open(bool log=true);
  virtual void Close(void);
  virtual bool Stream(unsigned char *&data, unsigned long &len, const unsigned char *rest=NULL);
  virtual bool Seek(unsigned long long pos=0);
  bool Valid(void) { return ParseURLFile(Filename,false); }
  const char *IcyName(void) const { return icyName; }
  const char *IcyUrl(void) const { return icyUrl; }
  const char *IcyTitle(void) const { return icyTitle; }
  bool IcyChanged(void);
  };

#endif //___STREAM_H
