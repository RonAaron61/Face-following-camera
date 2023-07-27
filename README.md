# Face-following-camera

Making a camera that follows person face, using a webcam as input video, python for detecting face and location, and Arduino to move the webcam. To move the camera
I'm using 2 servo motors for X and Y axis. When the camera not detecting any face it will scan the whole room, and if it finds a face the camera will follow
the person's face. For detecting a face I'm using Python with OpenCV and Mediapipe library because mediapipe already have a builtin function to detect face so
we don't have to make XML file containing features of a person's face. Then python program will send the coordinates to Arduino to move the servos.

Results: [Youtube](https://youtu.be/mI4-Z3ocw5Y?t=780)

## Components

- 2 Servo motor
- webcam
- Arduino
- Computer

## How it went

One motor servo is placed vertical and one horizontal, and on top of it is the webcam

![image](https://github.com/RonAaron61/Face-following-camera/assets/105662575/59721aa6-db0d-4bd8-b0fe-d5bf498b5253)

# Code

## Python Code

First we declare some variable and library

```
import cv2 as cv
import mediapipe as mp
import time

from numpy import byte
import serial
arduino = serial.Serial(port='COM4', baudrate=9600, timeout=.1)
```

the some variable for the open cv

```
cap = cv.VideoCapture(1)
```

Camera (0 -> your default camera (If you're using a laptop then it's your main webcam) because I Use an external webcam then I use 1   

```
cTime = pTime = 0  #For calculating FPS

mpFaceDetection = mp.solutions.face_detection  #Mediapipe for detecting person's face
mpDraw = mp.solutions.drawing_utils   #Mediapipe function for drawing on the screen
faceDetection = mpFaceDetection.FaceDetection(0.80)  #confidence level (80%)
```

Then we go to the main loop

```
succes, img = cap.read()  #Read the output of camera
imgRGB = cv.cvtColor(img, cv.COLOR_BGR2RGB)  #Change the color from BGR (OpenCV default color) to RGB
results = faceDetection.process(imgRGB)  #Detect face
```

then if there's is a face detected:

```
 if results.detections:  #If there's a face detected
        print(f'Number of Faces: {len(results.detections)}')    #If you want to know how many faces is detected
        for id, detection in enumerate(results.detections):  #Check for every face
            if(id==0):  #Only track the face with the highest confidence level
```

Then we can draw some boxes on the screen just to know where is the faces, but firs I change from the default coordinates (percent of the height and width of the 
video size to coordinates based on the image pixel

This is the example output when a face is detected

![image](https://github.com/RonAaron61/Face-following-camera/assets/105662575/b1c3c9c3-f540-4d4b-a6ed-196d1af9cade)

so,

![image](https://github.com/RonAaron61/Face-following-camera/assets/105662575/aeed988a-eea5-4ac9-8ab6-f20939984720)

```
bboxC = detection.location_data.relative_bounding_box
h, w, c = img.shape
bbox = int(bboxC.xmin * w), int(bboxC.ymin * h), int(bboxC.width * w), int(bboxC.height * h)    #Change coordinat to based on the image pixel
cv.rectangle(img, bbox, (255,0,255), 2) #Draw boundingbox with  OpenCv
```

After that we can send those coordinates to Arduino 

```
#sending coordinates to Arduino
string='X{0:d}Y{1:d}'.format((bbox[0]+(bbox[2]//2)),(bbox[1]+(bbox[3]//2)))  #send the middle coordinates
print(string)
arduino.write(string.encode('utf-8'))

#plot the center of the face
cv.circle(img,((bbox[0]+(bbox[2]//2)),(bbox[1]+(bbox[3]//2))),2,(0,255,0),2)   
```

Next is just to show some dot on the middle od the face and show the fps

```
 #plot the squared region in the center of the screen
cv.rectangle(img,(640//2-40,480//2-40),
             (640//2+40,480//2+40),
              (255,255,255),3)
#FPS
cTime = time.time()
fps = 1/(cTime-pTime)
pTime = cTime
cv.putText(img, f'{str(int(fps))} FPS', (10,40), cv.FONT_HERSHEY_PLAIN, 2, (0,255,0), 2)
```

Then lastly we can show the video on screen, and if I press 'd' on the keyboard the program will stop

```
#Show image video
cv.imshow("Webcam",img)

if cv.waitKey(20) & 0xFF==ord('d'): #If 'd' is pressed then lopping break
  break
```


## Arduino Code

Arduino Code

On the arduino code firstly whe declare some variable, including the width and height of the video resolution, then on the void setup ()
we initialize servos pin and start position which is in the middle. On the void loop()

```
void loop()
{
  if(Serial.available() > 0) // If face is detected then get the coordinates
  {
    if(Serial.read() == 'X')
    {
      x_mid = Serial.parseInt();
      if(Serial.read() == 'Y')
      {
       y_mid = Serial.parseInt();
       Pos(x,y);
      }
    }
  }
  
  else if(Serial.available() == 0){ //If face is not detected within 5 second then go to 0,0 position and search for the whole room
    count += 1;
    delay(1000);
    
    if (count > 5){
      for (int i = xpos; i>=0; i-=5){
        servoX.write(i);
        delay(70);
      }
      for (int j = ypos; j>=70; j-=5){
        servoY.write(j);
        delay(90);
      }
      search(); //Search function
    }
  }
}
```

If face is detected then we go to the pos() function, first we get the center coordinates of the face, and if the face is offset by 40px from the middle
we will move by 1 degree to X or Y axis until the face is on the middle screen again

```
void Pos() 
{
    if (x_mid > ((width / 2) + 40)){
      xpos -= angle;
    }
    else if (x_mid < ((width / 2) - 40)){
      xpos += angle;
    }
    
    if (y_mid < ((height / 2) - 40)){
      ypos += angle;
    }
    else if (y_mid > ((height / 2) + 40)){
      ypos -= angle;
    }
```

if the servo degree is outside its range

```
    if (xpos >= 180){
      xpos = 180;
    }
    else if (xpos <= 0){
      xpos = 0;
    }
    
    if (ypos >= 180){
      ypos = 180;
    }
    else if (ypos <= 60){  //Supaya gak kepentok, jadinya min di 60
      ypos = 60;
    }
```

Move to the face coordinates

```
    servoX.write(xpos);
    servoY.write(ypos);
}
```

If no face is detected then we will go to the serach() function, where it will move both servos from 5-179 for the X-axis servo and 70-120 for
the Y-axis servos, while also checking if there is any face detected, if no face is still detected until the end of each axis, then we will do the same
backward, 
