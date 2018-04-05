

// 2 Stage Progressive Nitrous Controller

// Brenner Maull
// need to create a main menu
// ignition
// gas pump switch
// if not on race gas, don't engage nitrous
// home - red circle button for ignition, two rectangle buttons for fuel and nitrous
// 320 x 240
 
/*  This program uses the UTFT and URTouch libraries
 *  made by Henning Karlsen. 
 *  You can find and download them at:
 *  www.RinkyDinkElectronics.com
 */
#include <PWM.h>
#include <UTFT.h> 
#include <URTouch.h>

//==== Creating Objects
UTFT    myGLCD(CTE32_R2, 38, 39, 40, 41); //Parameters should be adjusted to your Display/Schield model
URTouch  myTouch( 6, 5, 4, 3, 2);

//==== Defining Variables
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
int x, y;
int a = 1;

boolean stage1 = false; //Determines if stages are armed
boolean stage2 = false;

int32_t frequency = 14;

int pwmpin1 = 11;

float time1 = 1.0;
float time2 = 1.0;

int percent1 = 25;
int percent2 = 25;

int finalduty = 255;

int n = 0;

bool trigger1 = false; //Nitrous on from outside source (rpm switch)
bool trigger2 = false;

int triggerpin1 = 1; //used in triggers program
int triggerpin2 = 2;

int addDuty1 = 0; //serves as step value for progression

bool buttonAct1 = false; //determines if stage comes on due to button activation or auto
bool buttonAct2 = false;

bool button = false; //outside activation button

bool raceGas = true; //safety mechanism to only allow nitrous with race gas

char currentPage, selectedUnit;

void setup() {
// Initial setup
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  
  //allows timers to be altered
  Timer1_Initialize(); //pins 11, 12
  Timer2_Initialize(); //pins 9, 10
 
  //sets the frequency for the specified timer
  Timer1_SetFrequency(frequency);
  Timer2_SetFrequency(frequency);

  pinMode(A4,OUTPUT); //first stage retard
  pinMode(A5,OUTPUT); //second

  pinMode(A1,INPUT); //first stage trigger
  pinMode(A2,INPUT); //second stage

  pinMode(A8,INPUT); //activation button

  pinMode(A9,OUTPUT); //fuel pump and fuel valve

  Serial1.begin(9600); //establishes communication with stage 2 driver
  

// main

  welcomeMessage();  // displays Welcome message  
  drawHomeScreen(); // draws Home screen
  currentPage = 'h'; // Indicates that we are at Home Screen
  
}

