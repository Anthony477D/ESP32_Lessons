#include <Arduino.h>

//_________________________________________________________________________________________________________
//                      створення дефайнів для ADC з потенціометром
#define PINPOTENC 1   // пін ADC
#define POTRANGE_MAX 4095 // максимальне значення потенціометра
#define POTRANGE_MIN 0  // мінамальне значення(потрібно буде щоб підняти нуль)
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
  analogSetPinAttenuation(PINPOTENC,ADC_ATTENDB_MAX); // виставляємо значення атенюатора на максимум


}
// змінні для зберігання значень напруги 
float milivotl = 0;

int RAW_ADC = 0;
float mathdata = 0;
float coef = (POTRANGE_MAX - POTRANGE_MIN) / POT_VOLTAGE_MAX;

float delta = 0;
float a = 0;

void loop() {

  // зчитуємо та виводимо данні напряму
  RAW_ADC = analogReadRaw(PINPOTENC);
  Serial.printf("\nRAW-> ");
  Serial.print(RAW_ADC);

  // розраховуєио та вивиодимо значення в вольтпх
  mathdata = RAW_ADC / coef;
  Serial.printf("___MATH-> ");
  Serial.print(mathdata);

  // виводимо значення в мілівольиах через програмну функцію
  milivotl = analogReadMilliVolts(PINPOTENC); // зчитуємо значення
  Serial.printf("___FUNC_mv-> ");
  Serial.print(milivotl);

  // розраховуємо та виводимо похибку
  a = milivotl / 1000;
  delta = ((mathdata - a)/a) * 100;

  Serial.printf("___DELTA-> ");
  Serial.print(delta);

  delay(100);

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}