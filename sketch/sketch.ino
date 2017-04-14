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
#define BTN_INTERVAL 25 // button check interval in ms

// upper motor driver
#define UM_STEP 51
#define UM_DIR 47 // gray -> blue
#define UM_EN 27 // needs to be low

// lower motor driver
#define LM_STEP 23  // TODO document cable colors
#define LM_DIR 25  // 27 is enable

// how much to increment or decrement (steps)
#define INC_AMT 5

/* TODO
motor speeds probably aren't going to be the same
we will need to characterize the differences in turn rates
also, this is probably where the encoder would be most useful

upper driver was set to all defaults (all switches up)
changing sw6 to down seemed to make it smooth but also I was stepping too fast
*/


/* END OF DEFINITIONS */
unsigned long run_start;

int run_speed = 1300; // steps per second
int accel = 800; // steps per second
unsigned long target_position = 128000; // position to move to when user presses button 
bool running = false; // are we running right now?
float multiplier = .25; //.4905; // lower motor targets
int align_offset_steps = 5; // how much to adjust up or down in steps for lower.  for visual alignment
Bounce increase = Bounce();
Bounce decrease = Bounce();
Bounce user = Bounce();

AccelStepper upper(AccelStepper::DRIVER, UM_STEP, UM_DIR);
AccelStepper lower(AccelStepper::DRIVER, LM_STEP, LM_DIR);

MultiStepper steppers;

void setup() {
  //upper.setMaxSpeed(MAX_SPEED);
  //lower.setMaxSpeed(MAX_SPEED);

  // set and invert enable pin
  upper.setEnablePin(UM_EN);
  //upper.setPinsInverted(false, false, true);
  upper.setPinsInverted(true, false, true);
  lower.setPinsInverted(true, false, false);

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

  upper.setMaxSpeed(run_speed);
  upper.setAcceleration(accel);
  lower.setMaxSpeed(run_speed * multiplier);
  lower.setAcceleration(accel * multiplier);

  steppers.addStepper(upper);
  steppers.addStepper(lower);

  //upper.moveTo(4000);
  //lower.moveTo(1940);
  /*
  long positions[2];
  positions[0] = target_position;
  positions[1] = target_position * multiplier;
  steppers.moveTo(positions);
  */
}

void loop() {
  user.update();
  increase.update();
  decrease.update();

  //steppers.run();
  if ((upper.isRunning() & lower.isRunning()) == false)
    running = false; 

  if (user.read() == LOW && running == false) {
    long positions[2];
    upper.setCurrentPosition(0);
    lower.setCurrentPosition(0);
    positions[0] = target_position;
    positions[1] = target_position * multiplier;
    steppers.moveTo(positions);
    running = true;
    Serial.println("run");
  }

  if (running == false) {
    if ((increase.read() == LOW || decrease.read() == LOW)) {
      int offset = 0;
      if (increase.read() == LOW) {
        offset = align_offset_steps;
      }
      else if (decrease.read() == LOW) {
        offset = align_offset_steps * -1;
      }
      lower.moveTo(lower.currentPosition() + offset);
      //Serial.print("Offsetting lower: ");
      //Serial.println(offset);
      lower.runToPosition();
      //Serial.println("done!");
    }
  }

  upper.run();
  lower.run();
}
