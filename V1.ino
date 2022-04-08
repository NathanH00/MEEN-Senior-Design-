#include <BfButton.h>
#include <BfButtonManager.h>
int buttonPin = 3;//GPIO button on encoder
int DT = 53; //GPIO DT on encoder (output B)
int CLK = 52; //GPIO CLK on encoder (Output A)
//BfButton btn(BfButton::STANDALONE_DIGITAL, buttonPin, true, LOW);

float counter = 0;
int angle = 0;
int aState;
int aLastState;
//track encoder
void eTrack(int aState){
  if(aState != aLastState){
    if(digitalRead(DT) != aState){
      counter += 0.005;
      angle ++;
    }
    else{
      counter -= 0.005;
      angle --;
    }
    if(counter >= 12.00){
      counter = 12;
    }
    if(counter <=0){
      counter = 0;
    }
  }
  aLastState = aState;
}
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################


//LCD
#import <LiquidCrystal.h>
LiquidCrystal lcd(43, 45, 42, 41, 40, 39, 38);
//LiquidCrystal lcd(12,11,10,5,4,3,2);
#define contra 8
#define bri 12


//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################

//Start = pin 32; //Button 1
//Stop = pin 33 //Button 2
//Toggledrain/fill = pin 34; //Button 3
// = pin 35; //Button 4
//Array for button pins
byte buttons[] = { 1, 32, 33, 35, 34 };
//Number of buttons
const byte numButtons = 5;
//Check Button State

byte checkButtonState() {
  byte currentButton = 0;
  byte readCurrentButton = 0;


  //loop through the bottons to see which is pressed
  for (byte t = 0; t < numButtons; t++) {
    //Serial.println("checking Button");
    //Serial.println(digitalRead(buttons[1]));
    if (digitalRead(buttons[t]) == 1) {
      currentButton = t;
    }
    readCurrentButton = currentButton;
    //wait for the button to be unpressed
  }

  while(currentButton != 0){
    currentButton = 0;
    for(int t = 0; t < numButtons; t ++){
      if(digitalRead(buttons[t]) == 1){
        currentButton = t;
      }
    }
    
  }
  return readCurrentButton;
}



int updateMenu(int subID, int readCurrentButton){
  if(readCurrentButton == 1 && subID == 1){ subID += 1; }
  //if(readCurrentButton == 2 && subID == 2){ subID -= 1; }
  return subID;
}

int execMenuFunc(int subID,float counter, bool fToggle){
 switch(subID){
   case 1:
    lcd.setCursor(0,0); 
    lcd.print(counter);
    lcd.print(" L");
    lcd.setCursor(0,1);
    if(fToggle == true){
      lcd.print("Fill");
    }
    else{
      lcd.print("Drain");
      lcd.clear();
    }
    break;
   case 2:
    //Execute Stepper Code 
    Serial.println(fToggle);
    lcd.clear();
    lcd.setCursor(0,0); 
    if(fToggle == true){
      lcd.print("Filling");
      runStepper(counter*1000,fToggle);
    }
    else{
      lcd.print("Draining");
      runStepper(counter*1000,fToggle);
    }
    //once finished filling, got back to subID 1
    subID = 1;
    lcd.clear();
    break;
   /*case 3:
   lcd.clear();
   //stop process
    lcd.setCursor(0,0); 
    lcd.print("Stopping");
    subID = 1;
    break;
    **/
 } 
 return subID;
}

//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//update fill/drain

bool updateFT(int userIn, bool fToggle){
  if(userIn == 4){
    fToggle = !fToggle;
  }
  return fToggle;
}

//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//#########################################################
//Stepper Motor Code
#include <Stepper.h>
Stepper myStepper(200,2,3,4,5);// initiate motor
bool on = false;

void runStepper(float countTo,bool fToggle){
  for(float revCount = 0; revCount*3.125 < countTo; revCount ++){
    //add function to check for stop button
    if(checkButtonState() == 2){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("STOPPING");
      delay(2000);
       break;
    }

    if(fToggle == true){
      myStepper.step(1600); Serial.println("Fill "); 
      lcd.setCursor(0,1);
      lcd.print(revCount*3.125);
      lcd.print(" ml");
    }
    else{
      myStepper.step(-1600); Serial.println("drain ");
      lcd.setCursor(0,1);
      lcd.print(revCount*3.125);
      lcd.print(" ml");
    }
    //lcd.setCursor(0,1);
    //lcd.print(ml);
    float ml = revCount*3.125;
    Serial.println("ml; ");
    Serial.println(ml);
  }
  //set revCount back to 0
  delay(300);
  lcd.clear();
}


bool fToggle = true;
int subID = 1;

void setup() {
  Serial.begin(9600);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  aLastState = digitalRead(CLK);

  //Button Settings
  //btn.onPress(pressHandler);

  //initialize LCD Screen
  lcd.begin(16, 2);
  pinMode(contra, OUTPUT);
  pinMode(bri, OUTPUT);
  digitalWrite(contra, LOW);
  analogWrite(bri, 255);

  //Stepper
  //Set rpm
  int rpm = 70;
  myStepper.setSpeed(rpm*8);
}

void loop() {
  int userIn = 0;
  aState = digitalRead(CLK);
  eTrack(aState);
  userIn = checkButtonState();
  fToggle = updateFT(userIn,fToggle);
  Serial.println(subID);
  subID = updateMenu(subID,userIn);
  subID = execMenuFunc(subID,counter,fToggle);
}
