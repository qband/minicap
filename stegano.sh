#!/usr/bin/env sh

convert -gravity center -size 50x40 label:"Watch\nthe\nPidgeon" ~/message.gif
identify ~/message.gif
composite ~/message.gif ~/rose.jpg -stegano +15+2 ~/rose_message.png
convert -size 50x40+15+2 stegano:$HOME/rose_message.png ~/message_recovered.gif
