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
#include <string>
#include <pthread.h>

#include <iostream>

/*static*/ XinePlayer* XinePlayer::Instance = NULL;

XinePlayer::XinePlayer()
	: xine(NULL),
	  stream(NULL),
	  ao_port(NULL),
	  event_queue(NULL),
	  tid(0)
{
	std::cout<<"Starting player!"<<std::endl;
	NeedToReplay = false;
	this->Running = false;
	pthread_mutex_init(&lockmutex, NULL);
}

XinePlayer* XinePlayer::GetInstance()
{
	if (Instance == NULL)
		Instance = new XinePlayer();
	return Instance;
}

XinePlayer::~XinePlayer()
{
}

void XinePlayer::InitPlayerEngine(std::string ConfigName, std::string FileName)
{
	std::string ConfigFile;

	pthread_mutex_lock(&lockmutex);

	xine = xine_new();
	std::cout << FileName <<std::endl;
	ConfigFile = ConfigName;
	xine_config_load(xine, ConfigFile.c_str());
	xine_init(xine);
	this->FileName = FileName;


	pthread_mutex_unlock(&lockmutex);

	if(!StartPlayingFile())
		return;
	if(tid == 0)
		pthread_create(&tid, NULL, XinePlayerThread,this);
	
}

void XinePlayer::StopPlayerEngine()
{
	if(!xine)
		return;

	pthread_mutex_lock(&lockmutex);

	xine_close(stream);
	xine_event_dispose_queue(event_queue);
	xine_dispose(stream);
	if(ao_port)
		xine_close_audio_driver(xine, ao_port);  
	xine_exit(xine);
	xine = NULL;

	Running = false;
	pthread_mutex_unlock(&lockmutex);
}

bool XinePlayer::StartPlayingFile()
{
	pthread_mutex_lock(&lockmutex);
	ao_port     = xine_open_audio_driver(xine , "auto", NULL);
	stream      = xine_stream_new(xine, ao_port, NULL);
	event_queue = xine_event_new_queue(stream);
	xine_event_create_listener_thread(event_queue, XineEventFunction, this);

	if((!xine_open(stream, FileName.c_str())) || (!xine_play(stream, 0, 0))) 
	{
		printf("Unable to open file: '%s'\n", FileName.c_str());
		pthread_mutex_unlock(&lockmutex);

		return false;
	}

	std::cout<<"Playing..."<<std::endl;

	pthread_mutex_unlock(&lockmutex);

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

	while(Player->Running) 
	{
		usleep(10000);
		if (Player->NeedToReplay)
		{
			Player->StartPlayingFile();
		}
	}

	//Player->StopPlayerEngine();
	return NULL;
}
