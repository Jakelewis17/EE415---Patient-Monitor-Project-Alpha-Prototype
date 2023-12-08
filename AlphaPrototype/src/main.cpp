/*******************************************************************************
 * Programmers: Jake Lewis, Zachary Harrington, Nicholas Gerth, Matthew Stavig *                                                      
 * Class: EE415 - Product Design Management                                    *
 * Sponsoring Company: Philips                                                 *
 * Industry Mentor: Scott Schweizer                                            *
 * Faculty Mentor: Mohammad Torabi Konjin                                      *
 *                                                                             *
 *                          Patient Monitor Project                            *
 *                                                                             *
 * Date: 11/23/2023                                                            *
 * File: main.cpp                                                              *
 *                                                                             *
 * Description: A patient monitor measuring the three most important           *
 *              physilogical parameters: blood oxygen, ECG, and blood pressure *   
 *                                                                             *
 *                                                                             *
 ******************************************************************************/

#include "patient_monitor.h"

/* Global variables */
int what_press = 0;

/* Define rotary encoder button */
BfButton rotary_sw(BfButton::STANDALONE_DIGITAL, PinSW, true, LOW);

/* Invoke display objects from TFT library */
TFT_eSPI tft = TFT_eSPI();  
TFT_eSprite ecg = TFT_eSprite(&tft);
TFT_eSprite background = TFT_eSprite(&tft);
TFT_eSprite title = TFT_eSprite(&tft);
TFT_eSprite digit_box = TFT_eSprite(&tft);
TFT_eSprite hr_display = TFT_eSprite(&tft);
TFT_eSprite spo2_display = TFT_eSprite(&tft);
TFT_eSprite bp_display = TFT_eSprite(&tft);

/* Invoke MAX30102 Object */
PulseOximeter pox;

MAX30105 particleSensor;

/* Define interrupt variables */
volatile boolean TurnDetected; //need volatile for interrupts
volatile boolean SWDetected; 

/*  Main Variable Definitions */
int PreviousCLK;
int PreviousDATA;
int LEDBrightness = 0;
int count = 0;
int displayState = 0;
int ecg_position = 0;
int title_bg_color = 0;
int bg_color = 0;
int spo2_title_bg_color = 0;
uint32_t tsLastReport = 0;


/* Define interrupt routines for KY-040 rotary encoder */
void rotarydetect()
{
  //interrupt routine runs if CLK pin changes state
  TurnDetected = true;
}

/* Encoder press handler */
void swHandler(BfButton* btn, BfButton::press_pattern_t pattern)
{
  //update what_press variable depending on type of input from button
  switch(pattern)
  {
    case BfButton::SINGLE_PRESS:
    Serial.println("single press");
    what_press = 1;
    break;

    case BfButton::DOUBLE_PRESS:
    Serial.println("Double Press");
    what_press = 2;
    break;

    case BfButton::LONG_PRESS:
    Serial.println("Long Press");
    what_press = 3;
    break;
  }
}


/* Setup Function*/
void setup() {
  //set up I2C
  
  Serial.begin(115200);
  Serial.println("Hello from startup");

  //get initial states of variables
  PreviousCLK = digitalRead(PinCLK);
  PreviousDATA = digitalRead(PinDT);

  //setup pins 
  pinMode(PinSW, INPUT_PULLUP);
  pinMode(PinCLK, INPUT);
  pinMode(PinDT, INPUT);
  pinMode(PIN_GPIO, OUTPUT);
  pinMode(PIN_SPO2_RST, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(valveSwitch, OUTPUT);

  //set up interrupt
  attachInterrupt(PinCLK, rotarydetect, CHANGE); 

  //Encoder button setup
  rotary_sw.onPress(swHandler)
  .onDoublePress(swHandler)
  .onPressFor(swHandler, 1000);

  /* TFT display setup */
  tft.init();
  tft.setRotation(14);
  tft.fillScreen(TFT_BLACK);
  tft.invertDisplay( true ); //invert display colors for proper displaying
  tft.setSwapBytes(true);
  tft.setTextWrap(true, true);


  /* TFT sprites setup*/
  ecg.createSprite(240,107); //create sprite for ecg waveform
  ecg.setSwapBytes(true);

  background.createSprite(220,220); //create sprite for background
  background.setSwapBytes(true);

  title.createSprite(120,90); //create sprite for physilogical parameter titles
  title.setTextColor(TFT_BLACK, TFT_WHITE);
  title.setFreeFont(&Dialog_plain_35); //custom font

  digit_box.createSprite(35,50); //create sprite for physilogical parameter titles
  digit_box.setTextColor(TFT_BLACK, TFT_WHITE);
  digit_box.setFreeFont(&Dialog_plain_35); //custom font

  hr_display.createSprite(93, 47);
  hr_display.setSwapBytes(true);
  hr_display.setTextColor(TFT_WHITE, TFT_BLUE);

  spo2_display.createSprite(93, 47);
  spo2_display.setSwapBytes(true);
  spo2_display.setTextColor(TFT_WHITE, TFT_BLUE);

  bp_display.createSprite(150, 47);
  bp_display.setSwapBytes(true);
  bp_display.setTextColor(TFT_WHITE, TFT_BLUE);

  
  tft.fillScreen(TFT_WHITE); //fill the screen with white

  delay(500);

}

/* Infinite Loop Function */
void loop() {

  // Make sure to call update as fast as possible
  //pox.update();

if(TurnDetected) //check if rotary encoder detected an input
{
  TurnDetected = false;
  count++;
  
  //check if turn was to the left
  if(((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 0)) || ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 1)))
  {
    //turn to left detected

    //update display state variable
    if(displayState == 0)
    {
      displayState = 2;
    }
    else
    {
      displayState--;
    }


  }
  else if (((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 1)) || ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 0)))
  {
    //turn to right detected

     if(displayState == 2)
     {
       displayState = 0;
     }
     else
     {
       displayState++;
     }
  }

}

