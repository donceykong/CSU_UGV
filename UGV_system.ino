/*
Written by:
Doncey Albin

This sketch includes the program the Arduino MEGA2560 utilizes for UGV control for the fire-prevention UGV
designed for Colorado State University, Fort Collins, CO, USA.
*/

// Servo Motors
#include <Servo.h>
Servo trigger;  // Servo object for trigger on turret
Servo steer; //Servo object for steering UGV

// DC Motors
#define enA 9
#define in1 4
#define in2 5
#define enB 10
#define in3 6
#define in4 7
int xAxis, yAxis;
int  x = 0;
int  y = 0;
int driveMotor = 0;

//PixyCam2
#include <Pixy2.h>
#include <PIDLoop.h>
Pixy2 pixy;
PIDLoop panLoop(400, 0, 400, true);
PIDLoop tiltLoop(500, 0, 500, true);

//HC05

//LCD Screen

//Speakers

void setup() {
  trigger.attach(9);
  steer.attach(10);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // We need to initialize the pixy object 
  pixy.init();
  // Use color connected components program for the pan tilt to track 
  pixy.changeProg("color_connected_components");
}

void loop() {
  

}

void userControl(){
// Default value - no movement when the Joystick stays in the center
  x = 510 / 4;
  y = 510 / 4;
  // Read the incoming data from the Joystick, or the master Bluetooth device
  while (Serial.available() >= 2) {
    x = Serial.read();
    delay(10);
    y = Serial.read();
  }
  delay(10);
  // Convert back the 0 - 255 range to 0 - 1023, suitable for motor control code below
  xAxis = x*4;
  yAxis = y*4;
  
  // Y-axis used for forward and backward control
  if (yAxis < 470) {
    // Set Motor A backward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    
    //Y-axis readings for going backward from 470 to 0 into 0 to 255 value for the PWM signal for increasing the motor speed
    driveMotor = map(yAxis, 470, 0, 0, 255);
  }
  else if (yAxis > 550) {
    // Drive forward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    
    // Convert the increasing Y-axis readings for going forward from 550 to 1023 into 0 to 255 value for the PWM signal for increasing the motor speed
    driveMotor = map(yAxis, 550, 1023, 0, 255);
  }
  // If joystick stays in middle the motors are not moving
  else {
    driveMotor = 0;
  }
  
  // X-axis used for steering
  if (xAxis < 470) {

  }
  
  if (xAxis > 550) {

  }
  analogWrite(enA, driveMotor); // Send PWM signal to motor A
}


void pixyControl(){
  static int i = 0;
  int j;
  char buf[64]; 
  int32_t panOffset, tiltOffset;
  
  // get active blocks from Pixy
  pixy.ccc.getBlocks();
  
  if (pixy.ccc.numBlocks)
  {        
    // calculate pan and tilt "errors" with respect to first object (blocks[0]), 
    // which is the biggest object (they are sorted by size).  
    panOffset = (int32_t)pixy.frameWidth/2 - (int32_t)pixy.ccc.blocks[0].m_x;
    tiltOffset = (int32_t)pixy.ccc.blocks[0].m_y - (int32_t)pixy.frameHeight/2; 
  
    // update loops
    panLoop.update(panOffset);
    tiltLoop.update(tiltOffset);
  
    // set pan and tilt servos  
    pixy.setServos(panLoop.m_command, tiltLoop.m_command);
  }  
  else // no object detected, go into reset state
  {
    panLoop.reset();
    tiltLoop.reset();
    pixy.setServos(panLoop.m_command, tiltLoop.m_command);
  }
}

void steerPID(){
  // Create integers for the object's center x-position
  int x_Pos = pixy.ccc.blocks[0].m_x; //Goes from 0 to 316
  
  //Desired Position of object center
  int x_Des = 158; // Center of screen along x-axis
  
  // Error Dynamics Control PID
  int x_Pos_Err = x_Des - x_Pos;
  
  if (x_Pos_Err < 0){
  steer.write(++); 
  }
  if(x_Pos_Err > 0){
  steer.write(--);
  }
}

void buzzer(){
  
}
