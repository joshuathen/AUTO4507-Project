/*
esp32 = client
ur5e = server


Approach:
- ESP32 automatically joins to wifi and sets up screen display
- Pressing left button will connect to actual robot and gripper, and enter the controlling/recording mode
- in controlling/recording mode the angles are continuosly measured and moveJ is called to move ur5 into desired position
- the controllers 6 angles are also sampled every 0.5 seconds ("SAMPLE_RATE") and stored in the 2Dp
  "playback_states" array
- Whilst in controlling/recording mode, the right button can be used to toggle the gripper, this is also saved in "playback_states"
- Pressing the left button again will stop the connection between the robot/gripper
- Once out of recording mode, pressing the right button will enter "playback mode"
- "Playback mode" will move the robot to first recorded position in "playback_angles" and then iterate through each recorded state


Code structure:
ProjectGroup7.h   - header file that declares any global variables that are used between files like pin assignments
ProjectGroup7.ino - high level strategy of controlling the robot, just calls the functions from other files
UR5Control.ino    - includes any functions that communicates or controls the UR5e 
Graphics.ino      - all functions that control the ttgo display


Whats been tested and works:
- MoveJ and MoveL and the general connection to robot works


Potential problems/changes:
- Making the "playback_angles" 2D array used dynamic memory allocation which I havent done in ages, might have some syntax errors
  or cause too much memory usage. Currently the stored values are rounded to floats with 2 decimal places, but less memory could be used
  if they are stored as ints (e.g. multiply by 100 and round, and then in playback just divide by 100 again).

- Currently the robot moves during recording sequence of angles AND during playback. I'm not sure if we want it to
  move during the recording state or not

- Haven't tested the gripper code yet, but should be easy to test by calling "toggleGripperConnection()" followed by 
  "setGripper(bool gripperState)" in the setup function just to see if it moves

- Potentiometer readings need to be scaled to the correct angle in "get_angles()" function

- When the connection is first established and recording/control mode is entered, might be useful to have a moveJ call to move robot to 
  the controller position and then wait for some time so the robot and controller are in sync.

- currently calling moveJ and moveL uses a and v to control speed, in documentation i think calling 
  movej(pose, a, v, t) will ignore the a and v, and just move to the position is time t.
  e.g. having command = "movej([10,10,10,10,10,10], a=1,v=1, t=0.5\n" would move to the new position in 
  0.5 seconds, this should be easy to implement in moveJ and could be more useful during playback as we are 
  sampling the controller positons at SAMPLE_RATE = 0.5 seconds. During playback this would ensure the robot 
  reaches the new position at exactly the right time rather than having to introduce a delay and waiting for it
*/

#include "ProjectGroup7.h"

//Pins
int buttonL = 0;
int buttonR = 14;
// int basePin = 1;
// int shoulderPin = 2;
// int elbowPin = 3;
// int wrist1Pin = 10;
// int wrist2Pin = 11;
// int wrist3Pin = 12;

TFT_eSPI tft = TFT_eSPI();
int width= 320, height = 170;

const char* ssid = "Donnabjbg"; //"UR5e"; 
const char* password = "Stanley061220"; //"noisytrain086"; 

//Server details
const char* HOST = "192.168.1.59"; //"192.168.1.54" ; //"192.168.86.120" ;
const int PORT = 30002;
const int GRIPPER_PORT = 63352;

WiFiClient client; 
WiFiClient gripper_client;


float playback_states[PLAYBACK_ROWS][7]; //each row = [t1,t2,t3,t4,t5,t6,gripperState]
int playbackIndex = 0;  // To track current sample index
bool gripperState; //0=open, 1=closed

float a = 0.1, v = 0.1; //max allowable speed in 

String STATE = "DISCONNECTED"; //initial state is disconnected from ur5
float angles[6];
float prev_angles[6]; //stores previous angles, used to determine if large change between angles so bot should slow down
void setup() {
  Serial.begin(115200);

  // Initialise joint pins and pushbutton
  pinMode(ANALOG_PIN_1, INPUT);
  pinMode(ANALOG_PIN_2, INPUT);
  pinMode(ANALOG_PIN_3, INPUT);
  pinMode(ANALOG_PIN_4, INPUT);
  pinMode(ANALOG_PIN_5, INPUT);
  pinMode(ANALOG_PIN_6, INPUT);
  pinMode(PUSHBUTTON_PIN, INPUT_PULLUP);

  for (int i = 0; i < 30; i++) {
    Serial.println();
  }
  Serial.println("Setup function:");
  delay(2000);

  pinMode(buttonL, INPUT_PULLUP);
  pinMode(buttonR, INPUT_PULLUP);

  //Set up ESP32-s3 screen
  tft.init();
  tft.setTextSize(1);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  Serial.println("ESP screen prepared");

  //Connect to router
  int num_attempts = 15;  //how many times wifi will try to connect before throwing error
  while (!connectToWifi(num_attempts)) {
    while (true) {
      if (!digitalRead(buttonL) || !digitalRead(buttonR)) {
        num_attempts += 5;
        break;
      }
    }
  }

  //prints small loading bar along bottom to give time to read IP
  for (int i = 0; i < width; i++) {
    tft.drawLine(i, height - 40, i, height-10, TFT_GREEN);
    delay(2000 / width);
  }
  delay(1000);
  createGraphicWindows();
  // toggleUR5Connection();
  STATE = "DISCONNECTED";
}

