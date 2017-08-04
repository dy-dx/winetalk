#!/bin/sh

docker build -f Dockerfile-winegcc -t winetalk-winegcc .
docker run --rm -v `pwd`:/tmp winetalk-winegcc winegcc /tmp/main.c -o /tmp/winetalk.sh
