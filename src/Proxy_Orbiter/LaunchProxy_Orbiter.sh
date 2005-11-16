#!/bin/bash

export SDL_VIDEO_X11_WMCLASS="Proxy_Orbiter"
export SDL_VIDEODRIVER="dummy"
exec ./Proxy_Orbiter "$@"

