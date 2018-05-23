# Main code for Green Arm
# Closest Euclidean distance to center

from picamera import PiCamera
import time
import math
#from NeuralNet import *
#import ImgGrabber as IG
import ShapeDetectBase as SDB
import cv2
import serial
from serial.serialutil import SerialException
import sys
print("Imported!")

cam = PiCamera()
cam.resolution = [1296, 972] #cam.MAX_RESOLUTION [1296, 972]
go = True

try:
    conn = serial.Serial("/dev/ttyACM0", 9600, timeout=5)
    print("Using port /dev/ttyACM0")
except SerialException:
    try:
        conn = serial.Serial("/dev/ttyACM1", 9600, timeout=5)
        print("Using port /dev/ttyACM1")
    except SerialException:
        print("Unable to find serial port!")
        go = False


cc = 0
def getCamImage(delay=0):
    try:
        #cam.start_preview(resolution=[1296, 972])
        time.sleep(delay)
        cam.capture("CamResult%s.jpg"%cc)
    except KeyboardInterrupt:
        pass
    return cv2.imread("CamResult%s.jpg"%cc, 1)

def eucDist(x1, y1, x2, y2):
    return math.sqrt((x1-x2)**2 + (y1-y2)**2)

while go:
    val = conn.read()
    print(val)
    #val = sys.stdin.readline()[:-1]
    if (val == b'?'):
        img = getCamImage()
        shapes = SDB.readShape(img)
        center = (200,200)
        mindex = 0
        mdist = eucDist(*center, *shapes[0][1])
        for i in range(len(shapes)):
            shape = shapes[i]
            cdist = eucDist(*center, *shape[1])
            if (cdist < mdist):
                mdist = cdist
                mindex = i
        
        tshape = shapes[mindex][0]
        if (len(tshape) == 3):
            print("Red")
            conn.write(b"Red")
        elif (len(tshape) == 4):
            print("Blue")
            conn.write(b"Blue")
        else:
            print("Green")
            conn.write(b"Green")
        conn.write(b"!")
