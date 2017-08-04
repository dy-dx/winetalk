#!/bin/sh
set -e

# scripts/build-with-docker-winegcc.sh
scripts/build-with-dockcross-mingw32.sh

docker build -f Dockerfile -t winetalk .

docker run --rm --net=none winetalk "hello world hello world" \
  | play -b 16 -e signed-integer -L -r 11025 -c 1 -t raw -
