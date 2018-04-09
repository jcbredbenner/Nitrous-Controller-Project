#include <PWM.h>


byte sentBytes[]= {0, 0};
int percent = 0;
int rampTime = 0;
int finalduty = 255;
int addDuty = 0;
int pwmpin = 9;
int32_t frequency = 14;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(4,OUTPUT);

  Timer1_Initialize(); //pins 9 and 10
  Timer1_SetFrequency(frequency);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    Serial.readBytes(sentBytes, 2);
  }
  
  if ((sentBytes[0]>0) && (sentBytes[0]<101) && (sentBytes[1]>0)) {
    percent = sentBytes[0];
    rampTime = sentBytes[1];
    int deltaT = rampTime*100;
    int startduty = (percent)/100*255; //percent * 255 = startduty
    digitalWrite(4, HIGH);
    int cycleDelay = deltaT / (finalduty - startduty); //cycle calculation
    pwmWrite(pwmpin, startduty + addDuty); //sends pulse to 40a ssr
    delay (cycleDelay); //One calulation per second.
    if(startduty + addDuty != finalduty){ //ramps duty
        addDuty ++;
    }
  }   
  
  if (sentBytes[0] == 0 && sentBytes[1] == 0) {
    pwmWrite(pwmpin, 0);
    digitalWrite(4, LOW);
    addDuty =0;
  }

  if (sentBytes[0]>101) {
    addDuty = 0;
  }
}
