/*
All functions related to ttgo display
Currently I use serial print to show what state the controller is in
e.g. connected to robot and recording angles, disconnected, or
playing back the recorded sequence but this might be good to display on ttgo too
*/
 
#include "ProjectGroup7.h"
 
 
 
 
//sets up TTGO display with area for angles, graphical display, state, and recent command
void createGraphicWindows() {
  tft.fillScreen(TFT_BLACK);
  int xOffset = 3;
  int dy = height / 7 + 4;
  for (int i = 0; i < 6; i++) {
    int yOffset = height / 28 + i * dy;
    tft.setTextSize(2);
    tft.drawString("O", xOffset, yOffset);
    tft.drawString("=", xOffset + 15, yOffset);
    //tft.drawString(String(angles[i],2), xOffset + 43, yOffset);
    tft.drawLine(xOffset, yOffset - 1 + 8, xOffset + 7, yOffset - 2 + 8, TFT_WHITE);
    tft.drawLine(xOffset, yOffset - 2 + 8, xOffset + 7, yOffset - 3 + 8, TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString(String(i+1), xOffset + 10, yOffset + 15);
  }
  tft.setTextSize(2);
 
  tft.drawRect(106, 35, width - 107, height - 35 - 10, TFT_RED); //window for graphical display
  /*
  tft.drawPixel(106, 25, TFT_GREEN);
  tft.drawPixel(106, 169, TFT_GREEN);
  tft.drawPixel(319, 25, TFT_GREEN);
  tft.drawPixel(319, 169, TFT_GREEN);
  */
  showState("DISCONNECTED");
  showCommand("NO COMMANDS YET");
  showButtons("cnct", "tgl gripper");
}
 
//displays 6 angles from controller onto ttgo
void printAngles(float angles[6]) {
  int xOffset = 33;
  tft.fillRect(xOffset, 0, 70, height, TFT_BLACK);
  int dy = height / 7 + 4;
  for (int i = 0; i < 6; i++) {
    int yOffset = height/28 + i*dy;
    tft.drawString(String(angles[i], 1), xOffset, yOffset);
  }
}
 
void showCommand(String command) {
  tft.fillRect(110, 24, width - 110, 10, TFT_BLACK);
  tft.setTextSize(1);
  tft.drawString(command, 110, 24);
  tft.setTextSize(2);
}
 
void showState(String state) {
  STATE = state; //assigns state to the global variable
  tft.fillRect(110, 4, width - 110 - height/7 - 4, 20, TFT_BLACK);
  tft.setTextSize(2);
 
 
  //circle in top right to show state
  uint16_t color = TFT_PINK; // default to unknown state
  if (STATE == "DISCONNECTED") {
    color = TFT_ORANGE;
  } else if (STATE == "CONNECTED") {\
     color = TFT_GREEN;
  } else if (STATE == "PLAYBACK") {
     color = TFT_BLUE;
  } else if (STATE == "FAILED CONNECTION") {
    color = TFT_RED;
  } else if (STATE == "RECORDING LIVE") {
    color = TFT_YELLOW;
  } else if (STATE == "RUNNING") {
    color = TFT_WHITE;
  }
 
  tft.setTextColor(color);
  tft.drawString(STATE, 110, 4);
  tft.setTextColor(TFT_WHITE);
}
 
//prints a string near buttons to show what each do e.g. "playback" or "connect"
void showButtons(String buttonLString, String buttonRString) {
  tft.setTextSize(1);
  tft.fillRect(106, 161, width-106, 10, TFT_BLACK);
 
  tft.setTextDatum(BL_DATUM); //align text at top right corner
  tft.drawString("L:" + buttonLString, 106, height-1);
 
  tft.setTextDatum(BR_DATUM); //align text at bottom right corner
  tft.drawString("R:" + buttonRString, width-1, height-1);
 
  tft.setTextDatum(TL_DATUM); //align text back at default top left
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
}