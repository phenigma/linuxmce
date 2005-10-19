#!/bin/bash

export SDL_VIDEO_X11_WMCLASS="Bluetooth_Dongle"
export SDLDRIVER="dummy"
exec ./Bluetooth_Dongle "$@"
