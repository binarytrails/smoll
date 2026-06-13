#!/bin/bash

# 1. Check if a file was actually passed as an argument
if [ -z "$1" ]; then
    echo "Usage: $0 input_file.mkv"
    exit 1
fi

# 2. Get the input file name
INPUT="$1"

# 3. Strip the extension and append .mp4 for the output name
OUTPUT="${INPUT%.*}.mp4"

# 4. Run ffmpeg
echo 'Convert all with: for file in *.mkv; do ./mkv-2-mp4.sh "$file"; done'
ffmpeg -i "$INPUT" -c:v copy -c:a aac -b:a 192k "$OUTPUT"
