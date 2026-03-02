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

uint32_t test = 0;
uint32_t test1 = 0;

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

class CounterTimeMashine{
  public:

    static uint32_t _startPointML; 
    static uint32_t _startPointMC;
    
    static uint32_t ReturnCounterML;
    static uint32_t ReturnCounterMC;

  void setZeroML(){
    _startPointML = millis();
  }  

  void setZeroMC(){
    _startPointMC = micros();
  }

  uint32_t MILcountMashin(){
    ReturnCounterML = millis() - _startPointML;
    return ReturnCounterML;
  }

  uint32_t MCRcountMashin (){
    ReturnCounterMC = micros() - _startPointMC;
    return ReturnCounterMC;
  }

};

uint32_t CounterTimeMashine::_startPointMC = 0;
uint32_t CounterTimeMashine::_startPointML = 0;
uint32_t CounterTimeMashine::ReturnCounterML = 0;
uint32_t CounterTimeMashine::ReturnCounterMC = 0;

CounterTimeMashine C_T_M_;

void IRAM_ATTR RightSensorInterapt(){
  test1 = C_T_M_.MCRcountMashin();

}


void setup() {
  
  Serial.begin(115200); // Додаємо монітор порту для діагностики
  //Serial.println("System Started");
  pinMode(SIGNAL, OUTPUT);
  pinMode(RIGHT_SENS, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(RIGHT_SENS), RightSensorInterapt, RISING);
 
}

String inputData = ""; // Рядок для зберігання даних
bool dataReady = false; // Прапорець, що ми отримали повну команду




void loop() {
  //delay(100);
  //Serial.println("System start");

  C_T_M_.setZeroML();
  C_T_M_.setZeroMC();
  
  dataReady = terminal.ReadInput();

  //delay(3);

  //test = C_T_M_.MILcountMashin();
  //test1 = C_T_M_.MCRcountMashin();


  if(dataReady){
    Serial.println("Any data input");
    digitalWrite(SIGNAL, HIGH);

  
    Serial.println(test);
    Serial.println(test1);

  }else{
   // digitalWrite(SIGNAL, LOW);
    //Serial.println(test);
  }

  


 
}


