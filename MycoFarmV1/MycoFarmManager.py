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
    try:
        mco2 = aio.feeds('mco2')
    except RequestError:
        cfeed = Feed(name="mco2")
        mco2 = aio.create_feed(cfeed)
    try:
        mvoc = aio.feeds('mvoc')
    except RequestError:
        vfeed = Feed(name="mvoc")
        mvoc = aio.create_feed(vfeed)

    ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
    ser.flush()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)
            pairs = line.split('|')
            for pair in pairs:
                # print(pair)
                if (pair.find('Temp:') > -1):
                    t = float(pair[5:])
                    # print(t)
                    aio.append(mtemp.key,t)
                if (pair.find('Humi:') > -1):
                    h = float(pair[5:])
                    # print(h)
                    aio.append(mhumi.key,h)
                if (pair.find('TVOC:') > -1):
                    v = float(pair[5:])
                    # print(h)
                    aio.append(mvoc.key,v)
                if (pair.find('CO2:') > -1):
                    c = float(pair[4:])
                    # print(h)
                    aio.append(mco2.key,c)
