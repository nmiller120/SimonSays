
#include <PinChangeInt.h> // class provides an API for using interrupts assigned to I/O pins 

const int buzzer = 9; // output pin number of the buzzer
const int note[5] = {440, 494, 554, 587, 659}; //freqency of the notes that sound when the buzzer is pressed
const int led[4] = {2, 3, 4, 5}; // output pin numbers of the LEDs
const int input[4] = {12, 8, 7, 6}; // Button pins, assumed buttons go from left to right 
int solutionArray[128];  // stores the array of colors that were displayed to the user or the solution
int lvl = 0; // keeps track of the number of succsesses  
volatile int inputRecieved = -1; // used by the interrupt handlers to store what button was pressed by the user
int userStroke = 0; // the next value in the sequence the user has to guess this round
bool simons_turn = true; // flag to determine whose turn it is, user or simon
unsigned long lastInterruptTime = 0; // used to keep track of when a button was last pressed
int beep_interval = 200; // controls the speed of the game
int lives = 3; // how many lives the user is allowed
bool nextLevel = true; // flag to indicate the user beat the last level
int bounce_time = 200; // used to reject an input that bounced 

void lightColor(int select, int interval) {
  // lights the specified color, intended to be called during simons turn to show the current expected sequence. 
  // LED and tone are activated for the given interval then deactivated for the given interval
  if (select < 4) {
    digitalWrite(led[select], HIGH);
    tone(buzzer, note[select]);
    delay(interval);
    digitalWrite(led[select], LOW);
    noTone(buzzer);
    delay(interval);
    }

  // if we attempt to show a value that is greater than or equal to 4 we light all LEDs and play the 5th tone in the scale. 
  // This is intended to be used when showing the final score as the leds shine in groupings of 5 to make it easier to see what 
  // score you got.
  
  else {
    for (int i = 0; i < 4; i ++) {digitalWrite(led[i], HIGH);}
    tone(buzzer, note[4]);
    delay(interval);
    for (int i = 0; i < 4; i ++) {digitalWrite(led[i], LOW);}
    noTone(buzzer);
    delay(interval);
    }

}

void buttonPressFeedback(int feedback){
  // function provides a visual and auditory cue to the success of the user action

  // feedback equals three means we guessed wrong but we still have 2 lives, blink leds and sound an ugly tone 3 times
  if (feedback == -3){
    for (int i = 0; i < 4; i ++) {digitalWrite(led[i], HIGH);}
    tone(buzzer, 40); delay(66); noTone(buzzer); delay(66);
    tone(buzzer, 40); delay(66); noTone(buzzer); delay(66);
    tone(buzzer, 40); delay(66); noTone(buzzer); delay(66);
    for (int i = 0; i < 4; i ++) {digitalWrite(led[i], LOW);} 
    }

  // feedback equals two means we guessed wrong but we still have 1 life, blink leds and sound an ugly tone 2 times
  else if (feedback == -2){
    for (int i = 0; i < 4; i ++) {digitalWrite(led[i], HIGH);}
    tone(buzzer, 40); delay(150); noTone(buzzer); delay(50);
    tone(buzzer, 40); delay(150); noTone(buzzer); delay(50);
    for (int i = 0; i < 4; i ++) {digitalWrite(led[i], LOW);}
    }

  // feedback equals -1 means we lost
  else if (feedback == -1){
    int level = lvl;
    int lightSpeed = 50;

    // blinks once for each level we passed successfully, shown in groupings of 5
    for (int i = 0; i < level; i++) {
      lightColor(i%5, lightSpeed);
      if (i%5 == 4) {delay(250);
      }
      }

    // pauses for a second to let the user prepare for next game
    delay(1000);
  }

  // any other feedback value means we get to continue playing. The device sounds a note and flashes the color of the button 
  // you pressed
  else {
    digitalWrite(led[feedback], HIGH);
    tone(buzzer, note[feedback]);
    delay(beep_interval);
    digitalWrite(led[feedback], LOW);
    noTone(buzzer);
    }  
  }

int inputHandler(){
  // function determines whether the user entered the correct color or not

  // store the value of inputRecieved in a local var, and clear inputRecieved
  int color = inputRecieved;
  inputRecieved = -1;

  // determine what value we are expected based of of what values the user has guessed so far
  int expecting = solutionArray[userStroke];

  // if the guess was right...
  if (color == expecting) { 

    // ...and this was the last element to be guessed this round
    if (userStroke == lvl - 1) {

      // clear the current stroke counter and signal to the calling function that we be simon this round
      userStroke = 0;
      return 1;
      }

    // ...and we still have more elements to guess this round
    else {
      // change the expected value to the next element in the sequence and signal to the calling function that the user guessed 
      // correctly
      userStroke++;
      return 0; 
      }
    }

   // if the guess was wrong, signal to the calling function that the guess was incorrect
    else if (color != expecting){return -1;}      
  }
  
