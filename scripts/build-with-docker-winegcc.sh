#!/bin/sh

docker build -f Dockerfile-winegcc -t winetalk-winegcc .
docker run --rm -v `pwd`:/tmp winetalk-winegcc winegcc -m32 -mconsole -mno-cygwin /tmp/main.c -o /tmp/winetalk.sh
