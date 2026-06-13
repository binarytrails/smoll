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
# -genpts: Helps fix "invalid EBML" by regenerating timestamps
# -analyze_duration / -probesize: Digs deeper into the file to understand the broken structure
# -bsf:v h264_mp4toannexb (optional): Can sometimes help if the stream itself is the issue
ffmpeg -analyzeduration 100M -probesize 100M -fflags +genpts \
    -i "$INPUT" \
    -c:v copy \
    -c:a aac -b:a 192k \
    -movflags +faststart \
    "$OUTPUT"
