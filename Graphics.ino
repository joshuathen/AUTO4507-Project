/*
All functions related to ttgo display
Currently I use serial print to show what state the controller is in 
e.g. connected to robot and recording angles, disconnected, or
playing back the recorded sequence but this might be good to display on ttgo too
*/

#include "ProjectGroup7.h"

int screen_no = 1;

void updateDisplay(int _screen_no) {
  // screen_no = 1 (raw values), 2 (visualisation of robot), 0 (don't change screen - just update!)
  if (_screen_no != 0) {
    screen_no = _screen_no;
  }

  tft.fillScreen(TFT_BLACK);

  if (screen_no == 1) {
    // Draw Joint Values
    tft.setTextDatum(TL_DATUM);
    tft.drawString("JOINTS", 0, 0);
    String s = "t1: " + String(t1, 2);
    tft.drawString(s, 0, 20);
    s = "t2: " + String(t2, 2);
    tft.drawString(s, 0, 40);
    s = "t3: " + String(t3, 2);
    tft.drawString(s, 0, 60);
    s = "t4: " + String(t4, 2);
    tft.drawString(s, 0, 80);
    s = "t5: " + String(t5, 2);
    tft.drawString(s, 0, 100);
    s = "t6: " + String(t6, 2);
    tft.drawString(s, 0, 120);

    // Draw Cartesian Values
    tft.setTextDatum(TR_DATUM);
    tft.drawString("CARTESIAN", 320, 0);
    s = "x: " + String(jointPositions[5][0], 2);
    tft.drawString(s, 320, 20);
    s = "y: " + String(jointPositions[5][1], 2);
    tft.drawString(s, 320, 40);
    s = "z: " + String(jointPositions[5][2], 2);
    tft.drawString(s, 320, 60);
  }

  else {
    // Draw each view in 100x100 box (representing -1.0 to 1.0 in both horizontal/vertical directions)
    double scale_factor = 50.0;
    tft.setTextDatum(TC_DATUM);

    // Side View
    int cx = 53;
    int cy = 85;
    tft.drawString("SIDE", cx, 0);
    tft.drawSmoothCircle(cx, cy, 2, TFT_RED, TFT_RED);
    for (int i = 0; i < 6; i++) {
      if (i == 0) {
        tft.drawLine(cx, cy, jointPositions[i][0]*scale_factor + cx, -jointPositions[i][2]*scale_factor + cy, TFT_WHITE);
      }
      else {
        tft.drawLine(jointPositions[i-1][0]*scale_factor + cx, -jointPositions[i-1][2]*scale_factor + cy, jointPositions[i][0]*scale_factor + cx, -jointPositions[i][2]*scale_factor + cy, TFT_WHITE);
      }
    }

    // Front View
    cx = 160;
    tft.drawString("FRONT", cx, 0);
    tft.drawSmoothCircle(cx, cy, 2, TFT_RED, TFT_RED);
    for (int i = 0; i < 6; i++) {
      if (i == 0) {
        tft.drawLine(cx, cy, jointPositions[i][1]*scale_factor + cx, -jointPositions[i][2]*scale_factor + cy, TFT_WHITE);
      }
      else {
        tft.drawLine(jointPositions[i-1][1]*scale_factor + cx, -jointPositions[i-1][2]*scale_factor + cy, jointPositions[i][1]*scale_factor + cx, -jointPositions[i][2]*scale_factor + cy, TFT_WHITE);
      }
    }

    // Top View
    cx = 267;
    tft.drawString("TOP", cx, 0);
    tft.drawSmoothCircle(cx, cy, 2, TFT_RED, TFT_RED);
    for (int i = 0; i < 6; i++) {
      if (i == 0) {
        tft.drawLine(cx, cy, jointPositions[i][0]*scale_factor + cx, -jointPositions[i][1]*scale_factor + cy, TFT_WHITE);
      }
      else {
        tft.drawLine(jointPositions[i-1][0]*scale_factor + cx, -jointPositions[i-1][1]*scale_factor + cy, jointPositions[i][0]*scale_factor + cx, -jointPositions[i][1]*scale_factor + cy, TFT_WHITE);
      }
    }
  }
}

// void loop() {
//   // put your main code here, to run repeatedly:
//   if (!digitalRead(0)) {
//     Serial.println("BOOT");
//     delay(300);
//     updateDisplay(1);
//   }
//   if (!digitalRead(14)) {
//     Serial.println("KEY");
//     delay(300);
//     updateDisplay(2);
//   }

//   t1 += 0.1;
//   forwardKinematics();
//   delay(100);
// }