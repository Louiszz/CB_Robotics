# Gray shape detection
# Compartmentalized

import numpy as np
import cv2

def readShape(cvimg):
    rsimg = cv2.resize(cvimg, (400, 400))
    imgcolor = rsimg.copy()
    img = cv2.cvtColor(rsimg, cv2.COLOR_BGR2GRAY)

    imgn = cv2.GaussianBlur(img, (7, 7), 0)

    #thresh = cv2.bilateralFilter(imgn, 5, 20, 20)
    thresh = cv2.adaptiveThreshold(imgn, 200, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                         cv2.THRESH_BINARY, 251, 1)
    #_, thresh = cv2.threshold(imgn, 150, 255, cv2.THRESH_BINARY)

    #inv = cv2.bitwise_not(thresh)
    
    _, contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    newConts = []
    selShapes = []
    selConts = imgcolor.copy()
    for n in range(1, len(contours)):
        cn = contours[n]
        a = cv2.contourArea(cn)
        if ((a < 100) or (a > 10000)):
            continue
        
        pm = cv2.arcLength(cn, True)
        poly = cv2.approxPolyDP(cn, 0.04 * pm, True)
        
        dims = cv2.minAreaRect(poly)
        rat = dims[1][0] / dims[1][1]
        if ((rat > 3) or (rat < 0.33)):
            continue
        
        if (len(poly) == 3):
            #print("Triangle!")
            selConts = cv2.drawContours(selConts, [cn], -1, (0, 0, 255), 2)
        elif (len(poly) == 4):
            #print("Rectangle!")
            selConts = cv2.drawContours(selConts, [cn], -1, (255, 140, 0), 2)
        elif (len(poly) < 6):
            #print("Circle!")
            selConts = cv2.drawContours(selConts, [cn], -1, (0, 255, 0), 2)

        newConts.append(poly)
        
    cv2.imwrite("cvPictures/selConts.jpg", selConts)

    return newConts

if __name__ =="__main__":
    testimg = cv2.imread("CamResult1.jpg", 1)
    res = readShape(testimg)

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
