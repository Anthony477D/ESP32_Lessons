// Перше ДЗ, в якому потрібно зробити поліцейську мигалку


#include <Arduino.h>

// Дефайни в який ми даємо назви пінам щоб далі краще розуміти до яких саме звертаємост
#define LED_PIN_BLUE 14
#define LED_PIN_RED 13


// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);

  Serial.begin(115200);

  pinMode(LED_PIN_BLUE,OUTPUT);
  pinMode(LED_PIN_RED,OUTPUT);
}

int source;
void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(LED_PIN_BLUE , HIGH);
  digitalWrite(LED_PIN_RED , LOW);

  delay(500);

  digitalWrite(LED_PIN_BLUE , LOW);
  digitalWrite(LED_PIN_RED , HIGH);

  delay(500);


   // put your main code here, to run repeatedly:
 
 
 
    Serial.println("HI HITLER  -[:_");
    Serial.println("-[:");
  
  
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}



