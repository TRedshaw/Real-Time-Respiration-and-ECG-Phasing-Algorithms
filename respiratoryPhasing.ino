//# TO BE CHOSEN BY USER #

// Choice between exhalation or inhalation phase
//String user_resp_choice = "exhale";
String user_resp_choice = "inhale";

// Choice of phase length (for combination with cardiac phasing)
const int phase_length = 100;

int resp_values[3]; // Save the most recent 3 values for peak detection
float RESP_PEAK_THRESHOLD = 800; // Set initial inhale threshold
float RESP_TROUGH_THRESHOLD = 200; // Set initial exhale threshold
const unsigned long triggerPause = 1000; // Set pause length to avoid multiple triggers per phase
unsigned long previousBreathTime = 0; // Initialise time of previous breath

int prev5_peaks[5] = {500,500,500,500,500}; // Initialise array to save the previous 5 trigger locations
float current_time = millis();

// Initialise times for trigger output
float next_stop_time = current_time;
float next_start_time = current_time;
int i = 0;

// Setup code to run once:
void setup() {

  // Set baud rate
  Serial.begin(9600);

  // Set up input and output pins
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(4, OUTPUT);

}


// Function to add a new element to the end of an array and delete the 0th element
void shiftArray(int arr[], int newElement, int size) {
  // Shift all elements one place to the left, deleting 0th
  for (int i = 0; i < size - 1; i++) {
    arr[i] = arr[i + 1];
  }
  // Add the new element to the end
  arr[size - 1] = newElement;
}

// Function to calculate the average of all values in an array
int avgArray(int arr[], int size) {
  int sum = 0;
  // Loop through each element and add it to the sum
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  // Divide the sum by the number of elements in array
  int avg = sum/size;

  return avg;
}

// Main code to run repeatedly:
void loop() {

  // Read the real-time respiratory value and print to monitor
  int current_resp_value = analogRead(A0);
  Serial.print(current_resp_value);

  // Update the array storing the most recent 3 respiratory values
  shiftArray(resp_values, current_resp_value, 3);
  float current_time = millis();

  // After two phases have been detected, start the triggering output
  if (i > 2){
    
    // Output zeros when not in selected phase
    if ((current_time > next_stop_time) || (current_time < next_start_time)){
      Serial.print(", ");
      Serial.print("0");

    }
    // Output trigger at selected phase
    if ((current_time > next_start_time) && (current_time < next_stop_time)){
      Serial.print(", ");
      Serial.print("1000");

    }
  }
  Serial.println("");

  // Peak detection for if user chooses inhalation phase
  if (user_resp_choice == "inhale") {
    // Local peak detection above threshold if trigger pause has elapsed
    if ((resp_values[1] >= RESP_PEAK_THRESHOLD) && (resp_values[1] >= resp_values[2])
      && (resp_values[1] > resp_values[0]) && (millis()-previousBreathTime > triggerPause)) {
        
        i++; // Increment peak count
        shiftArray(prev5_peaks, resp_values[1], 5); // Save new peak value
        int prev5_peaks_average = avgArray(prev5_peaks, 5); // Calculate new cycle length average
        RESP_PEAK_THRESHOLD = 0.9*prev5_peaks_average; // Adapt threshold

        // Reset times for trigger output
        next_start_time = current_time;
        next_stop_time = current_time + phase_length;
        previousBreathTime = current_time;
    }      
  }  

  // Trough detection for if user chooses exhalation phase
  if (user_resp_choice == "exhale") {
    // Local trough detection below threshold if trigger pause has elapsed
    if ((resp_values[1] <= RESP_TROUGH_THRESHOLD) && (resp_values[1] <= resp_values[2])
      && (resp_values[1] < resp_values[0]) && (millis()-previousBreathTime > triggerPause)) {
      
        i++; // Increment peak count
        shiftArray(prev5_peaks, resp_values[1], 5); // Save new trough value
        int prev5_peaks_average = avgArray(prev5_peaks, 5); // Calculate new cycle length average
        RESP_TROUGH_THRESHOLD = 0.9*prev5_peaks_average; // Adapt threshold

        // Reset times for trigger output
        next_start_time = current_time;
        next_stop_time = current_time + phase_length;
        previousBreathTime = current_time;
    }

  }  
  delay(1);
}
