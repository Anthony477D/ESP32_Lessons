// В данному завдання потрібно модифікувати код, написаний в першому занятті.
// А саме, добавити зовнішню, та використати внутрішню кнопку
// В звичайному режимі, затримка між світлодіодами 500мс, при настискання ні одну з кнопок 
// затримка стає 1000мс, а при натисканні на іншу - 200мс. 
//(Коли натиснуті обидві, вмикати початковий режим 500мс)

#include <Arduino.h>

#define LED_1 14
#define LED_2 13

#define BUT_LOW 0
#define BUT_HIGH 8


#define MIDLLE_SPD 400
#define LOW_SPD 1200
#define HIGH_SPD 75

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);

  pinMode(BUT_LOW, INPUT_PULLUP);
  pinMode(BUT_HIGH, INPUT_PULLDOWN);

}
int DelaySpeed = MIDLLE_SPD;


void loop() {
 
  
  if(0 == digitalRead(BUT_LOW) ){
    DelaySpeed = HIGH_SPD;
  }else if (1 == digitalRead(BUT_HIGH)){
    DelaySpeed = LOW_SPD;
  } else {
    DelaySpeed = MIDLLE_SPD;
  }

    digitalWrite(LED_1, HIGH);
    digitalWrite(LED_2, LOW);
    delay(DelaySpeed);
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, HIGH);
    delay(DelaySpeed);


}







// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}