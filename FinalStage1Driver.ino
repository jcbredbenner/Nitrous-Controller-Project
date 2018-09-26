//Final Stage 1

//==== Notes
//deleted reset. might want later
//fuel serial connnection needs testing (deleted)

//==== Inputs
//rx - mega communication

//==== Outputs
//10 - stage 1 relay
//2 - timing retard 1
//4 - fuel pump/valve

#include <PWM.h>

byte sentBytes[]= {0, 0};
int percent = 0;
int rampTime = 0;
int finalduty = 255;
int addDuty = 0;
int pwmpin = 10;
int32_t frequency = 14;
bool raceGas = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Timer1_Initialize(); //pins 9 and 10
  Timer1_SetFrequency(frequency);

  pinMode(2,OUTPUT);
  digitalWrite(2, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    Serial.readBytes(sentBytes, 2);
  }

//  if (sentBytes[2]>10) {  //race gas on
//    raceGas = true;
//    digitalWrite(4, LOW);
//  }
//
//  if (sentBytes[2]<10) {  //race gas off
//    raceGas = false;
//    digitalWrite(4, HIGH);
//  }
  
  if ((sentBytes[0]>0) && (sentBytes[0]<101)) {
    percent = sentBytes[0];
    rampTime = sentBytes[1];
    int deltaT = rampTime*100;
    int startduty = (percent)/100*255; //percent * 255 = startduty
    int cycleDelay = deltaT / (finalduty - startduty); //cycle calculation
    digitalWrite(2, LOW);
    pwmWrite(pwmpin, startduty + addDuty); //sends pulse to 40a ssr
    delay (cycleDelay); //One calulation per second.
    if(startduty + addDuty != finalduty){ //ramps duty
        addDuty ++;
    }
  }   
  
  if ((sentBytes[0] == 0) || (sentBytes[1] == 0)) {
    pwmWrite(pwmpin, 0);
    digitalWrite(2, HIGH);
    addDuty = 0;
  }


}

