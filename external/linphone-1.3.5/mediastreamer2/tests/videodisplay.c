/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "msfilter.h"
#include "msvideo.h"
#include "msticker.h"
#include "msv4l.h"

int main(int argc, char *argv[]){
	MSTicker *ticker;
	MSFilter *grabber;
	MSFilter *display;
	MSPixFmt fmt;
	
	ms_init();
	ticker=ms_ticker_new();
	grabber=ms_filter_new(MS_V4L_ID);
	display=ms_filter_new(MS_SDL_OUT_ID);
	
	ms_filter_call_method_noarg(grabber,MS_V4L_START);
	ms_filter_call_method(grabber,MS_FILTER_GET_PIX_FMT,&fmt);
	ms_filter_call_method(display,MS_FILTER_SET_PIX_FMT,&fmt);
	
	ms_filter_link(grabber,0,display,0);
	ms_ticker_attach(ticker,grabber);
	sleep(5);
	ms_ticker_detach(ticker,grabber);
	
	ms_filter_call_method_noarg(grabber,MS_V4L_STOP);
	
	ms_filter_destroy(grabber);
	ms_filter_destroy(display);
	ms_ticker_destroy(ticker);
	return 0;
}
