/*
esp32 = client
ur5e = server

Code structure:
ProjectGroup7.h   - header file that declares any global variables that are used between files like pin assignments
ProjectGroup7.ino - high level strategy of controlling the robot, just calls the functions from other files
UR5Control.ino    - includes any functions that communicates or controls the UR5e 
Graphics.ino      - all functions that control the ttgo display
*/

#include "ProjectGroup7.h"

TFT_eSPI tft = TFT_eSPI();
int width= 320, height = 170;

const char* ssid = "UR5e"; 
const char* password = "noisytrain086"; 

//Server details
const char* HOST = "192.168.1.59";//"192.168.1.54";//"; // //"192.168.86.120" ;
const int PORT = 30002;
const int GRIPPER_PORT = 63352;

WiFiClient client; 
WiFiClient gripper_client;


float playback_states[PLAYBACK_ROWS][7]; //each row = [t1,t2,t3,t4,t5,t6,gripperState]
int playbackIndex = 0;  // To track current sample index
bool gripperState; //0=open, 1=closed

float a = 0.1, v = 0.1; //max allowable speed(rad/s) and accel(rad/s^2) 

String STATE = "DISCONNECTED"; //initial state is disconnected from ur5
float angles[6]; //stores current joint angle of controller
float prev_angles[6]; //stores previous angle, used to limit speed 

//bool connectToWifi(int num_attempts);

void setup() {
  Serial.begin(115200);

  // Initialise joint pins and pushbutton
  pinMode(BASE_PIN, INPUT);
  pinMode(SHOULDER_PIN, INPUT);
  pinMode(ELBOW_PIN, INPUT);
  pinMode(WRIST_1_PIN, INPUT);
  pinMode(WRIST_2_PIN, INPUT);
  pinMode(WRIST_3_PIN, INPUT);
  
  pinMode(WHITE_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);

  //Clear serial monitor
  for (int i = 0; i < 30; i++) {
    Serial.println();
  }
  Serial.println("Setup function:");
  delay(2000);

  //Set up ESP32-s3 screen
  tft.init();
  tft.setTextSize(1);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  Serial.println("ESP screen prepared");

  //Connect to router
  int num_attempts = 20;  //how many times wifi will try to connect before timeout
  while (!connectToWifi(num_attempts)) {
    while (true) {
      if (!digitalRead(WHITE_BUTTON) || !digitalRead(GREEN_BUTTON)) {
        num_attempts += 5;
        break;
      }
    }
  }
  

  //prints small green loading bar along bottom to give time to read IP
  for (int i = 0; i < width; i++) {
    tft.drawLine(i, height - 40, i, height-10, TFT_DARK_GREEN);
    delay(2000 / width);
  }
  delay(1000);

  createGraphicWindows();
}

void loop() {

  static unsigned long last_pressed = millis();
  static unsigned long last_sample = millis();
  static unsigned long last_print = millis();
  
  /*
  if ((millis() - last_print) > 300) {
    Serial.println("W: " + String(!digitalRead(WHITE_BUTTON)) + "G: " + String(!digitalRead(GREEN_BUTTON)));
    last_print = millis();
  }
  */

  // Update and print joint angles every 250ms
  if ((millis() - last_print) > 350) {
    last_print = millis();
    getJointAngles();
    forwardKinematics(); 
    printAngles();
    showGraphicDisplay();
    for (int i = 0; i < 6; i++) {
      //Serial.print("t" + String(i+1) + ": " + String(angles[i]));
    }
    //Serial.println("");
  }

  

  if (STATE == "DISCONNECTED") {
    showButtons("Connect", "");
    if ((millis() - last_pressed) > 300) {
      if (!digitalRead(WHITE_BUTTON)) {
        last_pressed = millis();
        if (toggleUR5Connection() && toggleGripperConnection()) {
          showState("CONNECTED");
        } else {
          showState("FAILED CONNECTION");
        }
        
      }
    }
  } else if (STATE == "CONNECTED") {
    showButtons("Live control", "Playback");
    if ((millis() - last_pressed) > 300) {
      if (!digitalRead(WHITE_BUTTON)) {
        moveJ(angles, a, v, true);
        
        showState("LIVE CONTROL");
        showCommand("live control");

        //store current position as first row in playback_states
        for (int i = 0; i < 6; i++) {
          playback_states[0][i] = angles[i];
        }
        playback_states[0][6] = gripperState; 
        playbackIndex = 1;
        

      } else if (!digitalRead(GREEN_BUTTON)) {
        
        showState("PLAYBACK");
        //playbackIndex = 0;
        /*
        if (playbackIndex == 0) {
          Serial.println("NO SEQUENCE RECORDED");
        } else {
          
          //Serial.println("Playback finished");
        }
        */
        
        Serial.println("PRESSED");
      }
      last_pressed = millis();
    }
  } else if (STATE == "LIVE CONTROL") {
    showButtons("Toggle Gripper", "EXIT");
    if (millis() - last_sample > SAMPLE_RATE){
      moveJ(angles, a, v, true);

      //stores current state into playback array
      for (int i = 0; i < 6; i++) {
          playback_states[playbackIndex][i] = angles[i];
        }
      playback_states[playbackIndex][6] = gripperState; 
      playbackIndex++;
      last_sample = millis();
    }  
    if ((millis() - last_pressed) > 300) {
      if (!digitalRead(WHITE_BUTTON)) {
        gripperState = !gripperState;
        setGripper(gripperState);
      } else if (!digitalRead(GREEN_BUTTON)) {
        //stops live playback and goes back to just being connected
        showState("CONNECTED");
      }
      last_pressed = millis();
    }    
  } else if (STATE == "PLAYBACK") {
    delay(1000);
    showButtons("EXIT", "");
    //playBack(); 
    
    showCommand("Moving to start");
    moveJ(playback_states[0], a, v, true); //moves to first position 
    setGripper(playback_states[0][6]); //may need to change if gripperState is being stored as 1.0 not 1
    

    //iterate through playback_states
    int i = 0;
    while (i < playbackIndex && digitalRead(WHITE_BUTTON)) {
      if (millis() - last_sample > SAMPLE_RATE){
        String msg = "Position " + String(i) + "/" + String(playbackIndex-1); 
        Serial.println(msg);

        moveJ(playback_states[i], a, v, true);
        setGripper(playback_states[i][6]);
        i++;
        last_sample = millis();
      }
    }

    //ends playback and goes back to "CONNECTED" 
    Serial.println("Playback ended");
    showCommand("Playback ended");
    showState("CONNECTED");
  }
 
}

bool connectToWifi(int num_attempts) {
  //return true; //for testing

  tft.fillScreen(TFT_BLACK);
  //Connect to router
  String WifiConnectionMessage = "Connecting to " + String(ssid) + " wifi...";
  tft.drawString(WifiConnectionMessage, 2, height / 4);

  WiFi.begin(ssid, password);
  int failed_connections = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    WifiConnectionMessage += ".";
    tft.drawString(WifiConnectionMessage, 2, height / 4);
    failed_connections++;
    if (failed_connections > num_attempts) {
      tft.setTextColor(TFT_RED);
      tft.drawString("FAILED TO CONNECT TO WIFI", 20, height / 2);
      tft.drawString("Press any button to retry", 20, 3 * height / 4);
      tft.setTextColor(TFT_WHITE);
      return false;
    }
  }

  tft.setTextColor(TFT_GREEN);
  WifiConnectionMessage += "connected :)";
  tft.drawString(WifiConnectionMessage, 2, height / 4);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("localIP: " + String(WiFi.localIP()), 20, height / 2);

  return true;
}
