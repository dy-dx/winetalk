#!/bin/sh
Xvfb :0 2> /dev/null &
# ./winetalk.sh "$1"
WINEDEBUG=-all ./winetalk.sh "$1" 2>/dev/null
