/*
Iambic CW Keyer
  
 Read the Bencher Iambic Key, 
   plays tone on speaker, 
   lights LED, 
   writes to console, 
   keys the radio all without using delay()..
 
 Turns on and off a light emitting diode(LED) connected to digital  
 pin 13, when paddle 1 or 2 or both pressed.
 
 The circuit:
 * Yellow wire to ground with 22K resistor to Red and 22K resisstor to Black
 * Red wire pin 12 - plug center contact
 * Green wire pin 11 - plug ground contact
 * Black wire pin 10 - plug tip contact
 * Tone appears on pin 9 - attach 8 ohm skeaker in series with 100 ohm resistor 
   to pin 9 and ground pin
 * LED attached from pin 13 to ground (on board)
 * pushbutton attached to pin 2 from +5V
 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.
 
 The dot duration is the basic unit of time measurement in code transmission.
 
 The duration of a dash is three times the duration of a dot. 
 Each dot or dash is followed by a short silence, equal to the dot duration. 
 The letters of a word are separated by a space equal to three dots (one dash), 
 and the words are separated by a space equal to seven dots. 
 
 created 2015
 by Dave Negrych VE4NEG
 
 This code is in the public domain.

 Dave Negrych 13 Apr 2018 - Corrected variable type in encode routine
                         - changed type from BOOLEAN to INT

  Dave Negrych 23 Apr 2018 - Corrected letter and ./- print toggle
                           - key 6 dits to toggle ./- print
                           - key 6 dahs to toggle leter print                         

 */
 
#include <Arduino.h>
#include "pitches.h"

//-------------------------------------------------------------------
const unsigned long baseTime = 100;       // Base timing duration in milli seconds
const int toneFreq = 950;       // Frequency of tone
const boolean debug = 0;     // debug print out set to 1 to debug
//-------------------------------------------------------------------
//const int setDB = 5;              // Set the debounce delay
const unsigned long ditTime = baseTime;
const unsigned long dahTime = 3 * baseTime;
const unsigned long letterSpaceTime = 3 * baseTime + 100;  // Add two more dit times, total 3
const unsigned long wordSpaceTime = 7 * baseTime;  // Add four more dit times, total 7
//-------------------------------------------------------------------
// set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int redWire =  12;      // the number of the LED pin
const int greenWire =  11;      // the number of the LED pin
const int blackWire =  10;      // the number of the LED pin

const boolean Opened = 0;
const boolean Closed = 1;

const int stateIdle = 0;
const int stateDit = 1;
const int stateDah = 2;
const int stateDitDah = 3;
const int stateSending = 4;
const int stateWait = 5;
const int stateUnk = 9;

//-------------------------------------------------------------------

int lastKeyState = stateUnk;
int currentKeyState = stateUnk;
int nextKeyState = stateUnk;

int lastPgmState = stateUnk;
int currentPgmState = stateUnk;
int nextPgmState = stateUnk;

int lastDigit = 1;  // 1 = dit 2 = dah
boolean newChar = 1;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

// Encode Decode variables
unsigned long code = 0;      // letter code
unsigned long offset = 1;    // digit position 
String letter = "x";    // the letter
String textParm = " ";

unsigned long millisec = 0;
unsigned long timer = millis();
unsigned long spaceTimer = millis();

boolean redState = stateUnk;
boolean blackState = stateUnk;
boolean redStateOld = stateUnk;
boolean blackStateOld = stateUnk;
boolean redStateNew = stateUnk;
boolean blackStateNew = stateUnk;

boolean toneSent = 0;
boolean letterSent = 0;
boolean wordSent = 0;
boolean lineSent = 0;
boolean lastSend;   // 0 dit 1 dah
boolean printDitDah = 1;  // 1 to print decoded dit/dah; 0 to not print dit/dah
boolean printLetter = 1;  // 1 to print decoded letter; 0 to not print letter

