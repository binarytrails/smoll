# Copyright (C) 2015 Vsevolod Ivanov
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>

"Python File Expansion Tracking Module"

import Queue, traceback

class Downotify(object):
    
    _fileSize = Queue.Queue()
    
    def __init__(self, fileToWatch, queueToNotify):
        self._toWatch = fileToWatch
        self._toNotify = queueToNotify

    def __updateProgress(self, bytesReceived, fileSize):
        percentage = float(bytesReceived) / float(fileSize) * 100
        self.toNotify.put(percentage)

    def _fileSizeListener(self):
        while True:
            try:
                pass

            except Queue.Empty:
                pass

if __name__ == "__main__":

    progressQueue = Queue.Queue()
    Downotify("some.mp3", progressQueue)

    # do some download
