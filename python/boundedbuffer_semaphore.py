#!/usr/bin/python3
# Vsevolod Ivanov (seva@binarytrails.net)
# Bounded buffer with Semaphore

from time import sleep
from random import randint
from threading import Thread, Semaphore

s = Semaphore(1)

producer_idx = 0
consumer_idx = 0
counter = 0

buf_size = 10
buf = [" "] * buf_size
print(buf)

def produce():
    global producer_idx, counter, buf, buf_size
    while True:
        #s.acquire()
        with s:
            if (counter == buf_size): # full
                #s.release()
                continue
            buf[producer_idx] = "x"
            producer_idx = (producer_idx + 1) % buf_size
            print("{} <= produced 'x' at index='{}'".format(buf, producer_idx))
            counter = counter + 1
        #s.release()
        sleep(1)

def consume():
    global consumer_idx, counter, buf, buf_size
    while True:
        #s.acquire()
        with s:
            if (counter == 0): # empty (next element is)
                #s.release()
                continue
            buf[consumer_idx] = " "
            consumer_idx = (consumer_idx + 1) % buf_size
            print("{} => consumed '{}' at index='{}'".format(buf, buf[consumer_idx], consumer_idx))
            counter = counter - 1
        #s.release()
        sleep(1)

producers = list()

for i in range(randint(10,20)):
    producer = Thread(target=produce)
    producer.start()
    producers.append(producer)

consumers = list()

for i in range(randint(10,20)):
    consumer = Thread(target=consume)
    consumer.start()
    consumers.append(consumer)
