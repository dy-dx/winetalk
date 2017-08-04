#!/bin/sh

winegcc -m32 -mconsole -mno-cygwin main.c -o winetalk.sh \
  && WINEDEBUG=-all ./winetalk.sh "hello world hello world" \
    | play -b 16 -e signed-integer -L -r 11025 -c 1 -t raw -
