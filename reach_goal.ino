# include "NewPing.h"
#include <TimerOne.h>


//front sensor
#define TRIGGER_PIN  11
#define ECHO_PIN     12

//right sensor
#define TRIGGER_PINR  A0 
#define ECHO_PINR     A1 

//left sensor
#define TRIGGER_PINL  A2 
#define ECHO_PINL     A3 

#define MAX_DISTANCE 200

#define row    5
#define column 5



NewPing sonarLeft(TRIGGER_PINL, ECHO_PINL, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarRight(TRIGGER_PINR, ECHO_PINR, MAX_DISTANCE);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// duration and distance for 3 sensors, later used to calculate more accurate value
float duration_F, distance_F;
float duration_L, distance_L;
float duration_R, distance_R;


//controlling the motors
// left motor when looking from back
int enA            = 5;            // PWM pin
int goForward_1    = 7;           
int goBackward_1    = 8;

// motor B 
int enB            = 6;            // pwm
int goForward_2    = 9;
int goBackward_2   = 10;

int left_motor_speed = 55;      // pwm between 0 - 255
int right_motor_speed = 55 ;

int delay_yes_L;
int delay_yes_R;

// encoder variables
int counter_left = 0;
int counter_right = 0;

char state; // states are 'l'(left), 'r'(right), 'f'(forward), 's'(stop)

//create maze array for 6*6 maze with 0 as goal (maze[3][3])
// x is row (horizontal), y is column(vertical)
int maze[row][column] = {                // y co-ordinates
                           {4,3,2,3,4},  // 1
                           {3,2,1,2,3},  // 2
                           {2,1,0,1,2},  // 3
                           {3,2,1,2,3},  // 4 
                           {4,3,2,3,4},  // 5
       // x co-ordinates    1 2 3 4 5 
};

int current_position_x = 1;
int current_position_y = 5;
int goal_x= 3; int goal_y = 3;  // should be 3 , 3
int goal_found = 0;
char dir;

// variable stores which way the mouse is pointing at every square
int pointing = 8; // 8 is north, 2 is south, 4 is west , 6 is east


void setup() {
 
   Serial.begin(9600);
  pinMode (enA, OUTPUT);
  pinMode (enB, OUTPUT);
  pinMode (goForward_1, OUTPUT);
  pinMode (goBackward_1 , OUTPUT);
  pinMode (goForward_2, OUTPUT);
  pinMode (goBackward_2, OUTPUT);

//Timer1.initialize(3000000); // set timer for 3 sec

//attach encoders to interrupts
  attachInterrupt(0, count_left, RISING);  // increase counter when speed sensor pin goes High
  attachInterrupt(1, count_right, RISING); // increase right encoder
//  Timer1.attachInterrupt( timerIsr ); // enable the timer
//   Serial.println("Rotations per second");
}



/*********** Encoder Variables *************************/
void count_left()  // counts from the speed sensor
{
  counter_left++;  // increase +1 the counter value
} 

void count_right()
{
  counter_right++;
  }

/*******************************************************/

void loop() {

//print array values
/*
int i,j;
for (i =0; i< row; i++){
  for (j = 0; j < column; j++){
    Serial.print( maze[i][j]);
    }
    Serial.print("\n");
  }
*/

if(counter_left >= 1500){  // 1600 ticks of encoder reading is equal to distance to another square block
  
   moveStop();
    delay(1000);

   
 if (pointing == 8){ // if its pointing north then y coordinates - 1
  current_position_y = current_position_y - 1;
  }
  else if (pointing == 6){  // if its pointing east then x coordinates + 1
    current_position_x = current_position_x + 1;
    }
    else if (pointing == 4){ // if its pointing west then x coordinates - 1
      current_position_x = current_position_x - 1;
      }
      else if (pointing == 2 ){ // if its pointing south then y coordinates + 1
        current_position_y = current_position_y + 1 ;
        }


// goal is center of maze [3,3]
if(current_position_x == goal_x && current_position_y == goal_y){ 
    Serial.print("\n Goal found! \n");

    // turn around for a bit
    moveStop();
    delay(1000);
    turn_360();
    moveStop();
    delay(1000);
    turn_360();
    moveStop();
    delay(1000);
    turn_360();
    moveStop();
    goal_found = 1;
    }

  
  counter_left = 0;  
  }

// read the 3 sensors 

   read_front_sensor();
   read_left_sensor();
   read_right_sensor();

  // set the values for state
   if (distance_F <= 5.00) {state = 's';} //if wall in front then stop
   if(distance_L <= 4.50 ) {state = 'r';} // turn right 
   if(distance_R <= 3.50) {state = 'l';} // turn left
   if ( distance_L > 4.50  && distance_R > 3.50 && distance_F > 5.00) {state = 'f';} //go forward
   
 // if (distance_L >= 7.00){  state = 'o'; }  // if wall open on left

switch(state){
  
  case 's': 

  Serial.print("\n Case 0: stop! \n");
  moveStop();
  read_left_sensor();
   read_right_sensor();
  if (distance_L <= 7.00 && distance_R <= 7.00){ // if wall on both sides turn around 180
   turn_360();
    dir = 'B';  
      if (pointing == 8){ // if initially facing north, then facing south now
      pointing = 2; 
      } 
      else if (pointing == 2){// if initially facing south, then facing north now
        pointing = 8;
        }
        else if (pointing == 4){// if initially facing west, then facing east now
          pointing = 6;
          }
          else if (pointing = 6){// if initially facing east, then facing west now
            pointing = 4;
            }
    }
    
 else if ((distance_L > distance_R) || (distance_L >= 7.00 && distance_R >= 7.00)){ // turn right in a "T" junction or when wall on right
    turnLeft(1);
    dir = 'L';

    
      if (pointing == 8){ // if initially facing north, then facing west now
      pointing = 4; 
      } 
      else if (pointing == 4){// if initially facing west, then facing south now
        pointing = 2;
        }
        else if (pointing == 2){// if initially facing south, then facing east now
          pointing = 6;
          }
          else if (pointing = 6){// if initially facing east, then facing north now
            pointing = 8;
            }

    }
    else {
      turnRight(1); // turn right when cant turn left or go straight
      dir = 'R';
      
      if (pointing == 8){ // if initially facing north, then facing east now
      pointing = 6; 
      } 
      /*********** this was (pointing == 8) before??!!! ******************************************/
      else if (pointing == 6){// if initially facing east, then facing south now
        pointing = 2;
        }
        else if (pointing == 2){// if initially facing south, then facing west now
          pointing = 4;
          }
          else if (pointing = 4){// if initially facing west, then facing north now
            pointing = 8;
            }
      }

      
  break;

  case 'r': 
    Serial.print("\n Case 5: turn slight right! \n");

  turnRight(0);
   dir = 'F';
  break;

  case 'l':
      Serial.print("\n Case 10: turn slight left! \n");

  turnLeft(0);
   dir = 'F';
  break;

  case 'f': 

      Serial.print("\n Case 1: forward! \n");
    dir = 'F';
  moveForward();
  break;
  /*
  case 'o':
  moveStop();
  delay(1000);
  break;
  */
  }
  


}

/*
void floodfill(int x, int y){
  
  if(current_position_x == goal_x && current_position_y == goal_y){
    Serial.print("\n Goal found! \n");
    goal_found = 1;
    
    }

 
  
  }

*/
/******************* Read Sensors ***************************************************/

void read_front_sensor(){
  duration_F = sonar.ping(); // get newping value

  // use 343 m/s as speed of sound
  distance_F = (duration_F / 2)* 0.0343;
  
  Serial.print("Front: ");
  Serial.print(distance_F);
//  delay(50);
  }
  
void read_left_sensor(){
  
  duration_L = sonarLeft.ping(); //get newping value
  distance_L = (duration_L / 2)* 0.0343;
  
  Serial.print("      Left: ");
  Serial.print(distance_L);
//    delay(50);

  }

 void read_right_sensor(){
    duration_R = sonarRight.ping();
  distance_R = (duration_R / 2)* 0.0343;
   Serial.print("      Right: ");
  Serial.print(distance_R);
  Serial.print("\n\n");
//    delay(50);

  }



/********************** MOTOR DIRECTIONS ***************************************************/
 void moveForward(){
// left motor (motor a ) 
digitalWrite(goForward_1 , HIGH);// turn motor A on 
digitalWrite(goBackward_1 , LOW);
analogWrite(enA, left_motor_speed);    // set speed of possible range 0-255 


digitalWrite(goForward_2  , HIGH);// turn motor B on
digitalWrite(goBackward_2, LOW);
analogWrite(enB, right_motor_speed);   // set speed  of possible range 0-255

//Serial.print("going forward. \n\n");

  }

  void moveStop(){

digitalWrite(goForward_1 , LOW);  // turn motor A off
digitalWrite(goBackward_1, LOW);

digitalWrite(goForward_2 , LOW);  // turn motor B off
digitalWrite(goBackward_2, LOW);

  delay(300);
  }


void turnRight(int delay_yes_R ){
if (delay_yes_R == 1){
digitalWrite(goForward_1 , HIGH);// turn motor A on clockwise 
digitalWrite(goBackward_1, LOW);
analogWrite(enA, 55);    // set speed to 100 of possible range 0-255 
digitalWrite(goForward_2 , LOW);// turn motor B(right) anticlockwise
digitalWrite(goBackward_2, HIGH);
analogWrite(enB, 55);   // set speed to 100 of possible range 0-255

Serial.print("Turn right. \n\n");

delay(1100);   // delay value 
 counter_left = 0;
}
else{
  digitalWrite(goForward_1 , HIGH);// turn motor A on clockwise 
digitalWrite(goBackward_1, LOW);
analogWrite(enA, 40);    // set speed to 100 of possible range 0-255 
digitalWrite(goForward_2 , LOW);// turn motor B(right) anticlockwise
digitalWrite(goBackward_2, HIGH);
analogWrite(enB, 40);   // set speed to 100 of possible range 0-255
  }


  }

  
void turnLeft(int delay_yes_L){

  if(delay_yes_L == 1){
digitalWrite(goForward_1 , LOW);// turn motor A(left) anticlockwise
digitalWrite(goBackward_1, HIGH);
analogWrite(enA, 55);    // set speed to 100 of possible range 0-255 
digitalWrite(goForward_2 , HIGH);// turn motor B(right) opposite 
digitalWrite(goBackward_2, LOW);
analogWrite(enB, 55);   // set speed to 100 of possible range 0-255

Serial.print("Turn left. \n\n");

delay(1050);  
 counter_left = 0;
  }

    else {
      digitalWrite(goForward_1 , LOW);// turn motor A(left) anticlockwise
digitalWrite(goBackward_1, HIGH);
analogWrite(enA, 40);    // set speed to 100 of possible range 0-255 
digitalWrite(goForward_2 , HIGH);// turn motor B(right) opposite 
digitalWrite(goBackward_2, LOW);
analogWrite(enB, 40);   // set speed to 100 of possible range 0-255

      }

     
  }

  
void turn_360 (){
  
  digitalWrite(goForward_1 , LOW);// turn motor A(left) anticlockwise
digitalWrite(goBackward_1, HIGH);
analogWrite(enA, 55);    // set speed to 100 of possible range 0-255 
digitalWrite(goForward_2 , HIGH);// turn motor B(right) opposite 
digitalWrite(goBackward_2, LOW);
analogWrite(enB, 55);   // set speed to 100 of possible range 0-255

Serial.print("Turn left. \n\n");

delay(2250);  
counter_left = 0;
  }
