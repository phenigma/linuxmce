#!/bin/bash

alsaconf
amixer sset Master 74% unmute || /bin/true
amixer sset PCM 74% unmute || /bin/true
