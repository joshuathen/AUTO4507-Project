#include <TFT_eSPI.h>
#include <WiFi.h>
#include <math.h>

//Pin assignments
extern int buttonL;
extern int buttonR;
// extern int basePin;
// extern int shoulderPin;
// extern int elbowPin;
// extern int wrist1Pin;
// extern int wrist2Pin;
// extern int wrist3Pin;

//Global variables used across multiple files
extern TFT_eSPI tft;

extern const char* ssid;
extern const char* password;

extern const char* HOST;
extern const int PORT;
extern const int GRIPPER_PORT;

extern WiFiClient client; 
extern WiFiClient gripper_client;
extern String STATE;

#define PLAYBACK_MAX_TIME 60  //max playback length in seconds
#define SAMPLE_RATE 3000 //samples position every 500ms
#define PLAYBACK_ROWS (PLAYBACK_MAX_TIME / (SAMPLE_RATE / 1000))

extern float playback_states[PLAYBACK_ROWS][7];
extern bool gripperState;

//UR5Control functions
bool toggleUR5Connection();
bool toggleGripperConnection();
bool moveL(float pose[], float a, float v, bool JointSpace);
bool moveJ(float pose[], float a, float v, bool JointSpace);
void getAngles();
bool setGripper(bool gripperState);
bool playBack();

//Graphics functions
void printAngles();
void getJointAngles();
void showButtons(String buttonLString, String buttonRString);
void showGraph();
void showState(String state);

// Joint Space (angles of each joint from base to tip)
extern double t1; 
extern double t2; 
extern double t3; 
extern double t4; 
extern double t5;
extern double t6;

// PINOUT ESP32-S3
#define ANALOG_PIN_1 1    // GPIO1 (safe on S3)
#define ANALOG_PIN_2 2    // GPIO2 (safe)
#define ANALOG_PIN_3 3    // GPIO3 (safe)
#define ANALOG_PIN_4 10
#define ANALOG_PIN_5 11
#define ANALOG_PIN_6 12
#define PUSHBUTTON_PIN 13 // Pushbutton used to toggle/change screens

// Cartesian Space (3D point vectors of each joint from base to tip)
extern double jointPositions[6][3];

// Graphics
extern int screen_no;

