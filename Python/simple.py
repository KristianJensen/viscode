from __future__ import print_function

import pandas as pd
import serial
import sys
import argparse
import time
import glob
import os

# Argument parsing
parser = argparse.ArgumentParser(description='Get data from the device')
parser.add_argument('port', type=str,
                    help='The serial port the device is on')
parser.add_argument('--outname', default="output", type=str
                    help='The name of the output files (.tsv and .log)')

args = parser.parse_args()

# From http://stackoverflow.com/questions/12090503/listing-available-com-ports-with-python
def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


# Try to open the connection
try:
    conn = serial.Serial(args.port, 9600, timeout=0)
except (OSError, serial.SerialException):
    raise ValueError(
        "Invalid port. Available ports are: " + str(serial_ports)
    )


# Open the output files
resfile = open(args.outname + ".tsv")
logfile = open(args.outname + ".log")


try:
    while True: # Infinite loop
        msg = conn.readline() # Get a message from the device
        if msg: # Parse the message
            typ, msg = msg.split(" ", 1)
            if typ == "DEB":
                print(msg.strip(), file=logfile, flush=False)
            elif typ == "RES":
                nums = msg.strip().split(" ")
                sense = nums[0]
                timestamp = nums[1]
                readings = nums[2:]
                print(timestamp, sense, *readings, sep="\t", file=resfile, flush=True)
        else: # No message - wait a bit
            time.sleep(1)

finally:
    # Remember to close everything
    resfile.close()
    logfile.close()
    conn.close()
