#!/usr/bin/env sh

export MAGICK_DEBUG=All

#convert -gravity center -size 50x40 label:"Watch\nthe\nPidgeon" ~/message.gif
convert -gravity center -size 384x640 label:"Watch\nthe\nPidgeon" ~/message.gif

echo ===========================================================================
identify ~/message.gif

echo ===========================================================================
composite ~/message.gif ~/rose.jpg -stegano +15+2 ~/rose_message.png

echo ===========================================================================
#convert -size 50x40+15+2 stegano:$HOME/rose_message.png ~/message_recovered.gif
convert -size $((384/1))x$((640/1))+15 stegano:$HOME/rose_message.png ~/message_recovered.gif && xdg-open ~/message_recovered.gif
