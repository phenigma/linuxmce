#!/bin/bash

export SDL_VIDEO_X11_WMCLASS="Photo_Screen_Saver"

Executable=./Photo_Screen_Saver

exec "$Executable" "$@"
