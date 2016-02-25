#!/usr/bin/env sh

export MAGICK_DEBUG=All

convert -gravity center -size 50x40 label:"Watch\nthe\nPidgeon" ~/message.gif

echo ===========================================================================
identify ~/message.gif

echo ===========================================================================
composite ~/message.gif ~/rose.jpg -stegano +15+2 ~/rose_message.png

echo ===========================================================================
convert -size 50x40+15+2 stegano:$HOME/rose_message.png ~/message_recovered.gif

