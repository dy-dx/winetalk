#!/bin/sh
Xvfb :0 2> /dev/null &
# wine talk.exe "$1"
WINEDEBUG=-all wine talk.exe "$1" 2>/dev/null
