
// The setup routine runs once when you press reset:
void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // Output US trigger to be sent to pin 13
  pinMode(13, OUTPUT);
}

// User to enter define variables:
String peakSelection = "R";
float inputDelay = 0;
float proportionDelay = 0.5; // Factor of heart rate to delay by

// Initalize variables
int previousValue = 0;
float timeElapsed = 0;
float heartRate = 1;

int Rpeak; int Speak; int Tpeak; int Ppeak;
int Rwait = 0; int Swait = 0; int Twait = 0; int Pwait = 0;
int Rcount = 0; int Scount = 0; int Tcount = 0; int Pcount = 0;
int dydxTCount = 0; int dydxPCount = 0;


// The loop routine runs over and over again forever:
void loop() {
  float start = micros();
  // Read the input on analog pin 0:
  int currentValue = analogRead(A0);
  
  // Simple derivate calculation
  int dydx = currentValue-previousValue;

  // Set of conditions for R-peak detection
  if ((dydx > 50) and (currentValue > 450) and (Rwait > heartRate/2)) {
    Rpeak = 1000;
    Rwait = 0;
    heartRate = (heartRate+timeElapsed)/2;
    timeElapsed = 0;
  }
    else{
      Rwait ++;
      timeElapsed ++;
    }

  // Set of conditions for S-peak detection
  if ((dydx < -100) and Swait > heartRate/2){
    Speak = 1000;
    Swait = 0;
  }
    else{
      Swait ++;
    }
    
  // Set of conditions for T-peak detection
  if ((timeElapsed/heartRate < 0.5) and (timeElapsed/heartRate > 0.15) and (dydx > 0)){
    Twait++;
    dydxTCount++;
  }
  else if ((timeElapsed/heartRate < 0.5) and (timeElapsed/heartRate > 0.15) and (dydx < 0) and (dydxTCount > 3) and (Twait > heartRate/2)){
    Tpeak = 1000;
    Twait = 0;
    dydxTCount = 0;
  }
    else{
      Twait ++;
      dydxTCount = 0;
    }

  // Set of conditions for P-peak detection
  if ((timeElapsed/heartRate > 0.7) and (timeElapsed/heartRate < 0.9) and (dydx > 0)){
    dydxPCount++;
    Pwait++;
  }
  else if ((timeElapsed/heartRate > 0.7) and (timeElapsed/heartRate < 0.9) and (dydx < 0) and (dydxPCount >2) and (Pwait > heartRate/2)){
    Ppeak = 1000;
    Pwait = 0;
    dydxPCount = 0;
  }
    else{
      Pwait ++;
      dydxPCount = 0;
    }

   // Calculate number of sample to delay
   if (proportionDelay != 0){
   inputDelay = proportionDelay*heartRate;
   }

  // Trigger outputs when delay is satisfied (P-trigger)
  if (peakSelection == "P"){
    if ((Ppeak == 1000) and (Pcount >= inputDelay)){
      Serial.print(currentValue);
      Serial.print(",");
      Serial.println(Ppeak);
      digitalWrite(13, HIGH);
      Ppeak = 0;
      Pcount = 0;
    }
      else if ((Ppeak == 1000) and (Pcount < inputDelay)){
        Pcount++;
        Serial.print(currentValue);
        Serial.print(",");
        Serial.println(0);
       }
        else {
          Serial.print(currentValue);
          Serial.print(",");
          Serial.println(0);
        }
}

  // Trigger outputs when delay is satisfied (R-trigger)
  if (peakSelection == "R"){
    if ((Rpeak == 1000) and (Rcount >= inputDelay)){
    Serial.print(currentValue);
    Serial.print(",");
    Serial.println(1000);
    digitalWrite(13, HIGH);
    Rpeak = 0;
    Rcount = 0;
  }
    else if ((Rpeak == 1000) and (Rcount < inputDelay)){
      Rcount++;
      Serial.print(currentValue);
      Serial.print(",");
      Serial.println(0);
      digitalWrite(13, LOW);
    }
      else {
        Serial.print(currentValue);
        Serial.print(",");
        Serial.println(0);
        digitalWrite(13, LOW);
      }
}

  // Trigger outputs when delay is satisfied (S-trigger)
  else if (peakSelection == "S"){
    if ((Speak == 1000) and (Scount >= inputDelay)){
    Serial.print(currentValue);
    Serial.print(",");
    Serial.println(Speak);
    digitalWrite(13, HIGH);
    Speak = 0;
    Scount = 0;
    }
    else if ((Speak == 1000) and (Scount < inputDelay)){
      Scount++;
      Serial.print(currentValue);
      Serial.print(",");
      Serial.println(0);
      digitalWrite(13, LOW);
    }
      else {
      Serial.print(currentValue);
      Serial.print(",");
      Serial.println(0);
      digitalWrite(13, LOW);
      }
}

  // Trigger outputs when delay is satisfied (T-trigger)
  else if (peakSelection == "T"){
   if ((Tpeak == 1000) and (Tcount >= inputDelay)){
    Serial.print(currentValue);
    Serial.print(",");
    Serial.println(Tpeak);
    digitalWrite(13, HIGH);
    Tpeak = 0;
    Tcount = 0;
  }
    else if ((Tpeak == 1000) and (Tcount < inputDelay)){
      Tcount++;
      Serial.print(currentValue);
      Serial.print(",");
      Serial.println(0);
      digitalWrite(13, LOW);
    }
      else {
        Serial.print(currentValue);
        Serial.print(",");
        Serial.println(0);
        digitalWrite(13, LOW);
      }
}

  previousValue = currentValue;

float endtime = micros();
float timetime = endtime-start;
  
  delay(1);        // Delay in between reads for stability

  // Enable to test time of each sample
  //Serial.println(timetime);
}
