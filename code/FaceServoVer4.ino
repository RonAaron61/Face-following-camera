/*
 * Sumber: https://www.learnrobotics.org/blog/face-tracking-opencv/
 * Sumber: https://create.arduino.cc/projecthub/shubhamsantosh99/face-tracker-using-opencv-and-arduino-55412e 
 * 
 * VER 4
 * melakukan pencarian wajah apabila tidak ditetksi wajah selama 5 detik
 */

#include<Servo.h>

Servo servoX; //Vertical Servo
Servo servoY; //Horizontal Servo

int x_mid;
int y_mid;

int width = 640, height = 480;  // total resolution of the video
int xpos = 90;
int ypos = 90;  // initial positions of both Servos

int angle = 1;  //Sudut tiap motor servo bergeser
int count = 0;  //Sebagai variable waktu untuk mulai search

void setup()
{
  Serial.begin(9600);
  servoY.attach(10); //Attach Vertical Servo to Pin 5
  servoX.attach(9); //Attach Horizontal Servo to Pin 6
  
  servoY.write(ypos);
  servoX.write(xpos);
}

void Pos()  //Servo menuju koordinat wajah
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


    // if the servo degree is outside its range
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

    servoX.write(xpos);
    servoY.write(ypos);
}

//INI KODENYA KALO BISA DISIMPLIFIKASI, BIAR GAK RIBET
void search(){  //Servo mencari scan wajah orang 180 derajat x dan y
  int delayX = 50;  //delay dari servoX
  int delayY = 90; //delay dari servoY

  ypos = 70;  //ubah ypos dan xpos
  xpos = 5;
  
  for (int x = 5; x <= 179; x+=15){ //first 180 derajat
    servoX.write(x);
    xpos = x;   /*ubah nilai xpos agar saat ngedetect wajah di sudut x saat ini pergerakan berlanjut dari seudut  tersebut dan gak tiba2 ke 90 derajt lagi*/
    
    for (int y = 70; y <= 120; y+=3){
      ypos = y;   //ubah sudut y same kayak di atas
      if(Serial.available() > 0){     /*jika ngedetsi ada wajah maka keluar looping*/
        break;
      }
      servoY.write(y);   /*kenapa di bawah, agar jika wajah kedetksi ngubah nilai dari ypos dulu aja, jadinya gerakan servo nya berlanjut dan gak tiba tiba keubah ke sudut sebelumnya*/
      delay(delayY);  
    }
    for (int y = 120; y >= 70; y-=3){
      if(Serial.available() > 0){ //ngecek kondisi dulu biar kalo seandainya ngedetksi wajah dia gk ngubah ke ke sudutr 120 derajat, tapi langsung break keluar dari looping nya 
        break;
      }
      ypos = y;
      servoY.write(y);
      delay(delayY);  
    }
    
    if(Serial.available() > 0){
      break;
    }
    delay(delayX);
  }
  
  if(Serial.available() == 0){    //cek apakah ada wajah apa gak dulu, biar gk keubah lagi x nya, misalnya pas diatas dia ngedetksi wajah di sudut 30 derajat, kalau gak di cek sekarang, nanti malah tb2 keubah sudutnya ke 179 derajat
    for (int x = 179; x >= 5; x-=15){   //sudut dari X  179 derajat balik ke 0 lagi 
      servoX.write(x);
      xpos = x;
      for (int y = 70; y <= 120; y+=3){
        ypos = y;
      
        if(Serial.available() > 0){
          break;
        }
        servoY.write(y);
        delay(delayY);  
      }
      for (int y = 120; y >= 70; y-=3){
        if(Serial.available() > 0){
          break;
        }
        ypos = y;
        servoY.write(y);
        delay(delayY);  
      }
    
      if(Serial.available() > 0){
        break;
      }
      delay(delayX);
    }
  }
  if(Serial.available() > 0){
    count = 0;
  }
}


void loop()
{
  if(Serial.available() > 0)
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
    //while(Serial.available() > 0)
    //{
      //Serial.read();
    //}
  }
  
  else if(Serial.available() == 0){ //Jika gak ngedetksi wajah, hitung sealam 5 detik, baru lakukan pencarian wajah
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
      search();
    }
  }
}
