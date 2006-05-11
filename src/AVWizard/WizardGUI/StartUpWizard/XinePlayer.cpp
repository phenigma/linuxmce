//
// C++ Implementation: XinePlayer 
//
// Description: XinePlayer - a minimalist sound player
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "XinePlayer.h"

#include <stdio.h>

XinePlayer::XinePlayer()
	: xine(NULL),
	  stream(NULL),
	  ao_port(NULL),
	  event_queue(NULL)
{
	NeedToReplay = false;
	this->Running = false;
}

XinePlayer::~XinePlayer()
{
}

void XinePlayer::InitPlayerEngine(std::string ConfigName, std::string FileName)
{
	char ConfigFile[1024];

	xine = xine_new();
	sprintf(ConfigFile, "%s%s", xine_get_homedir(), ConfigName.c_str());
	xine_config_load(xine, ConfigFile);
	xine_init(xine);

	this->FileName = FileName;

	pthread_create(&tid,NULL, XinePlayerThread,this);
}

void XinePlayer::StopPlayerEngine()
{
	xine_close(stream);
	xine_event_dispose_queue(event_queue);
	xine_dispose(stream);
	if(ao_port)
		xine_close_audio_driver(xine, ao_port);  
	xine_exit(xine);
}

bool XinePlayer::StartPlayingFile()
{
	ao_port     = xine_open_audio_driver(xine , "auto", NULL);
	stream      = xine_stream_new(xine, ao_port, NULL);
	event_queue = xine_event_new_queue(stream);
	xine_event_create_listener_thread(event_queue, XineEventFunction, this);


	if((!xine_open(stream, FileName.c_str())) || (!xine_play(stream, 0, 0))) 
	{
		printf("Unable to open file: '%s'\n", FileName.c_str());
		return false;
	}

	return true;
}

void XinePlayer::Replay()
{
	this->NeedToReplay = true;
}

std::string XinePlayer::GetFileName()
{
	return FileName;
}

void XineEventFunction(void *XinePlayerPtr, const xine_event_t *XineEvent)
{
	XinePlayer* Player = (XinePlayer*)XinePlayerPtr;

	switch(XineEvent->type) { 
	case XINE_EVENT_UI_PLAYBACK_FINISHED:
		Player->Replay();
		break;
	}
}

void* XinePlayerThread(void* XinePlayerPtr)
{
	XinePlayer* Player = (XinePlayer*)XinePlayerPtr;

	Player->StartPlayingFile();
	while(Player->Running) 
	{
		usleep(100000);
		if (Player->NeedToReplay)
		{
			Player->StartPlayingFile();
		}
	}

	Player->StopPlayerEngine();
	return NULL;
}
