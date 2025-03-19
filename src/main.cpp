/*********************************************************************************************************
* LILYGO T-Display-S3 KY-040 Rotary Encoder Module Project
*
* This sketch read the rotary encoder module and displays the value along with a slider on the built-in
* screen using the TFT_eWidget library. The counter value and slider position are updated in real-time.
*
* Description:
*  The code is based off of the slider example project from the TFT_eWidget library and modified to
*  work with the KY-040 rotary encoder module on a (non-touch) LilyGo T-Display-S3.
*  The rotary encoder moves the knob on the slider and updates the counter value.
*  Pressing the encoder button will reset the slider to the middle (50) and counter to 50.
*  On startup, the slider will sweep from 50 to 0, then to 100, and back to 50 within 1sec.
*
* Pin Connections:
*  - Module VCC -> 3.3V
*  - Module GND -> GND
*  - Module SW  -> GPIO01 (input)
*  - Module DT  -> GPIO02 (input)
*  - Module CLK -> GPIO03 (input)
*
* KY-040 Specifications:
*  - Protocol: Digital
*  - Operating Voltage: 3.3V to 5V
*  - Pulses per Revolution: 20
*  - Output: 2-bit Gray Code
*  - Mechanical Angle: 360° Continuous
*  - Built-in Switch: Yes (Push-to-Operate)
**********************************************************************************************************/


/*************************************************************
******************* INCLUDES & DEFINITIONS *******************
**************************************************************/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>

#include "FS.h"
#include "Free_Fonts.h" // include the header file attached to this sketch

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite knob = TFT_eSprite(&tft); // sprite for the slide knob

SliderWidget slider1 = SliderWidget(&tft, &knob); // slider widget

// Rotary Encoder Pins
#define SW_PIN 1
#define DT_PIN 2
#define CLK_PIN 3

// Variables for rotary encoder
int counter = 50; // initial slider position
int currentStateCLK;
int lastStateCLK;
bool btnPressed = false;
int encoderSpeed = 2; // amount to increment/decrement by each rotation click


/*************************************************************
********************** HELPER FUNCTIONS **********************
**************************************************************/

// Function to draw the rotary counter value
void drawRotaryCounter(int value) {
  tft.setFreeFont(FF17);                  // set the font type & size
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color and background

  // Draw counter value text
  String countLabel = "Rotary Count:  "; // label text (used double space here as a single space doesn't render for some reason)
  int countLabelWidth = tft.textWidth(countLabel);
  int countLabelX = (tft.width() - tft.textWidth("Rotary Count: 100")) / 2; // center full length line
  int countLabelY = (tft.height() / 2 - 10); // adjust to move up or down

  // Clear only the area for the counter value (3 characters wide)
  tft.fillRect(countLabelX + countLabelWidth, countLabelY, tft.textWidth("100"), 20, TFT_BLACK);

  // Draw the counter value
  String counterText = String(value);
  tft.drawString(counterText, countLabelX + countLabelWidth, countLabelY);
}

// Function to draw the static text on the screen
void drawText() {
  tft.setFreeFont(FF17);                  // set the font type & size (see the Free_Fonts.h file for FF number)
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color and background

  // Draw heading text
  String title = "KY040 Rotary Encoder Module";
  int titleWidth = tft.textWidth(title);       // get the width of the text
  int titleX = (tft.width() - titleWidth) / 2; // center the text horizontally
  tft.drawString(title, titleX, 10);           // draw the text at (titleX, 10)

  // Draw "Rotary Count:" text
  String countLabel = "Rotary Count:  "; // label text (used double space here as a single space doesn't render for some reason)
  int countLabelWidth = tft.textWidth(countLabel);
  int countLabelX = (tft.width() - tft.textWidth("Rotary Count: 100")) / 2; // center full length line
  int countLabelY = (tft.height() / 2) - 10; // adjust last value to move up or down
  tft.drawString(countLabel, countLabelX, countLabelY);

  // Draw slider text
  int sliderY = tft.height() - 35; // Y position of the slider (adjust to move up or down)
  int textY = sliderY - 20;        // adjust to move up or down

  // Draw "0" on the left side
  tft.drawString("0", 21, textY); // adjust middle value to move left or right

  // Draw "50" in the middle
  String midText = "50";
  int midTextWidth = tft.textWidth(midText);
  int midTextX = (tft.width() - midTextWidth) / 2 - 2; // adjust last value to move left or right
  tft.drawString(midText, midTextX, textY);

  // Draw "100" on the right side
  String rightText = "100";
  int rightTextWidth = tft.textWidth(rightText);
  int rightTextX = tft.width() - rightTextWidth - 14; // adjust to move left or right
  tft.drawString(rightText, rightTextX, textY);
}

