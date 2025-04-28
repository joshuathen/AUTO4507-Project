#include "ProjectGroup7.h"

//Toggles connection to UR5e robot
bool toggleUR5Connection()
{
  if (!client.connected()) 
  {
    Serial.print("Connecting to ur5e...");
    int failed_connections = 0;
    while(!client.connect(HOST, PORT)) {
      delay(200);
      Serial.print(".");
      failed_connections++;
      if (failed_connections > 20){
        Serial.println("FAILED TO CONNECT TO ROBOT");
        return false;
      }
    }
    Serial.println("Robot connected :)");
    return true;
  } else {
    Serial.print("Disconnecting from ur5e...");
    while (client.connected()) {
      delay(200);
      Serial.print(".");
      client.stop();
    }
    Serial.println("disconnected :)");   
    return false;
  } 
}

//Toggles connection to UR5e gripper
bool toggleGripperConnection()
{
  if (!gripper_client.connected()) 
  {
    Serial.print("Connecting to gripper...");
    int failed_connections = 0;
    while(!gripper_client.connect(HOST, GRIPPER_PORT)) {
      delay(200);
      Serial.print(".");
      failed_connections++;
      if (failed_connections > 20){
        Serial.println("FAILED TO CONNECT TO GRIPPER");
        return false;
      }
    }
    Serial.println("Gripper connected :)");
    return true;
  } else {
    Serial.print("Disconnecting from Gripper...");
    while (gripper_client.connected()) {
      delay(200);
      Serial.print(".");
      gripper_client.stop();
    }
    Serial.println("disconnected :)");   
    return false;
  } 
}

// sends the moveL command, has option to be in jointspace(jointspace = true) or in pose
// most likely will just need jointspace=true as we are taking angle inputs
bool moveL(float pose[], float a, float v, bool JointSpace) 
{         
  String command = "movel(";

  if (JointSpace == false) {
    command += "p[";
  } else {
    command += "[";
  }
  
  for (int i = 0; i < 6; i++) {
    command += String(pose[i], 3); //rounds angle to 3 decimal place
    if (i != 5) {
      command += ",";
    }
  }
  command += "], a=" + String(a) + ", v=" + String(v) + ")\n";

  Serial.println(command);
  client.print(command);
  client.flush();
  return true;
}

//same as moveL function above but with moveJ
bool moveJ(float pose[], float a, float v, bool JointSpace) 
{         
  String command = "movej(";

  if (JointSpace == false) {
    command += "p[";
  } else {
    command += "[";
  }
  
  for (int i = 0; i < 6; i++) {
    command += String(pose[i], 3); //rounds angle to 3 decimal place
    if (i != 5) {
      command += ",";
    }
  }
  command += "], a=" + String(a) + ", v=" + String(v) + ")\n";

  Serial.println(command);
  client.print(command);
  client.flush();
  return true;
}

//sets the gripper fully open if gripperState=0 or 
//fully closed if gripperState=1, gripperState is toggled in the 
//ProjectGroup7.ino and stored for playback
bool setGripper(bool gripperState)
{
  String command = "SET POS ";
  if (!gripperState) {
    command += "0 ";
  } else {
    command += "255 ";
  }
  command += "\n";

  Serial.println(command);
  gripper_client.print(command);
  gripper_client.flush();
  return true;
 }

//iterates through the playback_angles 2D array containing the angles and gripper states 
//at each time the angles were sampled during moving the controller in form:
//[base angle, shoulder, elbow, wrist1, wrist2, wrist3, gripperState]
//currently using moveJ to move between positions and a 0.5 second delay between each 
//entire state (including the gripperState) can be passed to the moveJ functions as they only iterate through
//the first 6 elements anyways
bool playBack()
{
  //move to start position and wait 5 seconds
  Serial.println("Moving to start position");
  moveJ(playback_states[0], a, v, true);
  delay(5000);

  Serial.println("Playback started");
  for (int i = 1; i < playbackIndex; i++) {
    moveJ(playback_states[i], a, v, true);
    if (playback_states[i][6] != playback_states[i-1][6]) {
      setGripper(playback_states[i-1][6]);
    }

    //small delay to allow robot into new position
    delay(sampleRate);
  }
  Serial.println("Playback finished");
  return true;
}

float mapFloat(int x, int in_min, int in_max, float out_min, float out_max) {
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

