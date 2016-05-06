
# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np
import requests
 
# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (320, 240)
camera.framerate = 30
camera.hflip = True

rawCapture = PiRGBArray(camera, size=(320, 240))
 
# allow the camera to warmup
time.sleep(0.1)

cxPrev = 0
cyPrev = 0

start_time = time.time()
frames = 0
 
# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
	# grab the raw NumPy array representing the image, then initialize the timestamp
	# and occupied/unoccupied text
        image = frame.array

        blur = cv2.blur(image, (3,3))

        #hsv to complicate things, or stick with BGR
        #hsv = cv2.cvtColor(blur,cv2.COLOR_BGR2HSV)
        #thresh = cv2.inRange(hsv,np.array((0, 200, 200)), np.array((20, 255, 255)))

        #lower = np.array([76,31,4],dtype="uint8")
        ##upper = np.array([225,88,50], dtype="uint8")
        #upper = np.array([210,90,70], dtype="uint8")

        lower = np.array([0,70,70],dtype="uint8")
        upper = np.array([20,255,255], dtype="uint8")

        thresh = cv2.inRange(blur, lower, upper)
        thresh2 = thresh.copy()

        # find contours in the threshold image
        image, contours,hierarchy = cv2.findContours(thresh,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)

        # finding contour with maximum area and store it as best_cnt
        max_area = 0
        best_cnt = 1
        for cnt in contours:
                area = cv2.contourArea(cnt)
                if area > max_area:
                        max_area = area
                        best_cnt = cnt

        # finding centroids of best_cnt and draw a circle there
        M = cv2.moments(best_cnt)
        cx,cy = int(M['m10']/M['m00']), int(M['m01']/M['m00'])
        #if best_cnt>1:
        cv2.circle(blur,(cx,cy),10,(0,0,255),-1)
        # show the frame
        cv2.imshow("Frame", blur)
        #cv2.imshow('thresh',thresh2)
        #print(best_cnt, max_area)
        if (cx != cxPrev or cy != cyPrev) and cx != 0 and cy != 0 :
                cxPrev = cx
                cyPrev = cy
                print(cx, cy)

                payload="{} {} 25 25 1".format(cx, cy)
                connectTimeout = 0.1
                readTimeout = 0.1
                #serverUrl='http://172.16.1.201:9097'
                serverUrl='http://172.16.1.175:9097'
                
                try:
                        r = requests.post(serverUrl,
                                          data=payload,
                                          timeout=(connectTimeout,readTimeout))
                except requests.exceptions.ConnectTimeout as exc:
                        print("connection timeout")
                except requests.exceptions.ReadTimeout as exc:
                        print("read timeout")

        #print("response:", r.text)
        
        key = cv2.waitKey(1) & 0xFF
 
	# clear the stream in preparation for the next frame
        rawCapture.truncate(0)
 
	# if the `q` key was pressed, break from the loop
        if key == ord("q"):
        	break

        # Apparently not supported for my cameras:
        #print ("FPS:", camera.GetCaptureProperty(cap, cv2.CV_CAP_PROP_FPS))
        # print "Frame", frames
        frames = frames + 1
        if frames % 100 == 0 :
                currtime = time.time()
                numsecs = currtime - start_time
                fps = frames / numsecs
                print ("average FPS: ", fps)
