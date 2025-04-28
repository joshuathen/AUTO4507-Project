



// /*
// esp32 = client
// ur5e = server


// Approach:
// - ESP32 automatically joins to wifi and sets up screen display
// - Pressing left button will connect to actual robot and gripper, and enter the controlling/recording mode
// - in controlling/recording mode the angles are continuosly measured and moveJ is called to move ur5 into desired position
// - the controllers 6 angles are also sampled every 0.5 seconds ("sampleRate") and stored in the 2Dp
//   "playback_states" array
// - Whilst in controlling/recording mode, the right button can be used to toggle the gripper, this is also saved in "playback_states"
// - Pressing the left button again will stop the connection between the robot/gripper
// - Once out of recording mode, pressing the right button will enter "playback mode"
// - "Playback mode" will move the robot to first recorded position in "playback_angles" and then iterate through each recorded state


// Code structure:
// ProjectGroup7.h   - header file that declares any global variables that are used between files like pin assignments
// ProjectGroup7.ino - high level strategy of controlling the robot, just calls the functions from other files
// UR5Control.ino    - includes any functions that communicates or controls the UR5e 
// Graphics.ino      - all functions that control the ttgo display


// Whats been tested and works:
// - MoveJ and MoveL and the general connection to robot works


// Potential problems/changes:
// - Making the "playback_angles" 2D array used dynamic memory allocation which I havent done in ages, might have some syntax errors
//   or cause too much memory usage. Currently the stored values are rounded to floats with 2 decimal places, but less memory could be used
//   if they are stored as ints (e.g. multiply by 100 and round, and then in playback just divide by 100 again).

// - Currently the robot moves during recording sequence of angles AND during playback. I'm not sure if we want it to
//   move during the recording state or not

// - Haven't tested the gripper code yet, but should be easy to test by calling "toggleGripperConnection()" followed by 
//   "setGripper(bool gripperState)" in the setup function just to see if it moves

// - Potentiometer readings need to be scaled to the correct angle in "get_angles()" function

// - When the connection is first established and recording/control mode is entered, might be useful to have a moveJ call to move robot to 
//   the controller position and then wait for some time so the robot and controller are in sync.

// - currently calling moveJ and moveL uses a and v to control speed, in documentation i think calling 
//   movej(pose, a, v, t) will ignore the a and v, and just move to the position is time t.
//   e.g. having command = "movej([10,10,10,10,10,10], a=1,v=1, t=0.5\n" would move to the new position in 
//   0.5 seconds, this should be easy to implement in moveJ and could be more useful during playback as we are 
//   sampling the controller positons at sampleRate = 0.5 seconds. During playback this would ensure the robot 
//   reaches the new position at exactly the right time rather than having to introduce a delay and waiting for it
// */

// #include "ProjectGroup7.h"

// //Pins
// int buttonL = 0;
// int buttonR = 14;
// int basePin = 1;
// int shoulderPin = 2;
// int elbowPin = 3;
// int wrist1Pin = 10;
// int wrist2Pin = 11;
// int wrist3Pin = 12;

// TFT_eSPI tft = TFT_eSPI();
// int width= 320, height = 170;

// const char* ssid = "UR5e"; 
// const char* password = "noisytrain086"; 

// //Server details
// const char* HOST = "192.168.1.59"; //"192.168.1.54" ; //"192.168.86.120" ;
// const int PORT = 30002;
// const int GRIPPER_PORT = 63352;

// WiFiClient client; 
// WiFiClient gripper_client;

// int anglePins[6] = {basePin, shoulderPin, elbowPin, wrist1Pin, wrist2Pin, wrist3Pin};
// float angles[6];

// float **playback_states; //2d array of measured joint angles for playback
// float sampleRate = 500; //samples position every 500ms
// int number_of_samples = 10; //will be increased as sampling goes on
// int playbackIndex = 0;  // To track current sample index
// bool gripperState; //0=open, 1=closed

// float a = 0.2, v = 0.2;