void loop() {
  //Nitrous Functions
  triggers(triggerpin1); //reads for stage 1 trigger
  triggers(triggerpin2); //reads for stage 2

  if (analogRead(A8) >= 50) {  //reads for button being pressed
    button = true; 
  }
  if (analogRead(A8) < 50) {  //reads for button off
    button = false; 
  }
  

    
  if (stage1 == true && trigger1 == true && raceGas == true){                          //Stage 1 Activation                //EDIT
      if (buttonAct1 == true && button == true || buttonAct1 == false){
        int startduty1 = (percent1)/100*255; //percent * 255 = startduty 
        int deltaT1 = (time1)*1000; // Time interval for nitrous to go from startduty -> endduty (ms)
        int cycleDelay1 = deltaT1 / (finalduty - startduty1); //cycle calculation
        digitalWrite(A4, HIGH);
        pwmWrite(pwmpin1, startduty1 + addDuty1); //sends pulse to 40amp ssr
        delay (cycleDelay1); //One calulation per second.
        if(startduty1 + addDuty1 != finalduty){ //ramps duty
          addDuty1 ++;
        }
      }
  }
  if (stage1 == false || trigger1 == false || raceGas == false || buttonAct1 == true && button == false){
      pwmWrite(pwmpin1, 0);
      digitalWrite(A4, LOW);
      addDuty1 = 0;
  }
  if (stage2 == true && trigger2 == true && raceGas == true){                            //Stage 2 Activation
    if (buttonAct2 == true && button == true || buttonAct2 == false){
      int deltaT2 = (time2)*10;
      byte array2[] = {percent2, deltaT2};
      Serial1.write(array2, 2);
      digitalWrite(A5, HIGH);
    }  
  }
  if (stage2 == false || trigger2 == false || raceGas == false  || buttonAct2 == true && button == false){
      byte off2[] = {0,0};
      Serial1.write(off2, 2);
      digitalWrite(A5, LOW);
      
  }
  

//BRENNER WORK START
                                                                                                              //hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
  if (currentPage == 'h') {
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX(); // X coordinate where the screen has been pressed
      y=myTouch.getY(); // Y coordinates where the screen has been pressed

      if ((x>=20) && (x<=180) && (y>=155) && (y<=225)) {
        currentPage = '0';
        myGLCD.clrScr();
        drawNitrousScreen();  // draws Nitrous Screen
        }

      if ((x>=20) && (x<=180) && (y>=30) && (y<=100)) {
        currentPage = 'f';
        myGLCD.clrScr();
        drawFuelScreen();  // draws Fuel Screen
             
        }
      }
    }

  //Fuel Screen
  if (currentPage == 'f') {
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX(); // X coordinate where the screen has been pressed
      y=myTouch.getY(); // Y coordinates where the screen has been pressed

      
      if (currentPage == 'f') {
        // Activation sequence
        if ((x>=120) && (x<=210) && (y>=45) && (y<=145)) {
          raceGas = true; // sets race gas to true
          myGLCD.clrScr();
          // Activation message
          myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
          myGLCD.setColor(30,255,30); // Sets color to white
          myGLCD.setFont(BigFont); // Sets font to big
          myGLCD.print("Race fuel", CENTER, 110); // Prints the string on the screen
          myGLCD.print("activated", CENTER, 130); // Prints the string on the screen
          delay(1000); // Delays welcome screen for ___ milliseconds
          drawFuelScreen();
        }
        
        //return to home screen button
        if ((x>=120) && (x<=200) && (y>=210) && (y<=250)) {
          currentPage = 'h';
          myGLCD.clrScr();
          drawHomeScreen();
        }

        // deactivation sequence
        if ((x>=0) && (x<=40) && (y>=90) && (y<=130)) {
          raceGas = false; // sets race gas to false
          myGLCD.clrScr();
          // Deactivation message
          myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
          myGLCD.setColor(255,30,30); // Sets color to white
          myGLCD.setFont(BigFont); // Sets font to big
          myGLCD.print("Race fuel", CENTER, 110); // Prints the string on the screen
          myGLCD.print("deactivated", CENTER, 130); // Prints the string on the screen
          delay(1000); // Delays welcome screen for ___ milliseconds
          drawFuelScreen();
        }
      }
    }
  }
  
 //BRENNER WORK END
  
  //Nitrous Screen
  if (currentPage == '0') {
    
    
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX(); // X coordinate where the screen has been pressed
      y=myTouch.getY(); // Y coordinates where the screen has been pressed
      // Stage 1
      if ((x>=35) && (x<=285) && (y>=90) && (y<=130) && raceGas == true) {
        drawFillRed(35, 90, 285, 130, 102); // Custom Function -Highlights the buttons when it's pressed
        stage1 = true;
      }
      // Stage 2
      if ((x>=35) && (x<=285) && (y>=140) && (y<=180) && raceGas == true) {
        drawFillRed(35, 140, 285, 180, 152);
        stage2 = true;
      }
            
      //Stage 1 cancel
      if ((x>=0) && (x<=30) && (y>=90) && (y<=130)) {
        drawFillGreen(35, 90, 285, 130, 102, 1);
        stage1 = false;
      }
      //Stage 2 cancel
      if ((x>=0) && (x<=30) && (y>=140) && (y<=180)) {
        drawFillGreen(35, 140, 285, 180, 152, 2);
        stage2 = false;
      }
      
      //settings
      if ((x>=290) && (x<=320) && (y>=90) && (y<=130)) {
        currentPage='1';
        drawMenu(1);
        stage1 = false; //turns stages off
        stage2 = false;
      }
      if ((x>=290) && (x<=320) && (y>=140) && (y<=180)) {
        currentPage='2';
        drawMenu(2);
        stage1 = false;
        stage2 = false;
        
      }
      
      //return to home screen button
      if ((x>=110) && (x<=200) && (y>=200) && (y<=240)) {
        currentPage = 'h';
        myGLCD.clrScr();
        drawHomeScreen();
      }
    }
  }
  if (currentPage == '1') { //Menu for Stage 1
    myGLCD.setColor(255,255,255);  
    myGLCD.printNumI(percent1, 50, 142);
    myGLCD.printNumF(time1, 1, 232, 142);
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX(); // X coordinate where the screen has been pressed
      y=myTouch.getY(); // Y coordinates where the screen has been pressed
      if ((x>=105) && (x<=215) && (y>=185)) {
        currentPage='0';
        drawNitrousScreen();
      }
      menuButtons(1);
       
    }
  }
  if (currentPage == '2') { //Menu for Stage 2
    myGLCD.setColor(255,255,255);  
    myGLCD.printNumI(percent2, 50, 142);
    myGLCD.printNumF(time2, 1, 232, 142);
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX(); // X coordinate where the screen has been pressed
      y=myTouch.getY(); // Y coordinates where the screen has been pressed
      if ((x>=105) && (x<=215) && (y>=185)) {
        currentPage='0';
        drawNitrousScreen();
      }
      menuButtons(2); 


    }
  }  
}

