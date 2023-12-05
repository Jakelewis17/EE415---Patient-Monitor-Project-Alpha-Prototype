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
 * File: blood_pressure.cpp                                                    *
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

void read_bp()
{
  what_press = 0;
  tft.fillScreen(TFT_BLACK);

  //display title
  title.fillSprite(TFT_BLACK);
  title.setTextColor(TFT_WHITE, TFT_BLACK);
  title.drawString("BP", 0, 0);
  title.pushSprite(90, 30);

  //display instructions
  tft.drawString("Click once to start", 0, 80, 4);
  tft.drawString("Click twice to return", 0, 120, 4);

  for(;;) //infinite polling loop for switch input
  {
    rotary_sw.read();
    if(what_press == 1)
    {
      //single press - jump to ECG measurement function
      //For right now, just go back to selection screen
      what_press = 0;
      tft.fillScreen(TFT_BLACK);
      break;
    }
    else if(what_press == 2)
    {
      //double press - go backto seleciton screen
      tft.fillScreen(TFT_BLACK);
      break;
    }
    
    what_press = 0;
  }
}