// void setup() {
//   Serial.begin(115200);
//   for (int i = 0; i < 30; i++) {
//     Serial.println();
//   }
//   Serial.println("Setup function:");
//   delay(2000);

//   pinMode(buttonL, INPUT_PULLUP);
//   pinMode(buttonR, INPUT_PULLUP);
//   for (int i = 0; i < 6; i++) {
//     pinMode(anglePins[i], INPUT);
//   }

//   //Set up ESP32-s3 screen
//   tft.init();
//   tft.setTextSize(2);
//   tft.setTextDatum(MC_DATUM);
//   tft.setRotation(1);
//   tft.fillScreen(TFT_BLACK);
//   tft.setTextColor(TFT_WHITE);
//   Serial.println("ESP screen prepared");
//   printAngles(angles);

//   //Connect to router
//   Serial.print("Connecting to ");
//   Serial.print(ssid);
//   Serial.print(" wifi...");
  
//   WiFi.begin(ssid, password);
//   int failed_connections = 0;
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(200);
//     Serial.print(".");
//     failed_connections++;
//     if (failed_connections > 100){
//       Serial.println("FAILED TO CONNECT TO WIFI");
//       return;
//     }
//   }
//   Serial.println("connected :)");
//   Serial.print("localIP: ");
//   Serial.println(WiFi.localIP());

//   // Display raw joint space / cartesian space values
//   updateDisplay(1);
// }

// void loop() {
//   toggleUR5Connection();
//   delay(1000);
//   moveL(pose[], 0.5, 1.2, true);
//   /*
//   //temp comment 
//   static int last_pressed = millis();
//   static int last_sample = millis();

//   // Debounce
//   if ((millis() - last_pressed) > 500) {
//     if (digitalRead(buttonL) == LOW) {
//       if (toggleUR5Connection() && toggleGripperConnection()) 
//       {
//         Serial.println("Recording started");
//         //free memory from previous playback_states
//         for (int i = 0; i < number_of_samples; i++) {
//           free(playback_states[i]);
//         }
//         free(playback_states);

//         number_of_samples = 10; //will be increased as sampling goes on
//         playbackIndex = 0;  // To track current sample index
//         gripperState = 0;
//         //allocate memory for new playback_states
//         playback_states = (float**)malloc(number_of_samples * sizeof(float*));
//         for (int i = 0; i < number_of_samples; i++) {
//           playback_states[i] = (float*)malloc((6 + 1) * sizeof(float));  // 6 angles plus gripper state
//         }
//       } else {
//         Serial.println("Recording ended");
//       }
//       last_pressed = millis();
//     } else if (digitalRead(buttonR) == LOW) {
//       if (client.connected()) {
//         gripperState = !gripperState;
//         setGripper(gripperState);
//       } else {
//         playBack();
//       }
      
//       last_pressed = millis();
//     }
//     getAngles();
    

//     if (client.connected() && millis() - last_sample > sampleRate) {
//       //append state to playback_states
//       if (playbackIndex >= number_of_samples) {
//         // needs to increase memory allocated to playback_states
//         number_of_samples += number_of_samples;
//         playback_states = (float**)realloc(playback_states, number_of_samples * sizeof(float*));
        
//         // Reallocate memory for each row to store joint angles + gripper state
//         for (int i = playbackIndex; i < number_of_samples; i++) {
//           playback_states[i] = (float*)malloc((6 + 1) * sizeof(float));
//         }
//       }
      
//       // Record joint angles and gripper state
//       if (playbackIndex < number_of_samples) {
//         for (int i = 0; i < 6; i++) {
//           playback_states[playbackIndex][i] = (int)(angles[i] * 100.0) / 100.0;  // Record joint angle rounded to 2 d.p
//         }
//         playback_states[playbackIndex][6] = gripperState;  // Store gripper state
        
//         playbackIndex++;  // Increment the sample index
//       }
//       last_sample = millis();  
//     }

//   }

//   static int last_refresh = millis();
//   if ((millis() - last_refresh) > 1000) {
//     printAngles(angles);
//     last_refresh = millis();
//   }
//   */
// }



