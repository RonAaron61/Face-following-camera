import cv2 as cv
import mediapipe as mp
import time

"""ARDUINO PART"""
from numpy import byte
import serial
arduino = serial.Serial(port='COM4', baudrate=9600, timeout=.1)

cap = cv.VideoCapture(1)    #Kamera
#cap = cv.VideoCapture('Stock/Human Dance 2.mp4')

cTime = pTime = 0

mpFaceDetection = mp.solutions.face_detection
mpDraw = mp.solutions.drawing_utils 
faceDetection = mpFaceDetection.FaceDetection(0.80) #Default confidence levelnya 50%, kalo mau di ubah kyk di bawah
#faceDetection = mpFaceDetection.FaceDetection(0.75)

while True:
    succes, img = cap.read()

    imgRGB = cv.cvtColor(img, cv.COLOR_BGR2RGB)
    results = faceDetection.process(imgRGB)

    if results.detections:  #Jika ada terdeteksi
        print(f'Number of Faces: {len(results.detections)}')    #Jumlah wajah
        for id, detection in enumerate(results.detections):
            if(id==0):  #Hanya mengikuti wajah dengan presentasi keyakinan tertinggi
                #print(id, detection)
                #print(detection.location_data.relative_bounding_box)    #Dapet dari hasil detecion
                #mpDraw.draw_detection(img, detection)  #Gambar boundin gbox dan lokasi mata hidung telinga
                bboxC = detection.location_data.relative_bounding_box
                h, w, c = img.shape
                bbox = int(bboxC.xmin * w), int(bboxC.ymin * h), int(bboxC.width * w), int(bboxC.height * h)    #Nyesuain sama ukuran ghambar biar koordinatnya segambar juga
                cv.rectangle(img, bbox, (255,0,255), 2) #Ngegambar boujndingbox nya aja manual pake OpenCv
                #cv.putText(img, f'{int(detection.score[0] * 100)} %', (bbox[0], bbox[1] - 10), cv.FONT_HERSHEY_PLAIN, 2, (0,255,0), 2)


                #sending coordinates to Arduino
                string='X{0:d}Y{1:d}'.format((bbox[0]+(bbox[2]//2)),(bbox[1]+(bbox[3]//2)))
                print(string)
                arduino.write(string.encode('utf-8'))

                #plot the center of the face
                cv.circle(img,((bbox[0]+(bbox[2]//2)),(bbox[1]+(bbox[3]//2))),2,(0,255,0),2)       

    #plot the squared region in the center of the screen
    cv.rectangle(img,(640//2-40,480//2-40),
                 (640//2+40,480//2+40),
                  (255,255,255),3)

    #FPS
    cTime = time.time()
    fps = 1/(cTime-pTime)
    pTime = cTime
    cv.putText(img, f'{str(int(fps))} FPS', (10,40), cv.FONT_HERSHEY_PLAIN, 2, (0,255,0), 2)

    #Show image video
    cv.imshow("Webcam",img)

    if cv.waitKey(20) & 0xFF==ord('d'): #jika tombol 'd' ditekan looping break
        break

cap.release()
cv.destroyAllWindows()  #Clode window kalo sudah selesai