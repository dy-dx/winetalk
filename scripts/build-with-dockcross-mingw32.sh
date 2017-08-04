#!/bin/sh
set -e

image_tag="dockcross/windows-x86"
dockcross_path="scripts/dockcross"

if [ ! -f "$dockcross_path" ]; then
  docker pull "$image_tag"
  docker run --rm "$image_tag" > "$dockcross_path"
  chmod +x "$dockcross_path"
fi

"$dockcross_path" bash -c '$CC main.c -o talk.exe'
