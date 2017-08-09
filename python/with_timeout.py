#!/usr/bin/python3

#! @author Vsevolod Ivanov seva@tumahn.net

import time
import threading

class TimeLimitExpired(Exception):
    pass

class Timeout(threading.Thread):

    _running = False

    def __init__(self, seconds, raise_e=False):
        self.seconds = seconds
        self.raise_e = raise_e

        threading.Thread.__init__(self)

    def run(self):
        start_time = time.time()
        self._running = True

        while (self._running):
            time_diff = time.time() - start_time

            if (time_diff > self.seconds):
                break

        self.stop()

    def stop(self):
        self._running = False

        if (self.raise_e):
            raise TimeLimitExpired('after {} seconds.'.format(self.seconds))

    def active(self):
        return self._running

class run_timeout:

    def __init__(self, seconds, raise_e=True):
        self.seconds = seconds
        self.raise_e = raise_e

    def __enter__(self):
        self.timeout = Timeout(self.seconds, self.raise_e)
        self.timeout.start()
        return self.timeout

    def __exit__(self, type, value, traceback):
        return True

with run_timeout(0.1) as run:

    while (run.active()):
        print('Running..')

