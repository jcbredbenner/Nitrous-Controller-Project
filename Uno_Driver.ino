//Brenner Maull and Josh Bredbenner
//AP CSP Exam Create Task
//Nitrous Controller Solenoid Driver

#include <PWM.h>

byte sentBytes[]= {0, 0};
int percent = 0;
int rampTime = 0;
int finalduty = 255;
int addDuty = 0;
int pwmpin = 9;
int32_t frequency = 14;

void setup() {
  //serial and frequency
  Serial.begin(9600);

  Timer1_Initialize(); //pins 9 and 10
  Timer1_SetFrequency(frequency);
}

void loop() {
  // read and write
  if (Serial.available() > 0) {
    Serial.readBytes(sentBytes, 2);
  }
  
  if ((sentBytes[0]>0) && (sentBytes[0]<101) && (sentBytes[1]>0)) { //nitrous activation
    percent = sentBytes[0];
    rampTime = sentBytes[1];
    int deltaT = rampTime*100;
    int startduty = (percent)/100*255; //percent * 255 = startduty
    int cycleDelay = deltaT / (finalduty - startduty); //cycle calculation
    digitalWrite(2, HIGH);
    pwmWrite(pwmpin, startduty + addDuty); //sends pulse to 40a ssr
    delay (cycleDelay); //One calulation per second.
    if(startduty + addDuty != finalduty){ //ramps duty
        addDuty ++;
    }
  }   
  
  if (sentBytes[0] == 0 && sentBytes[1] == 0) { //deactivation
    pwmWrite(pwmpin, 0);
    digitalWrite(2, LOW);
    addDuty =0;
  }

  if (sentBytes[0]>101) { //reset
    addDuty = 0;
  }
}
