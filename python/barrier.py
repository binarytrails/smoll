#!/usr/bin/python3

from time import sleep
from threading import Thread, Barrier

# parties - 1 = parties_to_wait_for = 1
b = Barrier(2)

def task1(b):
    sleep(2)
    print('first')
    b.wait()
    print('first done')

def task2(b):
    b.wait()
    print('second')

Thread(target=task1, args=(b,)).start()
task2(b)

print('finished!')