// Function to sweep the knob from 50 to 0, then to 100, and back to 50
void sweepKnob() {
  int stepDelay = 5; // delay in ms per step
  int stepSize = 1;  // how much to move per step

  // Move from 50 to 0
  for (int i = 50; i >= 0; i -= stepSize) {
    slider1.setSliderPosition(i);
    drawRotaryCounter(i); // update the counter value
    delay(stepDelay);
  }

  // Move from 0 to 100
  for (int i = 0; i <= 100; i += stepSize) {
    slider1.setSliderPosition(i);
    drawRotaryCounter(i); // update the counter value
    delay(stepDelay);
  }

  // Move from 100 back to 50
  for (int i = 100; i >= 50; i -= stepSize) {
    slider1.setSliderPosition(i);
    drawRotaryCounter(i); // update the counter value
    delay(stepDelay);
  }
}


/*************************************************************
*********************** MAIN FUNCTIONS ***********************
**************************************************************/

// SETUP
void setup() {
  tft.begin();
  tft.setRotation(1); // landscape mode
  tft.fillScreen(TFT_BLACK);

  // Initialize rotary encoder pins
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP);

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK_PIN);

  // Create a parameter set for the slider
  slider_t param;


  // Slider slot parameters
  param.slotWidth = 18;          // note: ends of slot will be rounded and anti-aliased - doubled width make it thicker
  param.slotLength = 300;        // length includes rounded ends
  param.slotColor = TFT_BLUE;    // slot colour
  param.slotBgColor = TFT_BLACK; // slot background colour for anti-aliasing
  param.orientation = H_SLIDER;  // sets it "true" for horizontal

  // Slider control knob parameters (smooth rounded rectangle)
  param.knobWidth = 15;          // always along x axis
  param.knobHeight = 25;         // always along y axis
  param.knobRadius = 5;          // corner radius
  param.knobColor = TFT_WHITE;   // anti-aliased with slot backgound colour
  param.knobLineColor = TFT_RED; // colour of marker line (set to same as knobColor for no line)

  // Slider range and movement speed
  param.sliderLT = 0;       // left side for horizontal, top for vertical slider
  param.sliderRB = 100;     // right side for horizontal, bottom for vertical slider
  param.startPosition = 50; // start position for control knob

  // Calculate the position to center the slider at the bottom middle
  int screenWidth = 320;
  int screenHeight = 170;
  int sliderX = (screenWidth - param.slotLength) / 2 - 3; // center horizontally
  int sliderY = screenHeight - 35;                        // adjust to move up or down
  
  // Start position for control knob
  param.startPosition = counter;

  // Create slider using parameters and plot at calculated position
  slider1.drawSlider(sliderX, sliderY, param);

  // Show border rectangle (1 pixel outside slider working area)
  /*
  int16_t x, y;
  uint16_t w, h;
  slider1.getBoundingRect(&x, &y, &w, &h); // update x, y, width, height with border
  tft.drawRect(x, y, w, h, TFT_DARKGREY);  // draw border
  */

  // Draw the static text on the screen
  drawText();

  // Draw the initial rotary counter value
  drawRotaryCounter(counter);

  // Sweep animation to ensure the knob is drawn
  sweepKnob();
}


// MAIN LOOP
void loop() {
  // Read the current state of CLK
  currentStateCLK = digitalRead(CLK_PIN);

  // If last and current state of CLK are different, then a pulse occurred
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    // If the DT state is different than the CLK state, the encoder is rotating clockwise
    if (digitalRead(DT_PIN) != currentStateCLK) {
      counter += encoderSpeed;          // increment counter based on set speed
      if (counter > 100) counter = 100; // limit counter to 100
    }
    else {
      counter -= encoderSpeed;      // decrement counter based on set speed
      if (counter < 0) counter = 0; // limit counter to 0
    }

    // Update slider position
    slider1.setSliderPosition(counter);

    // Update the rotary counter value on the screen
    drawRotaryCounter(counter);
  }

  // Save the last CLK state
  lastStateCLK = currentStateCLK;

  // Check if the button is pressed
  if (digitalRead(SW_PIN) == LOW && !btnPressed) {
    btnPressed = true;
    // Reset slider to the middle position
    counter = 50;
    slider1.setSliderPosition(counter);

    // Update the rotary counter value on the screen
    drawRotaryCounter(counter);
  }
  else if (digitalRead(SW_PIN) == HIGH && btnPressed) {
    btnPressed = false;
  }

  delay(1); // small delay to free up CPU cycles
}
