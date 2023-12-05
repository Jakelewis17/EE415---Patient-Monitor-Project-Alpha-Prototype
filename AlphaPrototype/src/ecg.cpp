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
 * File: ecg.cpp                                                               *
 *                                                                             *
 * Description: A patient monitor measuring the three most important           *
 *              physilogical parameters: blood oxygen, ECG, and blood pressure *   
 *                                                                             *
 *                                                                             *
 ******************************************************************************/

#include "patient_monitor.h"

extern int what_press;

/* Define rotary encoder button */
extern BfButton rotary_sw;

/* Invoke display objects from TFT library */
extern TFT_eSPI tft;  
extern TFT_eSprite ecg;
extern TFT_eSprite background;
extern TFT_eSprite title;
extern TFT_eSprite hr_display;
extern TFT_eSprite digit_box;

int BPM = 0;
int beat_old = 0;
float beats[15] = { 0 };  // Used to calculate average BPM
int beatIndex = 0;
float threshold = 130;  //Threshold at which BPM calculation occurs
boolean belowThreshold = true;

void read_ecg()
{
  what_press = 0;
  tft.fillScreen(TFT_BLACK);

  //display title
  title.fillSprite(TFT_BLACK);
  title.setTextColor(TFT_WHITE, TFT_BLACK);
  title.drawString("ECG", 0, 0);
  title.pushSprite(85, 30);

  //display instructions
  tft.drawString("Click once to start", 0, 80, 4);
  tft.drawString("Click twice to return", 0, 120, 4);

  for(;;) //infinite polling loop for switch input
  {
    rotary_sw.read();
    if(what_press == 1)
    {
      //single press - jump to ECG measurement function
      ecg_measurement();
      what_press = 0;
      tft.fillScreen(TFT_WHITE);
      break;
    }
    else if(what_press == 2)
    {
      //double press - go backto seleciton screen
      tft.fillScreen(TFT_WHITE);
      break;
    }
    
    what_press = 0;
  }
}



void ecg_measurement()
{
  tft.fillScreen(TFT_BLUE);
  int ecg_reading = 0;
  int ecg_counter = 0;
  int x2 = 0, y2 = 0;
  int ecg_spacing = 4;
  int xWriteIndex = 0; 
  int yWriteIndex = 0;
  int heartrate = 0, prev_hr = 0;
  int heartbeat_counter = 0, heartbeat_flag = 0;
  int initial_measurments = 0;

  int temp_reading[10] = {1500, 1400, 1500, 1200, 1100, 1900, 1300, 1500, 1500, 1500};
  int temp_index = 0;

  hr_display.setTextColor(TFT_WHITE, TFT_BLUE);

  //display heartrate
  title.fillSprite(TFT_BLUE);
  title.setTextColor(TFT_WHITE, TFT_BLUE);
  title.drawString("HR: ", 0, 0);
  title.pushSprite(30, 40);

  digit_box.setTextColor(TFT_BLACK, TFT_BLUE);
  digit_box.fillSprite(TFT_BLUE);

  //tft.setTextColor(TFT_WHITE, TFT_BLUE);

  for(;;) //infinite loop getting measurement
  {
    //get analog input and perfom binning
    ecg_reading = analogRead(PinECG);
    ecg_reading = ecg_reading / 17;

    //ecg_reading = ecg_reading % 220;
    //if(temp_index == 10)
    //{
     // temp_index = 0;
    //}
    //ecg_reading = temp_reading[temp_index] / 14;

    //reset screen once it gets to edge
    if(ecg_counter > (tft.width() / ecg_spacing))
    {
      ecg_counter = 0;
      tft.fillScreen(TFT_BLUE);
      title.pushSprite(30, 40);
    }

    //if counter reset, move x back to left side of screen
    if(ecg_counter == 0)
    {
      xWriteIndex = 0;
      yWriteIndex = tft.height() - ecg_reading;
    }

    //if not on left side of screen, update indicies and display signal
    if(ecg_counter > 0)
    {
      x2 = ecg_counter * ecg_spacing;
      y2 = tft.height() - ecg_reading + 30;
      tft.drawLine(xWriteIndex, yWriteIndex, x2, y2, TFT_WHITE);
      xWriteIndex = x2;
      yWriteIndex = y2;
    }

    ecg_counter++;

    //check for long press to go back to menu
    rotary_sw.read();
    if(what_press == 3)
    {
      //long press terminates the loop
      what_press = 0;
      tft.fillScreen(TFT_WHITE);
      break;
    }

    temp_index++;
    heartbeat_counter++;
    
    //detect heartbeat by checking if reading goes over a threshold
    if(ecg_reading > 130)
    {
      //detect heartbeat
      heartbeat_flag = 1;
      
    }

    //calculate heartbeat
    if(heartbeat_flag == 1)
    {
      prev_hr = heartrate;
      heartrate = 750 / heartbeat_counter;
      //heartrate = 1000 / heartbeat_counter;
      heartbeat_counter = 0;
      heartbeat_flag = 0;
    }

    //logic for displaying correctly
    //if((prev_hr >= 100) && (heartrate < 100))
   // {
      //hr_display.drawString(String(0), 100, 30, 7);
      //hr_display.fillSprite(TFT_BLUE);

      //fix extra digit issue
      //digit_box.pushSprite(160, 40);

    //}

    if(BPM < 100)
    {
      //digit_box.pushSprite(160, 40);
    }

    // BPM calculation check
    if (heartrate > threshold && belowThreshold == true)
    {
      calculateBPM();
      belowThreshold = false;
    }
    else if(heartrate < threshold)
    {
      belowThreshold = true;
    }
    
    if(initial_measurments >= 50)
    {
      hr_display.drawString(String(BPM), 0, 0, 7);
      hr_display.pushSprite(100, 30);
      digit_box.pushSprite(163, 30);
    }

    //tft.setTextColor(TFT_RED, TFT_GREEN);
    //digit_box.pushSprite(160, 40);

    //display heart rate
    //char heartrate_string[10];
    //sprintf(heartrate_string, "%02d", heartrate);
    //hr_display.drawString(String(heartrate), 0, 0, 7);
    //hr_display.drawString(heartrate_string, 0, 0, 7);
    //hr_display.pushSprite(100, 30);

    delay(70);
    

    //Serial.println("ECG: ");
    //Serial.println(ecg_reading);

    Serial.println("HR ");
    Serial.println(BPM);

    if(initial_measurments < 50)
    {
      initial_measurments++;
    }
    
  }
}

void calculateBPM () 
{  
  int beat_new = millis();    // get the current millisecond
  int diff = beat_new - beat_old;    // find the time between the last two beats
  float currentBPM = 60000 / diff;    // convert to beats per minute
  beats[beatIndex] = currentBPM;  // store to array to convert the average
  float total = 0.0;
  for (int i = 0; i < 15; i++){
    total += beats[i];
  }
  BPM = int(total / 15);
  beat_old = beat_new;
  beatIndex = (beatIndex + 1) % 15;  // cycle through the array instead of using FIFO queue
  }