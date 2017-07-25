#!/usr/bin/python3

from time import sleep
from threading import Thread, Semaphore

s = Semaphore(1)

def task1(s):
    # s.acquire()
    with s:
        sleep(2)
        print('first')
    # s.release()

def task2(s):
    with s:
        print('second')

Thread(target=task1, args=(s,)).start()
task2(s)

print('finished!')
