/*
 * IEEE Spring 2017 Arduino Workshop - Laser Trip Wire
 * 
 * 
 */

// 5 pins trip wire

// Variables
int mode = 0;       // initializes mode to the setting mode 
int ambiant;        // ambiant light level 
int trip = 500;     // The light value I get when using my laser
int minLight = 400; // arbitrary light value that is used to compare with the laser light value 
int makeBeep = 1;   // 0 for no beep, 1 for beep!
int atAverage;      // laser light value adjused with ambiant light level 
long millisCount;

// Output Pins
int laserPin = 2; 
int ledPin = 13;
int buzzerPin = 3;
String modeNames[3] = {"SETTINGS","ARMED","TRIP"};

// Input Pins
int modePin = 4; 
int tripPin = A0;
int ambiantPin = A1;


void setup() {
  pinMode(ledPin, OUTPUT); //sets LED pin 13 to an output
  pinMode(laserPin, OUTPUT); // sets pin 2 to output data from the photoresistor that the laser will point to 
  pinMode(buzzerPin, OUTPUT); //sets pin 3 to output data from the speaker 
  pinMode(modePin, INPUT); //sets pin 4 to recieve data from the button 
  Serial.begin(9600); //initializes the serial monitor to baud of 9600
}

void loop() {

  // calculates the currents state of the trip wire depending on both the button input and the condition of the trip wire 
  if (digitalRead(modePin) == 1) {
    
    trip = analogRead(tripPin);//determines the condition of the trip wire by reading Analog pin 0 
                               //which is connected to the photo resistor that the laser is pointing at  
                               
    mode=mode + 1; // switches from setting mode to armed mode (1) 
    
    if (mode >= 2) {
      mode = 0; //switches from tripped mode to setting mode 
    }
    beep(1);
    delay(300);
  }

  
  //does something when the mode changes
  switch (mode) {
    
    case 0: //setting state 
      digitalWrite(laserPin,HIGH); //turns the laser on 
      
      ambiant = analogRead(ambiantPin); //reads the ambiant light level from analog pin 1
      trip = analogRead(tripPin); //reads the laser light level from analog pin 0
      atAverage = ambiant + ((trip - ambiant)/2); //adjusts the laser light level with the ambient light level 
      
      stats();//prints relevant data to serial monitor 

      //determines whether the laser has been set or not (LED turns on if set vice versa)
      if (trip  >= minLight) { 
        digitalWrite(ledPin,HIGH);
      } else {
        digitalWrite(ledPin,LOW);
      }

    break;

    case 1: // Armed mode
      digitalWrite(laserPin,HIGH);// turns laser on 
      digitalWrite(ledPin,LOW); // turns LED off 
      
      ambiant = analogRead(ambiantPin); //reads the ambiant light level from analog pin 1
      atAverage = ambiant + ((trip - ambiant)/2); //adjusts the laser light level with the ambiant light level 
      
      if (analogRead(tripPin) < atAverage) //checks to see if laser wire has been tripped 
      
      {
        mode = 2; //changes state to mode 2 if wire has been tripped 
      }
      if ((millis() - millisCount) >= 3000) {
        millisCount = millis();
        stats(); //prints relevant infomation to serial monitor 
      }
    break;
    
    case 2: //Trip Mode
      if ((millis() - millisCount) >= 1000) {
        millisCount = millis();
      
        stats();// prints current stats of the program to serial monitor 
        beep(3); //makes the buzzer sound 
        mode = 1; // returns to alarm state to see if wire has still been tripped 
      }
    break;
    }

  delay(1);                       // wait for a bit
}

//make the buzzer beep
void beep(int qty) {
  int count;
  if (makeBeep == 1) {
    for (count = 1;count<=qty;count++) {
      digitalWrite(buzzerPin, HIGH);
      delay(50);
      digitalWrite(buzzerPin, LOW);
      delay(50);
    }
  }
}

//writes output to serial monitor
void stats() {
  Serial.print("A:");
  Serial.print(ambiant);
  Serial.print(" T:");
  Serial.print(trip);
  Serial.print(" AT:");
  Serial.print(atAverage);
  Serial.print(" MODE:");
  Serial.print(modeNames[mode]);
  Serial.println("");

}