void drawFrame(int x1, int y1, int x2, int y2) { //makes border
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (x1, y1, x2, y2);
}
void menuButtons(int stage) {
      myGLCD.setColor(255,255,255);
      if  ((x>=35) && (y>=75) && (x<=95) && (y<=125)) {  //percent up
        myGLCD.setBackColor(0,0,0);
        drawFrame(35, 75, 95, 125);
        if (stage == 1 && percent1 != 99) {
          percent1 ++;
          myGLCD.printNumI(percent1, 50, 142);
        }
        if (stage == 2 && percent2 != 99) {
          percent2 ++;
          myGLCD.printNumI(percent2, 50, 142);
        }        
      }
      
      if ((x>=35) && (y>=175) && (x<=95) && (y<=225)) {      //percent down
        myGLCD.setBackColor(0,0,0);
        drawFrame(35, 175, 95, 225);
        if (stage == 1 && percent1 != 10) {
          percent1 --;
          myGLCD.printNumI(percent1, 50, 142);
        }
        if (stage == 2 && percent2 != 10) {
          percent2 --;
          myGLCD.printNumI(percent2, 50, 142);
        }       
      }

      if ((x>=130) && (y>=75) && (x<=190) && (y<=125)) {    //button activation on
        myGLCD.setColor(255, 255, 255);
        myGLCD.fillRoundRect(130, 75, 190, 125);
        myGLCD.setColor(0, 0, 0);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.print("on", 152, 93);
        if (stage == 1) {
          buttonAct1 = true;         
        }
        if (stage == 2) {
          buttonAct2 = true;
        }
      }

      if ((x>=130) && (y>=140) && (x<=190) && (y<=190)) {     //button activation off
        myGLCD.setColor(255, 255, 255);
        myGLCD.fillRoundRect(130, 75, 190, 125);
        myGLCD.setColor(0, 0, 0);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.print("off", 152, 158);
        if (stage == 1) {
          buttonAct1 = false;         
        }
        if (stage == 2) {
          buttonAct2 = false;
        }
      }
     
      if  ((x>=225) && (y>=75) && (x<=285) && (y<=125)) {    //time up
        myGLCD.setBackColor(0,0,0);
        drawFrame(225, 75, 285, 125);
        if (stage == 1 && int(time1*10) != 99) {
          time1=time1+.1;
          myGLCD.printNumF(time1, 1, 232, 142);
        }
        if (stage == 2 && int(time2*10) != 99) {
          time2=time2+.1;
          myGLCD.printNumF(time2, 1, 232, 142);
        }        
      }
      
      if ((x>=225) && (y>=175) && (x<=285) && (y<=225)) {     //time down
        myGLCD.setBackColor(0,0,0);
        drawFrame(225, 175, 285, 225);
        if (stage == 1 && int(time1*10) != 1) {
          time1=time1-.1;
          myGLCD.printNumF(time1, 1, 232, 142);
        }
        if (stage == 2 && int(time2*10) != 1) {
          time2=time2-.1;
          myGLCD.printNumF(time2, 1, 232, 142);
        }        
      }
}  
void drawFillRed(int x1, int y1, int x2, int y2, int center) { //makes choice armed 
  myGLCD.setColor(255, 0, 0); // Sets red color
  myGLCD.fillRoundRect (x1, y1, x2, y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.drawRoundRect (x1, y1, x2, y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont); // Sets the font to big
  myGLCD.setBackColor(255, 0, 0); // Sets the background color of the area where the text will be printed to red, same as the button
  myGLCD.print("Armed", CENTER, center); // Prints the string
}

void drawFillGreen(int x1, int y1, int x2, int y2, int center, int stage) { //makes choice return
  myGLCD.setColor(16, 167, 103); // Sets green color
  myGLCD.fillRoundRect (x1, y1, x2, y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.drawRoundRect (x1, y1, x2, y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont); // Sets the font to big
  myGLCD.setBackColor(16, 167, 103); // Sets the background color of the area where the text will be printed to green, same as the button
  if (stage == 1) {
    myGLCD.print("Stage 1", CENTER, center); // Prints the stage 1 text
  }
  if (stage == 2) {
    myGLCD.print("Stage 2", CENTER, center); // Prints the stage 2 text
  }
}

void drawMenu(int stage) {
  myGLCD.clrScr();
  //Title
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(BigFont); // Sets font to big
  if (stage == 1){
    myGLCD.print("Stage 1", CENTER, 10); // Prints the string on the screen
  }
  if (stage == 2){
    myGLCD.print("Stage 2", CENTER, 10); // Prints the string on the screen
  }
  
  myGLCD.setColor(255, 0, 0); // Sets color to red
  myGLCD.drawLine(0,32,319,32); // Draws the red line

  //Start Percent
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Start%", 23, 57);
  myGLCD.fillRect(35, 75, 95, 125);
  myGLCD.fillRect(35, 175, 95, 225);
  myGLCD.drawRect(35, 125, 95, 175);
  myGLCD.setColor(0,0,0);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print("+", 58, 93);
  myGLCD.print("-", 58, 195);


  //Button Activation
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("Button", 120,57);
  myGLCD.drawRoundRect(130, 75, 190, 125);
  myGLCD.drawRoundRect(130, 140, 190, 190);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("on", 148, 93);
  myGLCD.print("off", 145, 158);

  if ((stage == 1) && (buttonAct1 == true)) {
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRoundRect(130, 75, 190, 125);
    myGLCD.setColor(0, 0, 0);
    myGLCD.setBackColor(255, 255, 255);
    myGLCD.print("on", 152, 93);
  }

  if ((stage == 2) && (buttonAct2 == true)) {
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRoundRect(130, 75, 190, 125);
    myGLCD.setColor(0, 0, 0);
    myGLCD.setBackColor(255, 255, 255);
    myGLCD.print("on", 152, 93);
  }

  if (((stage == 1) && (buttonAct1 == false)) || ((stage == 2) && (buttonAct2 == false))) {
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRoundRect(130, 75, 190, 125);
    myGLCD.setColor(0, 0, 0);
    myGLCD.setBackColor(255, 255, 255);
    myGLCD.print("off", 152, 158);
  }  
  

  //Time
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0,0,0);
  myGLCD.print("Time", 222, 57);
  myGLCD.fillRect(225, 75, 285, 125);
  myGLCD.fillRect(225, 175, 285, 225);
  myGLCD.drawRect(225, 125, 285, 175);
  myGLCD.setColor(0,0,0);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print("+", 248, 93);
  myGLCD.print("-", 248, 195);

  //Back Button
  myGLCD.setColor(255, 0, 0);
  myGLCD.setBackColor(0,0,0);
  myGLCD.print("Back", 128, 205);
}


void drawNitrousScreen() {
  
  myGLCD.clrScr();
  // Title
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255,255,255); // Sets color to white
  myGLCD.setFont(BigFont); // Sets font to big
  myGLCD.print("Nitrous Controller", CENTER, 10); // Prints the string on the screen
  myGLCD.setColor(255, 0, 0); // Sets color to red
  myGLCD.drawLine(0,32,319,32); // Draws the red line
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(BigFont);
  myGLCD.print("Select Stage", CENTER, 70);
  
  // Button - Stage 1
  myGLCD.setColor(16, 167, 103); // Sets green color
  myGLCD.fillRoundRect (35, 90, 285, 130); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.drawRoundRect (35, 90, 285, 130); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont); // Sets the font to big
  myGLCD.setBackColor(16, 167, 103); // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("Stage 1", CENTER, 102); // Prints the string
  
  // Button - Stage 2
  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (35, 140, 285, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (35, 140, 285, 180);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.print("Stage 2", CENTER, 152);

  //Resets
  myGLCD.setColor(255, 0, 0);
  myGLCD.setBackColor(0,0,0);
  myGLCD.print("X",8,102);
  myGLCD.print("X",8,152);

  //Settings
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0,0,0);
  myGLCD.setFont(SmallFont);
  myGLCD.print("...", 293, 102);
  myGLCD.print("...", 293, 152);

  
  // Button - Home
  myGLCD.setColor(255,255,255);
  myGLCD.setFont(BigFont);
  myGLCD.print("Home", CENTER, 220);
}

