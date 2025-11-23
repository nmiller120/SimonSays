// ----- SIMON GAME (NO INTERRUPTS, POLLED BUTTONS) -----

const int buzzer = 9;
const int note[5] = {440, 494, 554, 587, 659};
const int led[4] = {2, 3, 4, 5};
const int button[4] = {12, 8, 7, 6};

int solutionArray[128];
int lvl = 0;
int userStroke = 0;

bool simons_turn = true;
bool nextLevel = true;

unsigned long lastPressTime[4] = {0,0,0,0};
int debounce = 250;   // debounce in ms

int lives = 3;
int beep_interval = 200;


// ---------------------------------------------------------------------
// Helper: light a color (used by Simon demo)
// ---------------------------------------------------------------------
void lightColor(int select, int interval) {
  if (select < 4) {
    digitalWrite(led[select], HIGH);
    tone(buzzer, note[select]);
    delay(interval);
    digitalWrite(led[select], LOW);
    noTone(buzzer);
    delay(interval);
  }
  else {
    for (int i = 0; i < 4; i++) digitalWrite(led[i], HIGH);
    tone(buzzer, note[4]);
    delay(interval);
    for (int i = 0; i < 4; i++) digitalWrite(led[i], LOW);
    noTone(buzzer);
    delay(interval);
  }
}


// ---------------------------------------------------------------------
// Helper: show good/bad button feedback
// ---------------------------------------------------------------------
void buttonPressFeedback(int feedback) {

  // -3 = wrong, 2 lives left
  if (feedback == -3) {
    for (int i = 0; i < 4; i++) digitalWrite(led[i], HIGH);
    for (int k = 0; k < 2; k++) {
      tone(buzzer, 40); delay(66); noTone(buzzer); delay(66);
    }
    for (int i = 0; i < 4; i++) digitalWrite(led[i], LOW);
  }

  // -2 = wrong, 1 life left
  else if (feedback == -2) {
    for (int i = 0; i < 4; i++) digitalWrite(led[i], HIGH);
    for (int k = 0; k < 3; k++) {
      tone(buzzer, 40); delay(150); noTone(buzzer); delay(50);
    }
    for (int i = 0; i < 4; i++) digitalWrite(led[i], LOW);
  }

  // -1 = game over → show score
  else if (feedback == -1) {
    for (int i = 0; i < 4; i++) digitalWrite(led[i], HIGH);
    for (int k = 0; k < 4
    ; k++) {
      tone(buzzer, 40); delay(150); noTone(buzzer); delay(50);
    }
    for (int i = 0; i < 4; i++) digitalWrite(led[i], LOW);

    delay(1000);
  }

  // Otherwise: show the button pressed
  else {
    digitalWrite(led[feedback], HIGH);
    tone(buzzer, note[feedback]);
    delay(beep_interval);
    digitalWrite(led[feedback], LOW);
    noTone(buzzer);
  }
}


// ---------------------------------------------------------------------
// Check if the user input matches expected
// ---------------------------------------------------------------------
int inputHandler(int color) {

  int expecting = solutionArray[userStroke];

  if (color == expecting) {

    if (userStroke == lvl - 1) {
      userStroke = 0;
      return 1;   // Level completed
    }
    else {
      userStroke++;
      return 0;   // Correct but continue playing
    }
  }

  // Wrong input
  return -1;
}


// ---------------------------------------------------------------------
// Poll for buttons (returns -1 if no button pressed)
// ---------------------------------------------------------------------
int getButtonPress() {
  for (int i = 0; i < 4; i++) {
    if (digitalRead(button[i]) == LOW) {   // active LOW with pullup
      unsigned long now = millis();
      if (now - lastPressTime[i] > debounce) {
        lastPressTime[i] = now;
        return i;
      }
    }
  }
  return -1;
}


// ---------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------
void setup() {
  pinMode(buzzer, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(led[i], OUTPUT);
    digitalWrite(led[i], LOW);

    pinMode(button[i], INPUT_PULLUP);
  }

  randomSeed(analogRead(A0));
}


// ---------------------------------------------------------------------
// Main Loop
// ---------------------------------------------------------------------
void loop() {

  // SIMON'S TURN
  if (simons_turn) {

    solutionArray[lvl] = random(0, 4);

    if (nextLevel) lvl++;

    for (int i = 0; i < lvl; i++)
      lightColor(solutionArray[i], beep_interval);

    nextLevel = true;
    simons_turn = false;
  }


  // USER'S TURN
  else {

    int pressed = getButtonPress();

    if (pressed != -1) {

      int userFeedback = pressed;
      int result = inputHandler(pressed);

      if (result == 1) {
        simons_turn = true;
      }
      else if (result == -1) {
        lives--;

        if (lives == 0) {
          userFeedback = -1;
          lives = 3;
          userStroke = 0;
          lvl = 0;
          simons_turn = true;
        }
        else {
          userFeedback = -1 * lives - 1;
          simons_turn = true;
          nextLevel = false;
        }
      }

      buttonPressFeedback(userFeedback);

      if (simons_turn) delay(800);
    }
  }
}
