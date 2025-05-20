#include <TFT_eSPI.h>
#include <WiFi.h>
#include <math.h>

//Pin assignments

//Global variables used across multiple files
extern TFT_eSPI tft;
#define TFT_DARK_GREEN 0x03E0

extern const char* ssid;
extern const char* password;

extern const char* HOST;
extern const int PORT;
extern const int GRIPPER_PORT;

extern WiFiClient client; 
extern WiFiClient gripper_client;
extern String STATE;

#define PLAYBACK_MAX_TIME 60  //max playback length in seconds
#define SAMPLE_RATE 1000 //samples position every 500ms
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
void showButtons(String WHITE_BUTTONString, String GREEN_BUTTONString);
void showGraph();
void showState(String state);

// Joint Space (angles of each joint from base to tip)
extern double t1; 
extern double t2; 
extern double t3; 
extern double t4; 
extern double t5;
extern double t6;

#define BASE_PIN 1
#define SHOULDER_PIN 2   
#define ELBOW_PIN 3  
#define WRIST_1_PIN 10
#define WRIST_2_PIN 11
#define WRIST_3_PIN 12
#define WHITE_BUTTON 0 //43
#define GREEN_BUTTON 14 //44

// Cartesian Space (3D point vectors of each joint from base to tip)
extern double jointPositions[6][3];

// Graphics
extern int screen_no;