void buttonPressedInterrupt0(){
  // button press interrupt, function debounces the input and changes the value of inputRecieved to 0 
  unsigned long currentInterruptTime = millis();
  if (currentInterruptTime - lastInterruptTime < bounce_time) {}
  else {
  lastInterruptTime = millis();
  inputRecieved = 0;
  }
}

void buttonPressedInterrupt1(){
  // button press interrupt, function debounces the input and changes the value of inputRecieved to 1
  unsigned long currentInterruptTime = millis();
  if (currentInterruptTime - lastInterruptTime < bounce_time) {}
  else {
  lastInterruptTime = millis();
  inputRecieved = 1;
  }
  }
  
void buttonPressedInterrupt2(){
  // button press interrupt, function debounces the input and changes the value of inputRecieved to 2
  unsigned long currentInterruptTime = millis();
  if (currentInterruptTime - lastInterruptTime < bounce_time) {}
  else {
  lastInterruptTime = millis();
  inputRecieved = 2;
  }
  }
  
void buttonPressedInterrupt3(){
  // button press interrupt, function debounces the input and changes the value of inputRecieved to 3
  unsigned long  currentInterruptTime = millis();
  if (currentInterruptTime - lastInterruptTime < bounce_time) {}
  else {
  lastInterruptTime = millis();
  inputRecieved = 3;
  }
  }


void setup(){ 
  // runs at startup. Function initializes I/O pins, seeds our random number generator and attaches pin change interrupts on the 
  // input buttons

  pinMode(buzzer, OUTPUT);
  for (int i = 0; i<4; i++) {
    pinMode(led[i], OUTPUT);
    digitalWrite(led[i], LOW); 
    pinMode(input[i], INPUT_PULLUP);  
    }
  randomSeed(analogRead(0));
  attachPinChangeInterrupt(input[0], buttonPressedInterrupt0, FALLING ); 
  attachPinChangeInterrupt(input[1], buttonPressedInterrupt1, FALLING ); 
  attachPinChangeInterrupt(input[2], buttonPressedInterrupt2, FALLING ); 
  attachPinChangeInterrupt(input[3], buttonPressedInterrupt3, FALLING ); 
}

void loop(){
// main loop
  
  // on simons turn:
  if (simons_turn){
    // add a random value to the solution array
    solutionArray[lvl] = random(0,4);

    // if the user passed this level increment the current level
    if (nextLevel) {lvl++;}

    // display the solution sequence to the user
    for (int i = 0; i < lvl; i++) {
      lightColor(solutionArray[i], beep_interval); 
      }

      // clear the inputRecieved variable, start the next level and pass turn
     inputRecieved = -1;
     nextLevel = true;
     simons_turn = false;
    }

  // on users turn:
  else if (!simons_turn) {  

    // check to see if a button was selected yet
    if (inputRecieved != -1){ 

      // store the button recieved to display to the user later, this value gets overwritten if the value turns out to be wrong
      int userFeedback = inputRecieved;

      // check if the button pressed was correct
      int result = inputHandler();

      // Result equals 1 means we be simon this round and our turn is done
      if (result == 1) {
        simons_turn = true;
        }

      // result equals -1 means we did not guess the next value in the sequence correctly 
      else if (result == -1) {

        // remove one life
        lives -= 1;

        // if we are out of lives
        if (lives == 0) {
          
          // show final score
          userFeedback = -1;

          // reset lives
          lives = 3;

          // reset userStroke count
          userStroke = 0;

          // reset the lvl count
          lvl = 0;

          // let simon give us a new puzzle
          simons_turn = true;
          }  


        // if we have more lives
        else if (lives > 0) {

          // sound a buzzer, throw a flag saying we aren't ready to advance to the next level and pass turn.
          userFeedback = -1*lives-1;
          simons_turn = true;
          nextLevel = false;
          }
      }
      // show user the outcome of the pressed button, could just display what button was pressed, could show an error, or could 
      // show final score depending on the value of 
      // userFeedback variable
      buttonPressFeedback(userFeedback); 

      // if its simon's turn give the user a second to prepare to watch the next puzzle
      if (simons_turn) {delay(800);} 
      }
  }
}