void triggers(int trigger){
  if (analogRead(trigger) >= 50) {
    if (trigger == 1) {
      trigger1 = true;  
    }
     if (trigger == 2) {
      trigger2 = true;  
    }
  }
  if (analogRead(trigger) < 50) {
    if (trigger == 1) {
      trigger1 = false;  
    }
     if (trigger == 2) {
      trigger2 = false;  
    }
  }
}


//BRENNER WORK START
  
void welcomeMessage() {
  myGLCD.clrScr();
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255,255,255); // Sets color to white
  myGLCD.setFont(BigFont); // Sets font to big
  myGLCD.print("Welcome back", CENTER, 120); // Prints the string on the screen
  delay(1000); // Delays welcome screen for ___ milliseconds
}

void drawHomeScreen() {
  myGLCD.clrScr();
  // Title
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255,255,255); // Sets color to white
  myGLCD.setFont(BigFont); // Sets font to big
  myGLCD.print("Home", CENTER, 10); // Prints the string on the screen
  myGLCD.setColor(255, 0, 0); // Sets color to red
  myGLCD.drawLine(0,32,319,32); // Draws the red line

  
  // Button - Fuel
  myGLCD.setColor(100, 100, 100); // Sets color
  myGLCD.fillRoundRect (20, 50, 180, 120); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.drawRoundRect (20, 50, 180, 120); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont); // Sets the font to big
  myGLCD.setBackColor(100, 100, 100); // Sets the background color of the area where the text will be printed to ______, same as the button
  myGLCD.print("Fuel", 70, 80); // Prints the string
  
  // Button - Nitrous
  myGLCD.setColor(255, 130, 30);
  myGLCD.fillRoundRect (20, 155, 180, 225);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (20, 155, 180, 225);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(255, 130, 30);
  myGLCD.print("NOS", 75, 185);

  // Button - Ignition
  myGLCD.setColor(200, 0, 0);
  myGLCD.fillCircle (250, 140, 60);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawCircle (250, 140, 60);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(200, 0, 0);
  myGLCD.print("Start", 212, 130);
}

void drawFuelScreen() {
  
  myGLCD.clrScr();
  // Title
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255,255,255); // Sets color to white
  myGLCD.setFont(BigFont); // Sets font to big
  myGLCD.print("Race Fuel", CENTER, 10); // Prints the string on the screen
  myGLCD.setColor(255, 0, 0); // Sets color to red
  myGLCD.drawLine(0,32,319,32); // Draws the red line
  myGLCD.setColor(255, 255, 255); // Sets color to white

  // Button - Home
  myGLCD.setColor(255,255,255);
  myGLCD.setFont(BigFont);
  myGLCD.print("Home", CENTER, 220);

  // Button - Race fuel activation button
  myGLCD.setColor(200, 0, 0);
  myGLCD.fillCircle (160, 120, 70);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawCircle (160, 120, 70);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(200, 0, 0);
  myGLCD.print("Activate", 97, 110);

  // Button - Deactivation
  myGLCD.setColor(255, 0, 0);
  myGLCD.setBackColor(0,0,0);
  myGLCD.print("X",30,110);
}

