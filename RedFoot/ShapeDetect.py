# Gray shape detection

#from picamera import PiCamera
#import time
import numpy as np
import cv2
print("Imported stuff.")

delay = 4

PATH = "/home/pi/Documents/GreenArm2018/"

img = cv2.imread(PATH + "CamResult1.jpg", 1)
img = cv2.resize(img, (400, 400))
imgcolor = img.copy()
img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

for i in range(1):
    imgn = cv2.GaussianBlur(img, (7, 7), 0)

    #thresh = cv2.bilateralFilter(imgn, 5, 20, 20)
    thresh = cv2.adaptiveThreshold(imgn, 200, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                         cv2.THRESH_BINARY, 251, 1)
    #_, thresh = cv2.threshold(imgn, 150, 255, cv2.THRESH_BINARY)
    cv2.imwrite("cvPictures/img1.png", thresh)
    #continue
    #inv = cv2.bitwise_not(thresh)
    
    _, contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    tt = imgn.copy()
    tt = cv2.drawContours(tt, contours, -1, (0,255,0), 3)
    cv2.imwrite("cvPictures/allcs1.png", tt)

    newConts = []
    for n in range(1, len(contours)):
        cn = contours[n]
        a = cv2.contourArea(cn)
        if ((a < 100) or (a > 10000)):
            continue
        #cv2.imwrite("cvPictures/A"+str(n)+".png",
        #            cv2.drawContours(imgcolor.copy(), [cn], -1, (0, 255, 0), 3))
        pm = cv2.arcLength(cn, True)
        poly = cv2.approxPolyDP(cn, 0.04 * pm, True)
        #print("n:", n, ", poly:", poly)
        dims = cv2.minAreaRect(poly)
        
        #print("dims:", dims)
        rat = dims[1][0] / dims[1][1]
        if ((rat > 3) or (rat < 0.33)):
            continue
        #testconts = cv2.drawContours(imgcolor.copy(), [cn], -1, (0, 0, 255), 3))
        if (len(poly) == 3):
            print("Triangle!")
            cv2.imwrite("cvPictures/T"+str(n)+".png",
                        cv2.drawContours(imgcolor.copy(), [cn], -1, (0, 0, 255), 3))
        elif (len(poly) == 4):
            print("Rectangle!")
            cv2.imwrite("cvPictures/R"+str(n)+".png",
                        cv2.drawContours(imgcolor.copy(), [cn], -1, (0, 255, 0), 3))
        elif (len(poly) < 6):
            print("Circle!")
            cv2.imwrite("cvPictures/C"+str(n)+".png",
                        cv2.drawContours(imgcolor.copy(), [cn], -1, (0, 180, 220), 3))

"""
cam  = PiCamera()
cam.resolution = [900, 900] #cam.MAX_RESOLUTION [1296, 972]
cam.framerate = 10
try:
    cam.start_preview(resolution=[900, 900])
    for i in range(10):
        time.sleep(delay)
        cam.capture(PATH + "/T%s.jpg"%(i + 7))
    time.sleep(delay)
except:
    pass
finally:
    cam.stop_preview()
    cam.close()
"""
