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
    tft.drawString(String(i + 1), xOffset + 10, yOffset + 15);
  }
  tft.setTextSize(2);
  /*
  tft.drawPixel(106, 25, TFT_GREEN);
  tft.drawPixel(106, 169, TFT_GREEN);
  tft.drawPixel(319, 25, TFT_GREEN);
  tft.drawPixel(319, 169, TFT_GREEN);
  */
  showState("DISCONNECTED");
  showCommand("NO COMMANDS YET");
  showButtons("cnct", "tgl gripper");

  int box_width = 70;
  int graphic_window_x = 106;  //x of top left corner graphic window
  int graphic_window_y = 36;   //y of top left corner graphic window
  int graphic_window_bottom_y = graphic_window_y + box_width + 10;


  tft.drawRect(graphic_window_x, 35, width - 106, graphic_window_bottom_y - 35, TFT_RED);  //border for graphical display

  tft.drawRect(graphic_window_x, graphic_window_bottom_y + 1, width - graphic_window_x, height - graphic_window_bottom_y - 10, TFT_BLUE);  //border for gcartesian coords
}

//displays 6 angles from controller onto ttgo in degrees
void printAngles() {  //float angles[6]) {
  tft.setTextSize(2);
  int xOffset = 33;
  tft.fillRect(xOffset, 0, 70, height, TFT_BLACK);
  int dy = height / 7 + 4;
  for (int i = 0; i < 6; i++) {
    int yOffset = height / 28 + i * dy;
    tft.drawString(String(angles[i] * 180.0 / PI, 1), xOffset, yOffset);
  }
  /*
  for (int i = 0; i<6; i++) {
    Serial.print(String(i) + "=" + angles[i] + ", ");
  }
  */
}

void showCommand(String command) {
  tft.fillRect(110, 24, width - 110, 10, TFT_BLACK);
  tft.setTextSize(1);
  tft.drawString(command, 110, 24);
  tft.setTextSize(2);
}

void showState(String state) {
  STATE = state;  //assigns state to the global variable
  tft.fillRect(110, 4, width - 110 - height / 7 - 4, 20, TFT_BLACK);
  tft.setTextSize(2);


  //circle in top right to show state
  uint16_t color = TFT_PINK;  // default to unknown state
  if (STATE == "DISCONNECTED") {
    color = TFT_ORANGE;
  } else if (STATE == "CONNECTED") {
    color = TFT_GREEN;
  } else if (STATE == "LIVE CONTROL") {
    color = TFT_YELLOW;
  } else if (STATE == "PLAYBACK") {
    color = TFT_BLUE;
  } else if (STATE == "FAILED CONNECTION") {
    color = TFT_RED;
  }

  tft.setTextColor(color);
  tft.drawString(STATE, 110, 4);
  tft.setTextColor(TFT_WHITE);
}

