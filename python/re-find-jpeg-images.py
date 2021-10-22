#!/bin/python

import re
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', required=True, type=str)
    args = parser.parse_args()
    print("Loading image '{}' as binary".format(args.input))
    img = open(args.input, 'rb').read()
    print('Duplicating image into one huge BLOB')
    images = img+img
    # jpeg_start: ffd8, jpeg_end: ffd9
    jpeg_regex = re.compile(rb"\xff\xd8(.*?)\xff\xd9", re.DOTALL)
    counter = 0
    for match_obj in jpeg_regex.finditer(images):
        offset = match_obj.start()
        offset2 = match_obj.end()
        print("Image {} offsets: {} - {}".format(counter, offset, offset2))
        counter = counter + 1
