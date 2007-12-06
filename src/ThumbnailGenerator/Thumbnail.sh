#!/bin/bash

PK_Picture="$1"

#TODO: Detect what kind of thumbnail to generate
theme="cd"
theme="dvd"
theme="film"
theme="photos"
theme="vinyl"

# Call to generate thumbnail
#TODO: should use png, jpg doesn't have transparency
"/usr/share/thumbnail-generator/${theme}/generate_thumb.sh" "/home/medipics/${PK_Picture}.jpg" "/home/medipics/${PK_Picture}_tn.jpg"
