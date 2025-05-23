/* 
--------------------------------------------  
-------- MECH1010 Coursework 2024 --------
-------- Name:  Thomas del Rio
-------- Username:  mn23tdr
--------------------------------------------
*/
int gled_pin = 2 ;
int yled_pin = 3 ;
int rled_pin = 4 ;
int hbridgedirection_pin = 9 ;
int hbridgeenable_pin = 10 ;
int pot_pin = A0 ;
float pot_value = 0 ;
float supportrod = 0.491;
float target_height = 0.05 + (supportrod*(sin(68.5*(PI / 180)))) ; 
float neutral_height = supportrod*(sin(68.5*(PI / 180))) ;
int Kp = 1850 ;
unsigned long start_time = 0;
float elapsed_time = 0 ;
float pmw_in = 0 ;
int count = 0 ;
float max_error = 0;
float total_error = 0;
float average_error = 0 ;
int num_turns = 0 ;
unsigned long time_to_target = 0 ;



void setup() {
  Serial.begin(9600);


	pinMode(gled_pin, OUTPUT);
  pinMode(yled_pin, OUTPUT);
  pinMode(rled_pin, OUTPUT);
  pinMode(hbridgedirection_pin, OUTPUT);
  pinMode(hbridgeenable_pin, OUTPUT);
  pinMode(pot_pin, INPUT);


	//turning on LEDs for 0.5 seconds 
digitalWrite(gled_pin, HIGH);
digitalWrite(yled_pin, HIGH);
digitalWrite(rled_pin, HIGH);
delay(500);
digitalWrite(gled_pin, LOW);
digitalWrite(yled_pin, LOW);
digitalWrite(rled_pin, LOW);

Serial.println("System Started");
Serial.println("System Initiated");
Serial.println("Controller Starting");
Serial.println("Time, Angle, Height, Error"); 

unsigned long start_time = millis();

}



void loop() {

// timing the controller 
unsigned long current_time = millis(); 

do{

  //timing of the loop
unsigned long loop_start_time = millis() ;
elapsed_time = loop_start_time - start_time ;

// reading the angle and converting to current height of the drone
 float pot_value = analogRead(pot_pin) ;
 float angularposition = map(pot_value, 212, 410, -68.5, 0); 
 float current_height = supportrod*(sin(angularposition*(PI / 180)));

// making sure the current height is correct
if (-68.5 < angularposition < 0) {
  angularposition = abs(angularposition);
  current_height = neutral_height - (supportrod*(sin(angularposition*(PI / 180)))) ;
}
else if (angularposition > 0){
  current_height = neutral_height + (supportrod*(sin(angularposition*(PI / 180)))) ; 
}
// calculating height error
 float height_error = target_height - current_height ;


// setting direction and speed for motor
digitalWrite(hbridgedirection_pin, LOW);
pmw_in = current_height + (abs(height_error)*Kp) ;
float pmw_out = map(pmw_in, 0, (1023*1.85), 0, 255) ;
analogWrite(hbridgeenable_pin, pmw_out);



// Lighting for the errors 
if (-0.02 < height_error && height_error < 0.02) {
digitalWrite(gled_pin, HIGH);
digitalWrite(yled_pin, LOW);
digitalWrite(rled_pin, LOW);
}
else if (height_error < -0.02) {
  digitalWrite(yled_pin, HIGH);
  digitalWrite(gled_pin, LOW);
  digitalWrite(rled_pin, LOW);
}
else  {
  digitalWrite(rled_pin, HIGH);
  digitalWrite(gled_pin, LOW);
  digitalWrite(yled_pin, LOW);
}

//statistics calculations 

if ((-0.02 < height_error) && (height_error < 0.02) && (count == 0)) {
  time_to_target = elapsed_time ; 
  count = count + 1 ;
}

if (count > 0) {

 total_error = abs(total_error) + abs(height_error);
 num_turns = num_turns + 1 ;  


if (abs(height_error) > max_error) {
  max_error = abs(height_error);
}
}




//timings in seconds
elapsed_time = elapsed_time/1000 ;
time_to_target = time_to_target / 1000 ;

// Printing the Data
Serial.print(elapsed_time, 2);
Serial.print(",");
Serial.print(angularposition, 2);
Serial.print(",");
Serial.print(current_height);
Serial.print(",");
Serial.println(height_error, 2);


delay(40) ; 

 }while (elapsed_time <= 5) ;

//shutoff process

//Turning all LEDs on for 1 second 
digitalWrite(gled_pin, HIGH);
digitalWrite(yled_pin, HIGH);
digitalWrite(rled_pin, HIGH);
delay(1000);
digitalWrite(gled_pin, LOW);
digitalWrite(yled_pin, LOW);
digitalWrite(rled_pin, LOW);

//calculating average error
average_error = total_error / num_turns ;

//printing statistics
Serial.println("Shutdown");
Serial.print("Maximum Error: ");
Serial.println(max_error, 2);
Serial.print("Average Error: ");
Serial.println(average_error, 2);
Serial.print("Time taken to first reach target: ") ;
Serial.println(time_to_target, 2);


//turn off motor
digitalWrite(hbridgedirection_pin, HIGH);
analogWrite(hbridgeenable_pin, 0);

delay(1000000) ;
}