void loop() {
  static int last_pressed = millis();
  static int last_sample = millis();
  static int last_refresh = millis();

  // Get and print out joint angles every 1 second
  if ((millis() - last_refresh) > 300) {
    
    last_refresh = millis();
    getJointAngles();
    printAngles();
    /*
    Serial.print("t1: "); Serial.print(t1);
    Serial.print("| t2: "); Serial.print(t2);
    Serial.print("| t3: "); Serial.print(t3);
    Serial.print("| t4: "); Serial.print(t4);
    Serial.print("| t5: "); Serial.print(t5);
    Serial.print("| t6: "); Serial.println(t6);
    */
  }

  

  if (STATE == "DISCONNECTED") {
    showButtons("Connect", "");
    if ((millis() - last_pressed) > 300) {
      if (!digitalRead(buttonL)) {
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
      if (!digitalRead(buttonL)) {
        showState("LIVE CONTROL");
        showCommand("live control");

        //store current position as first row in playback_states
        for (int i = 0; i < 6; i++) {
          playback_states[0][i] = angles[i];
        }
        playback_states[0][6] = gripperState; 
        playbackIndex = 1;

      } else if (!digitalRead(buttonR)) {
        showState("PLAYBACK");
        playBack(); 
      }
    }
  } else if (STATE == "LIVE CONTROL") {
    showButtons("Toggle Gripper", "EXIT");
    if (millis() - last_sample > SAMPLE_RATE){
      moveL(angles, a, v, true);

      //stores current state into array
      for (int i = 0; i < 6; i++) {
          playback_states[playbackIndex][i] = angles[i];
        }
      playback_states[playbackIndex][6] = gripperState; 
      playbackIndex++;
      last_sample = millis();
      //Add code here to record states
    }  
    if ((millis() - last_pressed) > 300) {
      if (!digitalRead(buttonL)) {
        gripperState = !gripperState;
        setGripper(gripperState);
      } else if (!digitalRead(buttonR)) {
        showState("CONNECTED");
      }
    }    
  } else if (STATE == "PLAYBACK") {
    showButtons("EXIT", "");
    moveL(playback_states[0], a, v, true);
    setGripper(playback_states[0][7]); //may need to change if gripperState is being stored as 1.0 not 1
    Serial.println("Moving to starting position");
    showCommand("Moving to start");

    //iterate through playback_states
    int i = 0;
    while (i < playbackIndex && digitalRead(buttonL)) {
      if (millis() - last_sample > SAMPLE_RATE){
        String msg = "Position " + String(i) + "/" + String(playbackIndex-1); 
        Serial.println(msg);

        moveL(playback_states[i], a, v, true);
        setGripper(playback_states[i][1]);
        i++;
      }
    }

    //ends playback and goes back to "CONNECTED" but doesnt move bot
    Serial.println("Playback ended");
    showCommand("Playback ended");
    showState("CONNECTED");
  }


  //float myPose[] = {t1, (-90)*PI/180 , t3, -90*PI/180, 0, 0};
  // moveJ(myPose, 0.5, 0.5, true);
  // delay(3000);

  // Debounce
  if ((millis() - last_pressed) > 500) {
    if (digitalRead(buttonL) == LOW) {
      // UNCOMMENT ONCE TESTED

      // if (toggleUR5Connection() && toggleGripperConnection()) 
      // {
      //   Serial.println("Recording started");
      //   //free memory from previous playback_states
      //   for (int i = 0; i < number_of_samples; i++) {
      //     free(playback_states[i]);
      //   }
      //   free(playback_states);

      //   number_of_samples = 10; //will be increased as sampling goes on
      //   playbackIndex = 0;  // To track current sample index
      //   gripperState = 0;
      //   //allocate memory for new playback_states
      //   playback_states = (float**)malloc(number_of_samples * sizeof(float*));
      //   for (int i = 0; i < number_of_samples; i++) {
      //     playback_states[i] = (float*)malloc((6 + 1) * sizeof(float));  // 6 angles plus gripper state
      //   }
      // } else {
      //   Serial.println("Recording ended");
      // }
      last_pressed = millis();
    } else if (digitalRead(buttonR) == LOW) {
      // UNCOMMENT ONCE TESTED

      // if (client.connected()) {
      //   gripperState = !gripperState;
      //   setGripper(gripperState);
      // } else {
      //   playBack();
      // }
      
      last_pressed = millis();
    }

    // UNCOMMENT ONCE TESTED
    
    // if (client.connected() && millis() - last_sample > SAMPLE_RATE) {
    //   //append state to playback_states
    //   if (playbackIndex >= number_of_samples) {
    //     // needs to increase memory allocated to playback_states
    //     number_of_samples += number_of_samples;
    //     playback_states = (float**)realloc(playback_states, number_of_samples * sizeof(float*));
        
    //     // Reallocate memory for each row to store joint angles + gripper state
    //     for (int i = playbackIndex; i < number_of_samples; i++) {
    //       playback_states[i] = (float*)malloc((6 + 1) * sizeof(float));
    //     }
    //   }
      
    //   // Record joint angles and gripper state
    //   if (playbackIndex < number_of_samples) {
    //     for (int i = 0; i < 6; i++) {
    //       playback_states[playbackIndex][i] = (int)(angles[i] * 100.0) / 100.0;  // Record joint angle rounded to 2 d.p
    //     }
    //     playback_states[playbackIndex][6] = gripperState;  // Store gripper state
        
    //     playbackIndex++;  // Increment the sample index
    //   }
    //   last_sample = millis();  
    // }

  }
}

bool connectToWifi(int num_attempts) {
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
