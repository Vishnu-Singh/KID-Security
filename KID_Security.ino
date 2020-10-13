/*************************************************************
  ************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT DebugSerial


// You could use a spare Hardware Serial on boards that have it (like Mega)
#include <SoftwareSerial.h>
SoftwareSerial DebugSerial(2, 3),Serial1(4,5); // RX, TX


#include <BlynkSimpleStream.h>
#include <Wire.h>
#include <DS1307.h>
#include <TinyGPS.h>
int noisePin=9;
int buzzerPin=8;
int panicPin=10;

//char auth[] = "H2ajExldKt1umhBDhToljgeQePp_ojem";
char auth[]="wPO3cDjREgdtcUOpk4xWKEPckXLzmLmq";

DS1307 rtc;
TinyGPS gps;
uint8_t sec,min,hr,day,month;
uint16_t yr;
String ids[]={"1E00CCA51562","1200853A5EF3"};

WidgetMap myMap(V17);
void setup()
{
  // Debug console
  //pinMode(11,LOW);
  pinMode(noisePin,INPUT);
  pinMode(buzzerPin,OUTPUT);
  pinMode(panicPin,INPUT_PULLUP);
  DebugSerial.begin(9600);
  Serial1.begin(9600);

  // Blynk will work through Serial
  // Do not read or write this serial manually in your sketch
  Serial.begin(9600);
  Blynk.begin(Serial, auth);
  rtc.set(10,15,0,20,02,2020);
  rtc.start();
  
}

void loop()
{
  Blynk.run();
  checkID();
  checkNoise();
  panicStatus();
}

void getTime(){
  String t=" ";
  rtc.get(&sec,&min,&hr,&day,&month,&yr);
  t=String(sec)+":"+String(min)+":"+String(hr)+":"+String(day)+":"+String(month)+":"+String(yr);
  Blynk.virtualWrite(V10,t);
}
void checkID(){
  if(Serial1.available()){
    String id=Serial1.readStringUntil('$');
    id.trim();
    Blynk.virtualWrite(V16,id);
    for(int i=0;i<2;i++){
      if(ids[i].equals(id)){
        Blynk.virtualWrite(V11,"Entered");
        getTime();
      }
    }
  }
}

void checkNoise(){
  if(digitalRead(noisePin)==HIGH){
    Blynk.virtualWrite(V12,"Noisey");
    digitalWrite(buzzerPin,HIGH);
  }else{
    Blynk.virtualWrite(V12,".....");
    digitalWrite(buzzerPin,LOW);
  }
}

void panicStatus(){
  if(digitalRead(panicPin)==LOW){
    Blynk.virtualWrite(V15,"Panic");
    while(Serial.available()){
         String loc=" "; 
         float lat,lon;
         int c=Serial.read();
         if(gps.encode(c)){
          gps.f_get_position(&lat,&lon);
          Blynk.virtualWrite(V13,lat);
          Blynk.virtualWrite(V14,lon);
          myMap.location(0,lat,lon,"child");
         }
    }
  }else{
      Blynk.virtualWrite(V15,".....");
    }
  }
