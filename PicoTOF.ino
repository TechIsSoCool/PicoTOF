/**
 * Raspberry Pi Pico, VL53L1X Time of flight sensor, SSD1306 display for Arduino IDE
 * @file: PicoTOF.ino
 * @author TechIsSoCool.com
 * @license MIT
 * @version 1.0
 * @date 2022-07-15
 * @ref https://techissocool.com/pi-pico-tof-arduino
 */
 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L1X.h>

//Button connected to pin GP22 (physical pin 29) with external pull up resistor to 3.3V, button to GND
#define BTN_TOGGLE_DISPLAY 22
#define DEBOUNCE_DELAY 100
#define BTN_ACTIVE LOW
enum DisplayMode {BOTH, MM, IMPERIAL};

// OLED display width, in pixels
#define SCREEN_WIDTH 128 
// OLED display height, in pixels
#define SCREEN_HEIGHT 32 
// Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_RESET     -1 
//I2C Address of display
#define SCREEN_ADDRESS 0x3C

//Timing budget for TOF sensor (Affects max distance.)
//Valid timing budgets: 15, 20, 33, 50, 100, 200, 500 (in msec)
//See section 2.5.2 of datasheet: https://cdn-learn.adafruit.com/assets/assets/000/105/859/original/vl53l1x.pdf
#define TIMING_BUDGET 50

//Globals
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //OLED Display
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(); //TOF Sensor

/**
 * Initializes Serial, display, and TOF sensor
 */
void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Starting setup()");
  pinMode(BTN_TOGGLE_DISPLAY, INPUT); //Use INPUT_PULLUP if no external resistor
  initDisplay();
  initTOFSensor();
}

/*
 * Checks for a valid distance reading, displays it in the current display mode.
 */
void loop() {
   int16_t distance;
   
   distance = readDistance();
   if(distance == -1) {
    //error in reading
    displayError();
   }
   else if(distance != 0){
    //valid reading
    displayDistance(distance, getDisplayMode());
   }
   //If distance == 0 reading was not ready, just loop
}

/**
 * Displays the distance in the current displayMode
 * @param distance in mm
 */
void displayDistance(int16_t distance, enum DisplayMode displayMode) {
  display.clearDisplay();
  display.setTextSize(displayMode == BOTH ? 1 : 2);
  
  if(displayMode == MM) {
    display.setCursor(20,15);             
    display.println(String(String(distance) + "mm"));
    display.display();
    return;
  }
  
  int16_t ft = (distance/25.4) / 12;
  int16_t in = (int)(distance/25.4) % 12;
  String inStr = in < 10 ? String(' ') + String(in) : String(in); //add leading space to single digit inches
  
  if(displayMode == BOTH){  
    display.setCursor(20,0);             
    display.println(String(String(distance) + "mm"));
    display.setCursor(20,SCREEN_HEIGHT-12);
    display.println(String(ft) + "ft. " + inStr + "in.");
  }
  else {
    //displayMode == IMPERIAL
    display.setCursor(0,15);             
    display.println(String(ft) + "ft. " + inStr + "in.");
  }
  display.display();
 }

/*
 * Displays error message when error received from sensor
 */
void displayError() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0,15);
    display.println("Sensor Error");
    display.display();
}

/**
 * Checks if a new reading is available from the sensor
 * @return distance in mm, 0 for no reading available, -1 for error 
 */
int16_t readDistance() {
  int16_t distance = 0;
  if (vl53.dataReady()) {
    // new measurement for the taking!
    distance = vl53.distance();
    if (distance == -1) {
      // something went wrong!
      Serial.print(F("Couldn't get distance: "));
      Serial.println(vl53.vl_status);
    }
    else {
      // data is read out, time for another reading!
      vl53.clearInterrupt();  
    }
  }
    return distance;
}

/*
 * Cycles to next display mode on button press
 * @return enum DisplayMode - The current display mode
 */
enum DisplayMode getDisplayMode() {
  static DisplayMode displayMode = BOTH; //Initialize to BOTH
  
  //Read button state
  if(digitalRead(BTN_TOGGLE_DISPLAY) == BTN_ACTIVE) {
    //wait for debounce and see if it's still active
    delay(DEBOUNCE_DELAY);
    if(digitalRead(BTN_TOGGLE_DISPLAY) == BTN_ACTIVE) {
      //Set displayMode to next mode (BOTH -> MM -> IMPERIAL -> BOTH ...)
      displayMode = displayMode == BOTH ? MM :
                    displayMode == MM ? IMPERIAL : BOTH;
    } //end inner if
  } //end outer if
  
  return displayMode;
}

/**
 * Initializes display and sets initial settings. Hangs program on error. 
 */
void initDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 initialization failed");
    //Stop
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(1);            
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,10);             
  display.println("Time of Flight Distance Sensor");
  display.display();
}

/**
 * Initializes TOF Sensor and sets initial settings. Hangs program on error. 
 */
void initTOFSensor() {
  if (! vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    //Stop
    while(1);
  }
  Serial.println("VL53L1X sensor OK!");

  Serial.print("Sensor ID: 0x");
  Serial.println(vl53.sensorID(), HEX);

  if (! vl53.startRanging()) {
    Serial.print("Couldn't start ranging: ");
    Serial.println(vl53.vl_status);
    //Stop
    while(1);
  }
  Serial.println("Ranging started");

  vl53.setTimingBudget(TIMING_BUDGET);
  Serial.print("Timing budget (ms): ");
  Serial.println(vl53.getTimingBudget());
}