//======================================================
void Startup(){
 // Set up the serial connection to report information
  Serial.begin(115200);
  delay(100);
  Serial.println("Initialize Iambic Keyer");
  Serial.println("Code path /arduino/iambic-keyer/keyer");
  Serial.println("Initialize I/O pins");
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);      
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  pinMode(redWire, INPUT); 
  pinMode(greenWire, OUTPUT);
  digitalWrite(greenWire, LOW);  // clear signal
  pinMode(blackWire, INPUT);
  
  
  
  
  Serial.print("Base time = ");
  Serial.print(baseTime);
  Serial.println(" milliseconds");

  Serial.print("DIT time = ");
  Serial.print(ditTime);
  Serial.println(" milliseconds");
  
  Serial.print("DAH time = ");
  Serial.print(dahTime);
  Serial.println(" milliseconds");
  
  Serial.print("Character Space time = ");
  Serial.print(letterSpaceTime);
  Serial.println(" milliseconds");
  
  Serial.print("Word Space time = ");
  Serial.print(wordSpaceTime);
  Serial.println(" milliseconds");

  Serial.println("Key 6 dits to toggle ./- print");
  Serial.println("Key 6 dahs to toggle letter print");

  delay(1000);
}
//----------------------------------------------------------
void decode() {
 // lookup letter for code
  
  switch (code) {
        case 1: letter = "E"; break;
        case 11: letter = "I"; break;
        case 111: letter = "S"; break;
        case 1111: letter = "H"; break;
        case 11111: letter = "5"; break;
        
        case 12: letter = "N"; break;
        case 21: letter = "A"; break;
        
        case 112: letter = "D"; break;
        case 121: letter = "R"; break;        
        case 122: letter = "G"; break;
        case 211: letter = "U"; break;
        case 212: letter = "K"; break;
        case 221: letter = "W"; break;
        
        case 1112: letter = "B"; break;
        case 1121: letter = "L"; break;
        case 1122: letter = "Z"; break;
        case 1211: letter = "F"; break;
        case 1212: letter = "C"; break;
        case 1221: letter = "P"; break;
       //case 1222: letter = ""; break;
        case 2111: letter = "V"; break;
        case 2112: letter = "X"; break;
      //case 2121: letter = ""; break;
        case 2122: letter = "Q"; break;
      //case 2211: letter = ""; break;
        case 2212: letter = "Y"; break;
        case 2221: letter = "J"; break;
        
        case 11112: letter = "6"; break;
        //case 11121: letter = "6"; break;
        case 11122: letter = "7"; break;
        //case 11211: letter = "6"; break;
        //case 11212: letter = "6"; break;
        //case 11221: letter = "6"; break;
        case 11222: letter = "8"; break;
        //case 12111: letter = "6"; break;
        case 12112: letter = "/"; break;  // SLASH
        case 12121: letter = "+"; break;
        //case 12122: letter = "6"; break;
        //case 12211: letter = "6"; break;
        case 12222: letter = "9"; break;
        case 21111: letter = "4"; break;
        case 21112: letter = "="; break;
        //case 21121: letter = "6"; break;
        //case 21122: letter = "6"; break;
        //case 21211: letter = "6"; break;
        //case 21212: letter = "6"; break;
        //case 21221: letter = "@"; break; 
        //case 21222: letter = "6"; break;
        case 22111: letter = "3"; break;
        //case 22112: letter = "6"; break;
        //case 22121: letter = "6"; break;
        //case 22122: letter = "6"; break;
        case 22211: letter = "2"; break;
        //case 22212: letter = "6"; break;
        case 22221: letter = "1"; break;

        case 112211: letter = "?"; break;   // QUESTION MARK
        case 121221: letter = "@"; break;   // AT SIGN
        case 211112: letter = "-"; break;   // DASH
        case 212111: letter = "SK"; break;  // 
        // case 212121: letter = "."; break;   // PERIOD
        case 212121: letter = "(period)"; break;   // PERIOD
        case 221122: letter = "(comma)"; break;   // COMMA
        case 212212: letter = "()"; break;  // PARENTHESIS
                
        case 2: letter = "T"; break;
        case 22: letter = "M"; break;
        case 222: letter = "O"; break;
      //case 2222: letter = " "; break;  
        case 22222: letter = "0"; break;
        
        case 111111: letter = "Toggle dit/dah print";  // 6 dits to toggle dit/dah
           printDitDah = !printDitDah; // toggle 
           break;
           
        case 222222: letter = "Toggle letter print";  // 6 dahs to toggle
           printLetter = !printLetter; // toggle 
           break;
        
        default: letter = "(?)";
      }
  

    if (printLetter==1) {
        Serial.print(letter);
    }
   
    code=0; 
    offset=1;
}
//--------------------------------------------------------------
void readContactState() {

  digitalWrite(greenWire, HIGH);
  
  delay(10);
  redStateNew = digitalRead(redWire);
  blackStateNew = digitalRead(blackWire);
  
  if (redStateNew == redStateOld) {
    redState = redStateNew;
  }
  
  if (blackStateNew == blackStateOld) {
    blackState = blackStateNew;
  }
  
  redStateOld = redStateNew;
  blackStateOld = blackStateNew;
}

//--------------------------------------------------------------

