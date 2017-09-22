#!/usr/bin/python3

#! @author Vsevolod Ivanov seva@tumahn.net

import time
import timeout
import unittest

class TimeoutTests(unittest.TestCase):

    def test_with_run_stop(self):
        with timeout.run(1) as run:
            self.assertEqual(run.active(), True)
            run.stop()
            self.assertEqual(run.active(), False)

    def test_with_run_expire(self):
        with timeout.run(0.1) as run:
            while (run.active()):
                self.assertEqual(run.active(), True)
                time.sleep(1)
            self.assertEqual(run.expired(), True)

    def test_wait_expire(self):
        idle = timeout.Timeout()
        self.assertEqual(idle.active, False)
        self.assertEqual(idle.expired, False)
        idle.wait(0.1)
        self.assertEqual(idle.active, False)
        self.assertEqual(idle.expired, True)

    def test_wait_expire_raise(self):
        idle = timeout.Timeout(raise_e=True)
        secs = 0.1
        try:
            idle.wait(secs)
        except Exception as e:
            self.assertEqual(type(e),
                             timeout.TimeLimitExpired)
            self.assertEqual(list(e.args)[0], secs)

if __name__ == '__main__':
    unittest.main()

