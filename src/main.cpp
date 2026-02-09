#include <Arduino.h>

//_________________________________________________________________________________________________________
//                      створення дефайнів для ADC з потенціометром
#define PINPOTENC 1   // пін ADC
#define POTRANGE_MAX 4095 // максимальне значення потенціометра
#define POTRANGE_MIN 1000  // мінамальне значення(потрібно буде щоб підняти нуль)
#define POT_VOLTAGE_MAX 3.3// максимальне знаенння напруги на ADC (про всяк випадок)



// Clear project from new lessons created on bace platform in arduino

// put function declarations here:
int myFunction(int, int);

void setup() {
  //_________________________________________________________________________________________________________
  // налаштовуємо передачу по com потру
  Serial.begin(115200);

  //_________________________________________________________________________________________________________
  // налагтовужмо ADC пін

  analogReadResolution(12); // виставляжмо розширення ADC 0-4095 
  // виставляємо атенюатор ADC на 6db, це дозволить працювати з лініжю 1.8 вольта на максимальному діапазоні ADC
  //analogSetAttenuation(ADC_6db); 
  // встановлення атенюатора на конкретному піні
  analogSetPinAttenuation(PINPOTENC,ADC_ATTENDB_MAX);


}
// змінні для зберігання значень напруги 
float milivotl = 0;
float voltage = 0;

void loop() {
  // put your main code here, to run repeatedly:

  milivotl = analogReadMilliVolts(PINPOTENC);
  voltage = milivotl / 1000;
  // вивід значень напруги LED_RED або в термінал абл в teleplot
  Serial.printf("\nRED-> ");
  Serial.print(voltage);

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}