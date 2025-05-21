#include "ProjectGroup7.h"

double t1 = 0.0; 
double t2 = 0.0; 
double t3 = 0.0; 
double t4 = 0.0; 
double t5 = 0.0;
double t6 = 0.0;
double jointPositions[6][3];

// Debug function to print Cartesian Space
void printJoints() {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 3; j++) {
      Serial.println(jointPositions[i][j]);
    }
  }
}


//Reads analog inputs and converts to angles(rad) and updats "angles" array
//ANGLES STORED IN RADIANS
void getJointAngles() {
  for (int i = 0; i < 6; i++) {
    prev_angles[i] = angles[i];
  } 
  t1 = mapFloat(analogRead(BASE_PIN), 0, 4095, -142.0, 170.73) * PI / 180; //mapFloat(analogRead(BASE_PIN), 0, 4095, -150.0, 150.0) * PI / 180;
  t2 = mapFloat(analogRead(SHOULDER_PIN), 1040, 3300, 0, -180) * PI/180; //(-90*PI/180) - mapFloat(analogRead(SHOULDER_PIN), 0, 4095, -150.0, 150.0) * PI / 180;
  //Serial.println(String(t1) + " and " + String(t2));
  t3 = mapFloat(analogRead(ELBOW_PIN), 0, 4095, -150.0, 150.0) * PI / 180;
  t4 = mapFloat(analogRead(WRIST_1_PIN), 0, 4095, -150.0, 150.0) * PI / 180;
  t5 = mapFloat(analogRead(WRIST_2_PIN), 0, 4095, -90.0, 90.0) * PI / 180;
  t6 = mapFloat(analogRead(WRIST_3_PIN), 0, 4095, -90.0, 90.0) * PI / 180;

  angles[0] = t1; //0;//t1;//0             //  map(reading, angle), (1855,0), (3037,90), (680,-90), (0,-150), (4095,150) 
  angles[1] = t2;//-90*PI/180.0;// t2//  map(reading, angle), (2295, -90), (1040, 0), (3300, -180) 
  angles[2] = t3;//0; //t3;
  angles[3] =t4;//-90*PI/180.0; // map(reading, angle), (1433, -90), 
  angles[4] = t5;//0;
  angles[5] = t6;//0;
  //Serial.println("Reading:" + String(analogRead(WRIST_1_PIN)) + " MA1:" + String(t4 * 180.0 / PI, 1));
  delay(300);
}

// Updates Cartesian Space array (in-place) using joint angles t1-t6
void forwardKinematics() {
  jointPositions[0][0] = 0.0;
  jointPositions[0][1] = 0.0;
  jointPositions[0][2] = 0.089159;

  jointPositions[1][0] = -0.425*cos(t1)*cos(t2);
  jointPositions[1][1] = -0.425*cos(t2)*sin(t1);
  jointPositions[1][2] = 0.089159-0.425*sin(t2);

  jointPositions[2][0] = cos(t1)*(-0.425*cos(t2)-0.39225*cos(t2+t3));
  jointPositions[2][1] = (-0.425*cos(t2)-0.39225*cos(t2+t3))*sin(t1);
  jointPositions[2][2] = 0.089159-0.425*sin(t2)-0.39225*sin(t2+t3);

  jointPositions[3][0] = cos(t1)*(-0.425*cos(t2)-0.39225*cos(t2+t3))+0.10915*sin(t1);
  jointPositions[3][1] = -0.10915*cos(t1)+(-0.425*cos(t2)-0.39225*cos(t2+t3))*sin(t1);
  jointPositions[3][2] = 0.089159-0.425*sin(t2)-0.39225*sin(t2+t3);

  jointPositions[4][0] = 0.10915*sin(t1)+cos(t1)*(-0.425*cos(t2)-0.39225*cos(t2+t3)+0.09465*sin(t2+t3+t4));
  jointPositions[4][1] = -0.10915*cos(t1)+sin(t1)*(-0.425*cos(t2)-0.39225*cos(t2+t3)+0.09465*sin(t2+t3+t4));
  jointPositions[4][2] = 0.089159-0.09465*cos(t2+t3+t4)-0.425*sin(t2)-0.39225*sin(t2+t3);

  jointPositions[5][0] = (0.10915+0.0823*cos(t5))*sin(t1)+cos(t1)*(-0.425*cos(t2)-0.39225*cos(t2+t3)+0.09465*sin(t2+t3+t4)+0.04115*sin(t2+t3+t4-t5)-0.04115*sin(t2+t3+t4+t5));
  jointPositions[5][1] = cos(t1)*(-0.10915-0.0823*cos(t5))+sin(t1)*(-0.425*cos(t2)-0.39225*cos(t2+t3)+0.09465*sin(t2+t3+t4)+0.04115*sin(t2+t3+t4-t5)-0.04115*sin(t2+t3+t4+t5));
  jointPositions[5][2] = 0.089159-0.09465*cos(t2+t3+t4)-0.04115*cos(t2+t3+t4-t5)+0.04115*cos(t2+t3+t4+t5)-0.425*sin(t2)-0.39225*sin(t2+t3);
}