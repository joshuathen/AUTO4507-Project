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


// Get and map joint angles from arduino (also calls functions to update Cartesian Space values and display)
void getJointAngles() {
  t1 = mapFloat(analogRead(ANALOG_PIN_1), 0, 4095, -90.0, 90.0) * PI / 180;
  t2 = mapFloat(analogRead(ANALOG_PIN_2), 0, 4095, -90.0, 90.0) * PI / 180;
  t3 = mapFloat(analogRead(ANALOG_PIN_3), 0, 4095, -150.0, 150.0) * PI / 180;
  t4 = mapFloat(analogRead(ANALOG_PIN_4), 0, 4095, -90.0, 90.0) * PI / 180;
  t5 = mapFloat(analogRead(ANALOG_PIN_5), 0, 4095, -90.0, 90.0) * PI / 180;
  t6 = mapFloat(analogRead(ANALOG_PIN_6), 0, 4095, -90.0, 90.0) * PI / 180;
  forwardKinematics();
  updateDisplay(0);
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

  // Update display (but don't change the screen_no)
  updateDisplay(0);
}