//**************************************************************************************************** 
//**************************************************************************************************** 

// Гілка для роботи на уроках 
//**************************************************************************************************** 

#include <Arduino.h>
// включкння бібліотек

// створення дуфайнів
#define ADC 1
#define THRESHHOLD_VOLTAGE 255

#define MAX_DIGITAL 4095.0
#define U_REFERENCE 3.1

/*



*/
 


// Clear project from new lessons created on bace platform in arduino


//**************************************************************************************************** 
// put function declarations here:
int myFunction(int, int);


//**************************************************************************************************** 
void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);

  Serial.begin(115200);
  delay(1000);
  Serial.println("ADC Ready!");

  analogSetAttenuation(ADC_11db);
}






//**************************************************************************************************** 
void loop() {
  // put your main code here, to run repeatedly:
  int rawValue = analogRead(ADC);
  float voltage = (rawValue / MAX_DIGITAL) * U_REFERENCE; 

  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage);
  //if(voltage>=THRESHHOLD_VOLTAGE) {
      digitalWrite(ADC , HIGH); // Turn the LED ON
  //} else {
      digitalWrite(ADC , LOW); // Turn the LED ON
 // }

 delay(500);

}



//**************************************************************************************************** 

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}