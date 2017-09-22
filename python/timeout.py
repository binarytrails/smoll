#!/usr/bin/python3

#! @author Vsevolod Ivanov seva@tumahn.net

import threading

class TimeLimitExpired(Exception):
    pass

class Timeout:

    active = False
    expired = False

    def __init__(self, raise_e=False):
        self.raise_e = raise_e

    def wait(self, secs):
        self._event = threading.Event()
        self.active = True

        success = self._event.wait(secs)
        if (not success and self.raise_e):
            raise TimeLimitExpired(secs)

        self.expired = (success is False)
        self.active = False

    def stop(self):
        if hasattr(self, '_event'):
            self._event.set()

class TimeoutThread(threading.Thread):

    def __init__(self, secs):
        self.secs = secs
        self.timeout = Timeout()
        threading.Thread.__init__(self)

    def run(self):
        self.timeout.wait(self.secs)

    def stop(self):
        self.timeout.stop()

    def active(self):
        return self.timeout.active

    def expired(self):
        return self.timeout.expired()

class run:
    """With statement using a background Thread"""

    def __init__(self, secs):
        self.timeout = TimeoutThread(secs)

    def __enter__(self):
        self.timeout.start()
        return self.timeout

    def __exit__(self, type, value, traceback):
        self.timeout.stop()
        return True

