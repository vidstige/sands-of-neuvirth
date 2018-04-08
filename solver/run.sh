#!/bin/bash
set -e
make
./fluid | ffplay -v warning -f rawvideo -pixel_format rgb24 -video_size 320x200 -i -
