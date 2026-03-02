/*
1) реалізувати програмний PWM
2) створити три канали PWM з одного таймера та керувати їх скважністю незалежно один від одного
3) створити функцію яка зчитує значення котрі відправляються з терміналу
4) керувати кольором RGB світлодіода з терміналу вказуючи сквважність PWM сигналк

*/

#include <Arduino.h>

#define SIGNAL 12
#define RIGHT_SENS 17
#define LEFT_SENS 18


class ReadData{
  
    public:
    bool flag = false;


    // функція що кидає маркер коли на uart надходять будь які данні
  bool ReadInput() {

    if (Serial.available() > 0){ 
      flag = true;
      //перечитуєсо весь буфер щоб зкинулись всі данні
      while (Serial.available() > 0) {
        Serial.read(); 
      }
      return flag;

    }else{
      flag = false;
      return flag;
    }

  }


};

ReadData terminal;


void setup() {
  Serial.begin(115200); // Додаємо монітор порту для діагностики
  //Serial.println("System Started");
  pinMode(SIGNAL, OUTPUT);


}

String inputData = ""; // Рядок для зберігання даних
bool dataReady = false; // Прапорець, що ми отримали повну команду

void loop() {
  Serial.println("System loop");
  delay(500);
  dataReady = terminal.ReadInput();

  if(dataReady){
    Serial.println("Any data input");
    digitalWrite(SIGNAL, HIGH);

  }else{
    digitalWrite(SIGNAL, LOW);
  }




 
}
