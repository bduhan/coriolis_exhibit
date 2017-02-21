/* TTU Museum exhibit code for "The Coriolis Effect"
 * 2017 opening
 * 
 * visitors press a button to initiate spinning of the two motors
 * observe (hopefully) stationary seeming image on video screen
 * drop balls into tube to observe trajectory
 *
 * NOTE!  this does not account for the two motors spinning at different rates
 * there is an encoder installed that may be necessary for this purpose
 *    Yumo E6B2-CWZ3E
 */
#include <Bounce2.h> // https://github.com/thomasfredericks/Bounce2
#include <AccelStepper.h> // http://www.airspayce.com/mikem/arduino/AccelStepper/
#include <MultiStepper.h>

// how long do we run the demonstration when a user button is pressed?  (in ms)
unsigned long run_time = 8000;

/* PIN DEFINITIONS */
#define BTN_INCREASE 35 // active LOW
#define BTN_DECREASE 37 // active LOW
#define BTN_USER 31 // unknown
#define LED_PIN 13 // not really used
#define BTN_INTERVAL 5 // button check interval in ms

// upper motor driver
#define UM_STEP 51
#define UM_DIR 47 // gray -> blue
#define UM_EN 27 // needs to be low

// lower motor driver
#define LM_STEP 23  // TODO document cable colors
#define LM_DIR 25  // 27 is enable

// how much to increment or decrement (steps)
#define INC_AMT 5

float MAX_SPEED = 1000;
float MIN_SPEED = 10;

/* TODO
motor speeds probably aren't going to be the same
we will need to characterize the differences in turn rates
also, this is probably where the encoder would be most useful
*/


/* END OF DEFINITIONS */
unsigned long run_start;

int run_speed = 100;
bool running = false;
Bounce increase = Bounce();
Bounce decrease = Bounce();
Bounce user = Bounce();

AccelStepper upper(AccelStepper::DRIVER, UM_STEP, UM_DIR);
AccelStepper lower(AccelStepper::DRIVER, LM_STEP, LM_DIR);

void setup() {
  upper.setMaxSpeed(MAX_SPEED);
  lower.setMaxSpeed(MAX_SPEED);

  // set and invert enable pin
  upper.setEnablePin(UM_EN);
  upper.setPinsInverted(false, false, true);

  // buttons
  pinMode(BTN_INCREASE, INPUT_PULLUP);
  increase.attach(BTN_INCREASE);
  increase.interval(BTN_INTERVAL); // in ms
  pinMode(BTN_DECREASE, INPUT_PULLUP);
  decrease.attach(BTN_DECREASE);
  decrease.interval(BTN_INTERVAL);
  pinMode(BTN_USER, INPUT_PULLUP);
  user.attach(BTN_USER);
  user.interval(BTN_INTERVAL);
  
  Serial.begin(9600);
  Serial.println("setup!");
  upper.setSpeed(0);
  upper.setAcceleration(10);
  lower.setSpeed(0);
  lower.setAcceleration(10);
}

void loop() {
  increase.update();
  decrease.update();
  user.update();

  if (running == true) {
    unsigned long rt = run_start + run_time;
    //Serial.println(rt);
    if (rt < millis()) {
      Serial.println("STOP!");
      running = false;
      run_start = 0;
      upper.setSpeed(0.0);
      lower.setSpeed(0.0);
      digitalWrite(LED_PIN, LOW);
    }
  } else {
    if (user.read() == LOW) {
      Serial.print("RUN speed: ");
      Serial.println(run_speed);
      running = true;
      run_start = millis();
      upper.setSpeed(run_speed);
      lower.setSpeed(run_speed);
    }
  }

  // increment/decrement speed from buttons
  if (increase.read() == LOW) {
    if (run_speed + INC_AMT < MAX_SPEED) {
      Serial.print("INcrease speed to: ");
      Serial.println(run_speed);
      run_speed += INC_AMT;
    } else {
      Serial.println("AT MAX SPEED");
    }
  }

  if (decrease.read() == LOW) {
    if (run_speed - INC_AMT > MIN_SPEED) {
      Serial.print("DEcrease speed to: ");
      Serial.println(run_speed);
      run_speed -= INC_AMT;
    } else {
      Serial.println("AT MIN SPEED");
    }
  }

  //upper.runSpeed();
  //lower.runSpeed();
  upper.runSpeed();
  lower.runSpeed();
}
