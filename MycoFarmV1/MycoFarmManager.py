#!/usr/bin/env python3
# https://able.bio/rhett/how-to-set-and-get-environment-variables-in-python--274rgt5
# https://github.com/adafruit/Adafruit_IO_Python/blob/master/examples/api/data.py

import os
import serial
from Adafruit_IO import Client, Feed, Data, RequestError
from decouple import config
import datetime

if __name__ == '__main__':
    ADAFRUIT_IO_KEY = config('AIOKEY')
    ADAFRUIT_IO_USERNAME = config('AIOUSER')
    aio = Client(ADAFRUIT_IO_USERNAME,ADAFRUIT_IO_KEY)
    try:
        mtemp = aio.feeds('mtemp')
    except RequestError:
        tfeed = Feed(name="mtemp")
        mtemp = aio.create_feed(tfeed)
    try:
        mhumi = aio.feeds('mhumi')
    except RequestError:
        hfeed = Feed(name="mhumi")
        mhumi = aio.create_feed(hfeed)

    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.flush()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)
            aio.append(mtemp.key,24)
            aio.append(mhumi.key,75)