void dit() {
  //currentKeyState = stateDit;
  lastDigit = 1;
  newChar = 1;
  if (printDitDah==1) {
    Serial.print(".");
  } 
  sendTone(ditTime);
  encode(1);
}
//--------------------------------------------------------------
void dah() {
  //currentKeyState = stateDah;
  lastDigit = 2;
  newChar = 1;
  if (printDitDah==1) {
    Serial.print("-");
    } 
  sendTone(dahTime);
  encode(2);
}
//--------------------------------------------------------------
void sendTone(long millisec) {
  setTimer(millisec);
  // Turn LED on
  digitalWrite(ledPin, HIGH);
  // Play tone
  tone(9, toneFreq, millisec);
 }
//-------------------------------------------------------------- 
 void setTimer(long millisec) {
      timer = millis() + millisec;  
}
//--------------------------------------------------------------
void checkSpaceTimer(){

      if (letterSent == 1 && millis() > (spaceTimer + letterSpaceTime)) {
      // letter break
        printStats("decode letter");
        letterSent = 0;
        decode();
        }
       else if (wordSent == 1 && millis() > (spaceTimer + wordSpaceTime)) {
      // word break
      
        wordSent = 0;
        printStats("next word");
        Serial.print(" ");
      }
      else if (lineSent == 1 && millis() > (spaceTimer + wordSpaceTime + wordSpaceTime)) {
      // word break
      
        lineSent = 0;
        printStats("next word");
        Serial.println("");
      }
    }
//--------------------------------------------------------------
void nextState() {
  currentPgmState = nextKeyState;
}
//--------------------------------------------------------------
void setKeyState(){
  
  if ( redState == Opened && blackState == Opened ) {
      nextKeyState = stateIdle;
    }
  else if ( redState == Opened && blackState == Closed) {
      nextKeyState = stateDah;
    }
  else if ( redState == Closed && blackState == Opened) {
      nextKeyState = stateDit;
    }
  else if ( redState == Closed && blackState == Closed) {
      nextKeyState = stateDitDah;
    } 
  else {
      nextKeyState = stateIdle;
    }
  
}
//--------------------------------------------------------------
void encode(int digit) {
  // fix changed from boolean to integer djn 13 apr 2018
  // encode key presses for decode
  if (offset < 100001) {
    code = code + (offset * digit); // add digit to code
    offset = offset * 10;
  }
  else {
    code=0;
  }
}
//--------------------------------------------------------------

//--------------------------------------------------------------
void printStats(String text) {
  
  if (debug  == 1) { 
  Serial.println("");
  Serial.print(text);
  Serial.print(" ");
  Serial.print(millis());
  Serial.print(" - redWire = ");
  Serial.print(redState);
  Serial.print(" blackWire = ");
  Serial.print(blackState);
  Serial.print(" currentKeyState= ");
  Serial.print(currentKeyState);
  Serial.print(" nextKeyState = ");
  Serial.print(nextKeyState);
  Serial.print(" PgmState = ");
  Serial.println(currentPgmState);
  }
}
//--------------------------------------------------------------
void setup() {
  Startup(); 
  setTimer(0);
  currentPgmState = stateIdle;
}
//--------------------------------------------------------------
void loop(){
  
  readContactState(); // read state of paddle contacts
  setKeyState();      // set key state based on state of contacts
  
  
    switch (currentPgmState) {
      
    case  stateIdle:
          // Turn LED off
          digitalWrite(ledPin, LOW);
          toneSent = 0; // indicate that digit not sent
          nextState();
          checkSpaceTimer();       // check if letter or word space
          break;
    
    case  stateDit: 
          printStats("stateDit");
          setTimer(ditTime);
          dit();
          currentPgmState = stateSending;
          break;
    
    case  stateDah:
         printStats("stateDah");
         setTimer(dahTime);
         dah();
         currentPgmState = stateSending;
         break;
         
    case  stateDitDah:
         printStats("stateDitDah");
         if (lastDigit == 1) {
           currentPgmState = stateDah;
           }
         else {
           currentPgmState = stateDit;
           }
         break;
         
    case  stateSending:
            printStats("stateSending");
           // wait for tone to end  
          if( millis() > timer )  {
            setTimer(ditTime);  // add one dit delay after tone ends
            currentPgmState = stateWait;
            spaceTimer = millis(); // set start time for space & word
            toneSent = 1; // indicate that digit sent
            letterSent = 1;
            wordSent = 1;
            lineSent = 1; // control line feeds
          }        
          break;
          
    case  stateWait: 
          // this post digit wait
          if( millis() > timer )  {
            printStats("stateWait ");
            currentPgmState = stateIdle;
            
          }        
          break;
    
    default: letter = "UNKNOWN";
            printStats("state Unknown");
          
    }
  
}