void showGraph() {

  //tft.fillScreen(TFT_BLACK);

  //int width= 320, height = 170;
  //graphic window = width=213, height=125
  //box width = 213/3 = 71*71
  //location of first box TL = 106,35
  //location of 2nd box TL=106+71, 35
  //location of 3rd box TL=106+2*71, 35

  // Draw each view in 100x100 box (representing -1.0 to 1.0 in both horizontal/vertical directions)
  double scale_factor = 50.0 * 70.0 / 100.0;
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(1);
  int box_width = 70;          //width of each viewing box
  int graphic_window_x = 106;  //x of top left corner graphic window
  int graphic_window_y = 36;   //y of top left corner graphic window
  int graphic_window_bottom_y = graphic_window_y + box_width + 10;

  tft.fillRect(graphic_window_x + 1, 35 + 1, width - 106 - 2, graphic_window_bottom_y - 35 - 2, TFT_BLACK);  //border for graphical display

  tft.fillRect(graphic_window_x + 1, graphic_window_bottom_y + 2, width - graphic_window_x - 2, height - graphic_window_bottom_y - 10 - 2, TFT_BLACK);  //border for gcartesian coords

  //tft.drawRect(graphic_window_x+1, graphic_window_bottom_y+2, width - graphic_window_x - 2, height - graphic_window_bottom_y - 12, TFT_BLUE);  //border for gcartesian coords



  // Side View
  int cx = graphic_window_x + box_width / 2;
  int cy = graphic_window_y + box_width / 2 + 10;
  tft.drawString("SIDE", cx, graphic_window_y + 1);
  tft.drawSmoothCircle(cx, cy, 2, TFT_RED, TFT_RED);
  for (int i = 0; i < 6; i++) {
    if (i == 0) {
      tft.drawLine(cx, cy, jointPositions[i][0] * scale_factor + cx, -jointPositions[i][2] * scale_factor + cy, TFT_WHITE);
    } else {
      tft.drawLine(jointPositions[i - 1][0] * scale_factor + cx, -jointPositions[i - 1][2] * scale_factor + cy, jointPositions[i][0] * scale_factor + cx, -jointPositions[i][2] * scale_factor + cy, TFT_WHITE);
    }
  }

  // Front View
  cx = graphic_window_x + box_width + box_width / 2;
  tft.drawString("FRONT", cx, graphic_window_y + 1);
  tft.drawSmoothCircle(cx, cy, 2, TFT_RED, TFT_RED);
  for (int i = 0; i < 6; i++) {
    if (i == 0) {
      tft.drawLine(cx, cy, jointPositions[i][1] * scale_factor + cx, -jointPositions[i][2] * scale_factor + cy, TFT_WHITE);
    } else {
      tft.drawLine(jointPositions[i - 1][1] * scale_factor + cx, -jointPositions[i - 1][2] * scale_factor + cy, jointPositions[i][1] * scale_factor + cx, -jointPositions[i][2] * scale_factor + cy, TFT_WHITE);
    }
  }

  // Top View
  cx = graphic_window_x + 2 * box_width + box_width / 2;
  tft.drawString("TOP", cx, graphic_window_y + 1);
  tft.drawSmoothCircle(cx, cy, 2, TFT_RED, TFT_RED);
  for (int i = 0; i < 6; i++) {
    if (i == 0) {
      tft.drawLine(cx, cy, jointPositions[i][0] * scale_factor + cx, -jointPositions[i][1] * scale_factor + cy, TFT_WHITE);
    } else {
      tft.drawLine(jointPositions[i - 1][0] * scale_factor + cx, -jointPositions[i - 1][1] * scale_factor + cy, jointPositions[i][0] * scale_factor + cx, -jointPositions[i][1] * scale_factor + cy, TFT_WHITE);
    }
  }
  
  //draw cartesian coords along bottom of graphic window
  //tft.drawLine(graphic_window_x, graphic_window_bottom_y, width-1, graphic_window_bottom_y, TFT_GREEN);

  //graphic_window_x + 1, graphic_window_bottom_y + 2, width - graphic_window_x - 2
  tft.setTextSize(1);
  
  int y = (graphic_window_bottom_y + 4 + height - 10)/2;

  String s = "x:" + String(jointPositions[5][0], 1);
  tft.setTextDatum(ML_DATUM);
  tft.drawString(s, graphic_window_x + 1, y);

  s = "y:" + String(jointPositions[5][1], 1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(s, graphic_window_x + 1 + box_width+box_width/2, y);
  
  s = "z:" + String(jointPositions[5][2], 1);
  tft.setTextDatum(MR_DATUM);
  tft.drawString(s, width-1, y);
  tft.setTextDatum(TL_DATUM);
}

//prints a string near buttons to show what each do e.g. "playback" or "connect"
void showButtons(String buttonLString, String buttonRString) {
  static int lastUpdate = millis();
  if (millis() - lastUpdate > 500) {
    tft.setTextSize(1);
    tft.fillRect(106, 161, width - 106, 10, TFT_BLACK);

    tft.setTextDatum(BL_DATUM);  //align text at top right corner
    tft.drawString("L:" + buttonLString, 106, height - 1);

    tft.setTextDatum(BR_DATUM);  //align text at bottom right corner
    tft.drawString("R:" + buttonRString, width - 1, height - 1);

    tft.setTextDatum(TL_DATUM);  //align text back at default top left
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    lastUpdate = millis();
  }
}