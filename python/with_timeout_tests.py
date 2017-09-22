#!/usr/bin/python3

#! @author Vsevolod Ivanov seva@tumahn.net

import time
import unittest
from with_timeout import *

class TimeoutTests(unittest.TestCase):

    def test_with_run_stop(self):
        with run_timeout(1) as run:
            self.assertEqual(run.active(), True)
            run.stop()
            self.assertEqual(run.active(), False)

    def test_with_run_expire(self):
        with run_timeout(0.1) as run:
            while (run.active()):
                self.assertEqual(run.active(), True)
                time.sleep(1)
            self.assertEqual(run.expired(), True)

    def test_wait_expire(self):
        t = Timeout()
        self.assertEqual(t.running, False)
        self.assertEqual(t.expired, False)
        t.wait(0.1)
        self.assertEqual(t.running, False)
        self.assertEqual(t.expired, True)

    def test_wait_expire_raise(self):
        t = Timeout(raise_e=True)
        secs = 0.1
        try:
            t.wait(secs)
        except Exception as e:
            self.assertEqual(type(e), TimeLimitExpired)
            self.assertEqual(list(e.args)[0], secs)

if __name__ == '__main__':
    unittest.main()

