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
 * File: header.h                                                              *
 *                                                                             *
 * Description: A patient monitor measuring the three most important           *
 *              physilogical parameters: blood oxygen, ECG, and blood pressure *   
 *                                                                             *
 *                                                                             *
 ******************************************************************************/

// Guard Code 
#ifndef PATIENT_MONITOR_H
#define PATIENT_MONITOR_H

// Libraries 
#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <BfButton.h>
#include <Wire.h>
#include "ecg_sig.h"
#include "title_font.h"
#include "heartbeat/hb_frame0.h"
#include "heartbeat/hb_frame1.h"
#include "heartbeat/hb_frame2.h"
#include "heartbeat/hb_frame3.h"
#include "heartbeat/hb_frame4.h"
// SpO2
#include "SPO2_gif/SPO2_frame0.h"
#include "SPO2_gif/SPO2_frame1.h"
#include "SPO2_gif/SPO2_frame2.h"
#include "SPO2_gif/SPO2_frame3.h"
#include "SPO2_gif/SPO2_frame4.h"
#include "MAX30100_PulseOximeter.h"
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <SparkFun_Bio_Sensor_Hub_Library.h>

#define REPORTING_PERIOD_MS     1000
#define MAX_BRIGHTNESS 255

// Define IO Pins 
const int PinCLK = 27;
const int PinDT = 14;
const int PinSW = 13;
const int PinECG = 15;
//const int SDA_PIN = 21;
//const int SCL_PIN = 22;
const int PIN_GPIO = 19;
const int PIN_SPO2_RST = 18;
const int PINBP = 2;
const int A1 = 16;
const int A2 = 17;
const int valveSwitch = 18;
const int pVIn = 2;
// From Sparkfun Reset pin, MFIO pin
const int resPin = 4;
const int mfioPin = 5;


// Function definitions 
void read_ecg();
void read_spo2();
void read_bp();
void ecg_measurement();
void spo2_measurment();
void bp_measurement();
void display_spo2(int finger_detect);
void drawLine(int xPos, int analogVal);
void calculateBPM(); 

//Blood Pressure Functions
void openValve();
void closeValve();
void cycleBPSystem();
void runPump30s();
void pumpOn();
void stopPump();
void displayBP(float pressure);

#endif