int bp_count = 0;
int bp_average = 0;
int bp_sum = 0;
//Serial.print((count % 3));
/* Check what parameter should be displayed and display it */
if((count % 3) == 0)
{
  /* ECG Parameter */

  //change background color
  if(bg_color != 0)
  {
    tft.fillScreen(TFT_WHITE);
    bg_color = 0;
  }
  
  //display ECG text
  title.fillSprite(TFT_WHITE);
  title.drawString("ECG", 45, 35);

  //display ecg waveform image
  ecg.pushImage(0,0,240,107,ecg_sig);
  ecg.pushSprite(ecg_position, 133, TFT_BLACK);
  
  //update position of image to make it move
  ecg_position--;
  if(ecg_position == -85)
  {
    ecg_position = 240;
  }

  //Create flashing behind text
  if((ecg_position % 10) == 0)
  {
    if(title_bg_color == 0)
    {
      title.setTextColor(TFT_BLACK, TFT_WHITE);
      title_bg_color = 1;
    }
    else
    {
      title.setTextColor(TFT_BLACK, TFT_SKYBLUE);
      title_bg_color = 0;
    }
  }

  //push ECG sprite
  ecg.pushImage(0,0,240,107,ecg_sig);
  ecg.pushSprite(ecg_position,133, TFT_WHITE);

  title.pushSprite(35, 30);

  
  int bp_avg[200];
  
  

  for(int i = 0; i < 100; i++)
  {
    int bp_reading = analogRead(PINBP);
    bp_sum = bp_sum + bp_reading;
  }
  bp_sum = bp_sum / 100;
  
  Serial.print(bp_sum);
  Serial.print("\n");
  bp_count++;

}
else if((count % 3) == 1)
{
  /* Blood Pressure Paramter */

  //change background color
  if(bg_color != 1)
  {
    tft.fillScreen(TFT_BLACK);
    bg_color = 1;
  }

  //display BP text
  title.fillSprite(TFT_PINK);
  title.setTextColor(TFT_BLACK, TFT_PINK);
  title.drawString("BP", 63, 50);
  
  
  //display animation
  tft.pushImage(15,25, 210, 182, hb_frame0);
  title.pushSprite(35, 40, TFT_PINK);
  rotary_sw.read();
  delay(40);
  tft.pushImage(15,25, 210, 182, hb_frame1);
  title.pushSprite(35, 40, TFT_PINK);
  rotary_sw.read();
  delay(40);
  tft.pushImage(15,25, 210, 182, hb_frame2);
  title.pushSprite(35, 40, TFT_PINK);
  rotary_sw.read();
  delay(40);
  tft.pushImage(15,25, 210, 182, hb_frame3);
  title.pushSprite(35, 40, TFT_PINK);
  rotary_sw.read();
  delay(40);
  tft.pushImage(15,25, 210, 182, hb_frame4);
  title.pushSprite(35, 40, TFT_PINK);
  rotary_sw.read();
  
}
else  
{
  /* SPO2 Parameter */

  //change background color
  if(bg_color != 2)
  {
    tft.fillScreen(TFT_RED);
    bg_color = 2;
  }

  //flashing behind text
  if(spo2_title_bg_color == 0)
  {
    title.setTextColor(TFT_WHITE, TFT_BLACK);
    spo2_title_bg_color = 1;
  }
  else
  {
    title.setTextColor(TFT_WHITE, TFT_RED);
    spo2_title_bg_color = 0;
  }

  //display BP text
  title.fillSprite(TFT_RED);
  title.drawString("SPO2", 0, 0);

  //display animation
  tft.pushImage(0,0, 240, 133, SPO2_frame0);
  title.pushSprite(72, 170);
  rotary_sw.read();
  delay(40);
  tft.pushImage(0,0, 240, 133, SPO2_frame1);
  title.pushSprite(72, 170, TFT_RED);
  rotary_sw.read();
  delay(40);
  tft.pushImage(0,0, 240, 133, SPO2_frame2);
  title.pushSprite(72, 170, TFT_RED);
  rotary_sw.read();
  delay(40);
  tft.pushImage(0,0, 240, 133, SPO2_frame3);
  title.pushSprite(72, 170, TFT_RED);
  rotary_sw.read();
  delay(40);
  tft.pushImage(0,0, 240, 133, SPO2_frame4);
  title.pushSprite(72, 170, TFT_RED);
  rotary_sw.read();

}

  /* Detect a push of the rotary encoder */
  rotary_sw.read();

  if((what_press == 1) && ((count % 3) == 0))
  {
    //enter ecg data collection
    read_ecg();
  }
  else if ((what_press == 1) && ((count % 3) == 1))
  {
    //enter blood pressure data collection
    read_bp();
  }
  else if ((what_press == 1) && ((count % 3) == 2))
  {
    //enter SPO2 data collection
    read_spo2();
  }

  what_press = 0;
}

