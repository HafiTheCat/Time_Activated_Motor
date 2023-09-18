#include <Stepper.h>  //Lib for Stepper Motor
#include "DCF77.h"    //Lib for DCF77 receiver
#include "TimeLib.h"  //Lib for Time functions

//GENERAL
#define BT_BOT 4
#define BT_TOP 5
#define SWITCH 6
#define BUZZER 7
#define Thread1_TIME 1000
#define Thread2_TIME 1

//DCF77
#define DCF_INPUT 2     //Pin for DCF77 receiver
#define DCF_INTERRUPT 0 //DCF Interrupt

//STEPPER MOTOR
#define dirA 12
#define dirB 13
#define pwmA 3
#define pwmB 11
#define breakA 9
#define breakB 8
#define steps 200

time_t time;
DCF77 DCF = DCF77(DCF_INPUT,DCF_INTERRUPT);
bool activated=LOW,msDOWN=LOW,msUP=LOW;
int lastExecuteTime1=0,lastExecuteTime2=0;
Stepper stepper1(steps, dirA, dirB);

void digitalClockDisplay(){
  //displays time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits){
  Serial.print(":");
  if(digits < 10)
  Serial.print('0');
  Serial.print(digits);
}

void closedoor(){
  activated=HIGH;
  if(digitalRead(BT_TOP)==LOW && digitalRead(BT_BOT)==HIGH)
  msDOWN=HIGH; //moveStepperDOWN
  }

void opendoor() {
  activated=HIGH;
  if(digitalRead(BT_TOP)==HIGH && digitalRead(BT_BOT)==LOW)
  msUP=HIGH; //moveStepperUP
}
void setup(){
  Serial.begin(9600); //Enables Serial Monitor
  //Setting PinModes GENERAL
  pinMode(BT_BOT, INPUT_PULLUP);
  pinMode(BT_TOP, INPUT_PULLUP);
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  
  //Setting PinModes DCF
  pinMode(DCF_INPUT, INPUT);
  
  //Setting PinModes MOTOR
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(breakA, OUTPUT);
  pinMode(breakB, OUTPUT);
  
  //setting initial OUTPUT
  digitalWrite(pwmA, HIGH);
  digitalWrite(pwmB, HIGH);

  stepper1.setSpeed(60);
  DCF.Start();
  setTime(5,59,50,16,7,2020);
}
void thread1(){ //Every 1000 milliseconds
  time_t DCFtime = DCF.getTime(); // Check if new DCF77 time is available
  if (DCFtime!=0)
  {
  Serial.println("Time: Up to date");
  tone(BUZZER,440,3000); //440Hz tone on Piezo element for 3 seconds
  setTime(DCFtime); //Sets current time to DCFtime which got received from the DCF77 receiver
  }
  digitalClockDisplay(); //OUTPUTS clock on Serial Monitor
  
  if(hour()==8 || hour()==1) activated=LOW; //reset time
  if(digitalRead(SWITCH)==HIGH) {
    if (activated!=HIGH){
      if(hour()==6) { 
        opendoor();
      }

      if(hour()==23) {
        closedoor();
      }
    }
  }
}

void thread2(){ //Every millisecond
  if(msDOWN==HIGH)stepper1.step(10);
  if(digitalRead(BT_BOT)==LOW) msDOWN=LOW;
  if(msUP==HIGH)stepper1.step(-10);
  if(digitalRead(BT_TOP)==LOW) msUP=LOW;
}
  
void loop(){ //fake threads
  if(millis() - lastExecuteTime1 >= Thread1_TIME){
    thread1();
    lastExecuteTime1 = millis();
  }
  if(millis() - lastExecuteTime2 >= Thread2_TIME){
    thread2();
    lastExecuteTime2 = millis();
  }
}
