#include <TFT_eSPI.h>
#include <WiFi.h>
#include <math.h>

//Pin assignments
extern int buttonL;
extern int buttonR;
extern int basePin;
extern int shoulderPin;
extern int elbowPin;
extern int wrist1Pin;
extern int wrist2Pin;
extern int wrist3Pin;

//Global variables used across multiple files
extern TFT_eSPI tft;

extern const char* ssid;
extern const char* password;

extern const char* HOST;
extern const int PORT;
extern const int GRIPPER_PORT;

extern WiFiClient client; 
extern WiFiClient gripper_client;

extern int anglePins[6];
extern float angles[6];
extern float **playback_states;
extern bool gripperState;

//UR5Control functions
bool toggleUR5Connection();
bool toggleGripperConnection();
bool moveL(float pose[], float a, float v, bool JointSpace);
bool moveJ(float pose[], float a, float v, bool JointSpace);
void getAngles();
bool setGripper(bool gripperState);
bool playBack();

// Joint Space (angles of each joint from base to tip)
double t1 = 0.0; 
double t2 = 0.0; 
double t3 = 0.0; 
double t4 = 0.0; 
double t5 = 0.0;
double t6 = 0.0;

// Cartesian Space (3D point vectors of each joint from base to tip)
double jointPositions[6][3];

//Graphics functions
void printAngles(float angles[6]);

