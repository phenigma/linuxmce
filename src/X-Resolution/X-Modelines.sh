#!/bin/bash

# Hardcoded^W Predefined modelines, found on the web: http://forums.entechtaiwan.net/viewtopic.php?t=3730
# Things to notice:
# - the dot clock value is fixed, regardless of refresh, and the other timings are elongated to compensate
# - none of the modeline generation utilities out there are able to generate these for me
# - no modeline utility is able to fix both the refresh rate and the dot clock and adjust the timings to compensate
# - these don't seem to comply to VESA GTF in any way

Modeline_1280x720_50='"1280x720" 74.250 1280 1720 1760 1980 720 741 746 750 +hsync +vsync'
Modeline_1920x540_50='"1920x540" 74.250 1920 2448 2492 2640 540 574 579 562 +hsync +vsync'
Modeline_720x576_50='"720x576" 27.000 720 732 796 864 576 581 586 625 -hsync -vsync'
Modeline_1280x720_60='"1280x720" 74.250 1280 1390 1430 1650 720 725 730 750 +hsync +vsync'
#Modeline_1280x720_60='"1280x720" 74.160 1280 1352 1392 1648 720 725 730 750 -hsync -vsync' # HDTV derived
Modeline_1920x540_60='"1920x540" 74.250 1920 2008 2052 2200 540 542 547 562 +hsync +vsync'
Modeline_720x480_60='"720x480" 27.000 720 736 798 858 480 489 495 525 -hsync -vsync'
Modeline_720x576_60='"720x576" 27.000 720 732 796 864 576 581 586 625 -hsync -vsync'
Modeline_1920x1080i_50='"1920x1080" 74.250 1920 2448 2492 2640 1080 1148 1158 1124 interlace +hsync +vsync'
#Modeline_1920x1080i_60='"1920x1080" 74.250 1920 2008 2052 2200 1080 1084 1094 1124 interlace +hsync +vsync'
Modeline_1920x1080i_60='"1920x1080" 77.6 1920 1952 2240 2272 1080 1104 1110 1135 interlace'

# Driver bug workaround
Modeline_1024x768_60='"1024x768" 60.80 1024 1056 1128 1272 768 768 770 796'
