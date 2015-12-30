
// Control of a mini quadruped robot - Last update: AndreasVan 2015-12-30 Vers. 1.8
// Micro controller = Arduino UNO
// Detecting obstacles with an SR04 ultrasonic sensor 
// Based on Mini quadruped MK II by oevsegneev - thingiverse.com/thing:343420
// this code is public domain, enjoy!


#include <Servo.h>
#include <NewPing.h>


// set names of the servos
/*
 vr_s = front right shoulder
 vr_b = front right leg
 vl_s = front left shoulder
 vl_b = front left leg
 hl_s = rear left shoulder
 hl_b = rear left leg
 hr_s = rear right shoulder
 hr_b = rear right leg
 sono = "servo" for sensor
 */
 
Servo vr_s;
Servo vr_b;
Servo vl_s;
Servo vl_b;
Servo hr_s;
Servo hr_b;
Servo hl_s;
Servo hl_b;
Servo sono;

NewPing sonar(A5, A4, 200);

// set variables 

// set logical values for physical servo home position
int center_vr_s = 95;
int center_vr_b = 85;
int center_vl_s = 80;
int center_vl_b = 90;
int center_hl_s = 85;
int center_hl_b = 90;
int center_hr_s = 90;
int center_hr_b = 95;

int sh = 40;    // movement shoulder
int bn = 28;    // movement leg

int b;
int x;
int w;
int up;
int up2;
int up3;
int down;
int down2;
int steps;
int rightsteps;
int leftsteps;
int back;
int pos;
int dist = 0;

int time;
int step_time = 0;
int step_dir = 0;
boolean step_flag = 0;
boolean red_state = 0;
boolean yellow_state = 0;

int buzz_pin = 1;
int led_pin = 13;
int red_pin_1 = A2;
int red_pin_2 = A3;
int yellow_pin_1 = A0;
int yellow_pin_2 = A1;
int dir_pin_r = 2; 
int dir_pin_l = 4;
int dir_pin_f = 3;

const int STEP_TIMEOUT = 2000;

// setup

void setup(){
    pinMode(buzz_pin, OUTPUT);  // buzzer pin
    pinMode(led_pin, OUTPUT);   // hw led pin

    pinMode(red_pin_1, OUTPUT);  // red LED pin
    pinMode(red_pin_2, OUTPUT);  // red LED pin
    pinMode(yellow_pin_1, OUTPUT);  // yellow LED pin
    pinMode(yellow_pin_2, OUTPUT);  // yellow LED pin
    
    pinMode(dir_pin_r, OUTPUT);  // yellow direction LED pin
    pinMode(dir_pin_l, OUTPUT);  // red direction LED pin    
    pinMode(dir_pin_f, OUTPUT);  // green direction LED pin 
    
    randomSeed( analogRead(A1) );

    // assign servos to pins and center servos

    vr_s.attach(5);
    vr_s.write(center_vr_s);
    vr_b.attach(6);
    vr_b.write(center_vr_b);
    vl_s.attach(7);
    vl_s.write(center_vl_s);
    vl_b.attach(8);
    vl_b.write(center_vl_b);
    hl_s.attach(9);
    hl_s.write(center_hl_s);
    hl_b.attach(10);
    hl_b.write(center_hl_b);
    hr_s.attach(11);
    hr_s.write(center_hr_s);
    hr_b.attach(12);
    hr_b.write(center_hr_b);
  
    //startup 

    digitalWrite(yellow_pin_1, LOW);
    digitalWrite(yellow_pin_2, LOW);
    digitalWrite(red_pin_1, HIGH);
    digitalWrite(red_pin_2, HIGH);  
    digitalWrite(dir_pin_f, HIGH); 

    delay(1500);
    
    digitalWrite(yellow_pin_1, HIGH);
    digitalWrite(yellow_pin_2, HIGH);
    digitalWrite(red_pin_1, LOW);
    digitalWrite(red_pin_2, LOW);  
    digitalWrite(dir_pin_f, LOW); 
    digitalWrite(dir_pin_r, HIGH); 
    digitalWrite(dir_pin_l, HIGH);     
    
    delay(1500);

    digitalWrite(yellow_pin_1, LOW);
    digitalWrite(yellow_pin_2, LOW);
    digitalWrite(red_pin_1, LOW);
    digitalWrite(red_pin_2, LOW); 
    digitalWrite(dir_pin_r, LOW); 
    digitalWrite(dir_pin_l, LOW);   

    delay(1000);

    //sono

    Serial.begin(9600);
    Serial.println("startup");
    blink(10);
}

void beep(){
    digitalWrite(buzz_pin, HIGH);
    delay(50);
    digitalWrite(buzz_pin, LOW);
    delay(50);
}

void blink( int count ){
    for( char i; i<count; i++ ){
        digitalWrite(led_pin, HIGH);
        digitalWrite(buzz_pin, HIGH);
        delay(100);
        digitalWrite(led_pin, LOW);
        digitalWrite(buzz_pin, LOW);
        delay(100);
    }
}

void idle(){
    delay(300);  //  a bigger number slower walking (more delay between the steps ) 
}

