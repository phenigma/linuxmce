//
// C++ Interface: XinePlayer
//
// Description:	Xine player that loops on a thread. On close it destroys the 
// thread.
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef XinePlayer_H_
#define XinePlayer_H_

#include <xine.h>
#include <xine/xineutils.h>

#include <pthread.h>

#include <string>

class XinePlayer
{
	xine_t              *xine;
	xine_stream_t       *stream;
	xine_audio_port_t   *ao_port;
	xine_event_queue_t  *event_queue;
	pthread_t tid;
	std::string FileName;
public:
	bool NeedToReplay;
	bool Running;

	XinePlayer();
	~XinePlayer();

	void InitPlayerEngine(std::string ConfigName, std::string FileName);
	void StopPlayerEngine();

	bool StartPlayingFile();

	std::string GetFileName();
	void Replay();
};

void XineEventFunction(void *XinePlayerPtr, const xine_event_t *XineEvent);
void* XinePlayerThread(void* XinePlayerPtr);

#endif