void forward() {
    Serial.println("forward");
    digitalWrite(dir_pin_l, LOW);
    digitalWrite(dir_pin_r, LOW);
    digitalWrite(dir_pin_f, HIGH);
    
    hl_b.write(center_hl_b+bn);
    vr_b.write(center_vr_b+bn);
    idle();

// lift front right leg, move front right shoulder forward and rear right shoulder backward, lower front right leg
    vr_s.write(center_vr_s+sh);
    hr_s.write(center_hr_s-sh);
    vl_s.write(center_vl_s);
    hl_s.write(center_hl_s);
    idle();

 // lift rear left leg, move rear left shoulder forward and front right shoulder backward, lower rear left leg
    hl_b.write(center_hl_b);
    vr_b.write(center_vr_b);
    hr_b.write(center_hr_b-bn);
    vl_b.write(center_vl_b-bn);
    idle(); 

  // lift front left leg, move front left shoulder forward and rear left shoulder backward, lower front left leg
    vl_s.write(center_vl_s-sh);
    hl_s.write(center_hl_s+sh);
    vr_s.write(center_vr_s);
    hr_s.write(center_hr_s);
    idle();

// lift rear right leg, move rear right shoulder forward and front left shoulder backward, lower rear right leg
    hr_b.write(center_hr_b);
    vl_b.write(center_vl_b);
    idle();

    yellow_state = !yellow_state;
    digitalWrite(yellow_pin_1, yellow_state);
    digitalWrite(yellow_pin_2, !yellow_state);
    digitalWrite(red_pin_1, LOW);
    digitalWrite(red_pin_2, LOW);
    digitalWrite(dir_pin_r, LOW);
    digitalWrite(dir_pin_l, LOW);      
    digitalWrite(dir_pin_f, LOW);      
}

                          
void step_stop(){
    vr_s.write(center_vr_s+45);
    vl_s.write(center_vl_s-45);
    hr_s.write(center_hr_s-65);
    hl_s.write(center_hl_s+65);

    vr_b.write(center_vr_b+bn);
    vl_b.write(center_vl_b-bn);
    hr_b.write(center_hr_b-bn);
    hl_b.write(center_hl_b+bn);
}

void step( int step_dir ){
    if( step_dir )
        step_right();
    else
        step_left();

    red_state = !red_state;
    digitalWrite(yellow_pin_1, LOW);
    digitalWrite(yellow_pin_2, LOW);
    digitalWrite(red_pin_1, red_state);
    digitalWrite(red_pin_2, !red_state);
}

void step_right(){
    Serial.println("turn right"); 
    digitalWrite(dir_pin_l, LOW);
    digitalWrite(dir_pin_r, HIGH);
    digitalWrite(dir_pin_f, LOW);
    
// lift front right leg, move front right shoulder forward, rear right shoulder backward, lower front right leg
    vr_b.write(center_vr_b+bn);
    idle();
    vr_s.write(center_vr_s+55);
    hr_s.write(center_hr_s-45);
    idle();
    vr_b.write(center_vr_b);
    idle();

// lift rear left leg, move rear left shoulder forward and front right shoulder backward, lower rear left leg 
    hl_b.write(center_hl_b+bn); 
    idle();
    hl_s.write(center_hl_s+45);
    vr_s.write(center_vr_s-5);
    idle();
    hl_b.write(center_hl_b);
    idle();


// lift front left leg, move front left shoulder forward and rear left shoulder backward, lower front left leg
    vl_b.write(center_vl_b-bn);
    idle();
    vl_s.write(center_vl_s-45);
    hl_s.write(center_hl_s+55);
    idle();
    vl_b.write(center_vl_b);
    idle();


// lift rear right leg, move rear right shoulder forward and front left shoulder backward, lower rear right leg  
    hr_b.write(center_hr_b-bn);
    idle();
    hr_s.write(center_hr_s+25);
    vl_s.write(center_vl_s+45);
    idle();
    hr_b.write(center_hr_b);
    idle();

}

void step_left(){
    Serial.println("turn left"); 
    digitalWrite(dir_pin_l, HIGH);
    digitalWrite(dir_pin_r, LOW);
    digitalWrite(dir_pin_f, LOW);
    
// lift front right leg, move front right shoulder forward, rear right shoulder backward, lower front right leg
  vr_b.write(center_vr_b+bn);
  idle();
  vr_s.write(center_vr_s+55);
  hr_s.write(center_hr_s-45);
  idle();
    vr_b.write(center_vr_b);
      idle();

// lift rear left leg, move rear left shoulder forward and front right shoulder backward, lower rear left leg 
    hl_b.write(center_hl_b+bn); 
 idle();
    hl_s.write(center_hl_s+45);
    vr_s.write(center_vr_s-5);
 idle();
    hl_b.write(center_hl_b);
 idle();


// lift front left leg, move front left shoulder forward and rear left shoulder backward, lower front left leg
    vl_b.write(center_vl_b-bn);
  idle();
    vl_s.write(center_vl_s-45);
    hl_s.write(center_hl_s+55);
  idle();
    vl_b.write(center_vl_b);
  idle();


// lift rear right leg, move rear right shoulder forward and front left shoulder backward, lower rear right leg  
    hr_b.write(center_hr_b-bn);
  idle();
    hr_s.write(center_hr_s+25);
    vl_s.write(center_vl_s+45);
  idle();
    hr_b.write(center_hr_b);
  idle();
  
}

void loop(){
    dist = sonar.convert_cm( sonar.ping_median(5) );
    Serial.println(dist);
    time = millis();
    
    if( time - step_time > STEP_TIMEOUT ){
        step_flag = 0;
    }

    if( !step_flag && (!dist || dist > 25) ){
        step_flag = 0;
        forward();
    } else {
        beep();
        if( !step_flag ){
            step_dir = random(1);
            step_flag = 1;
            step_time = millis();
        } else
            step( step_dir );
    }  
}

