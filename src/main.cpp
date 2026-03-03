/*
заміряти значення спрацювання реле, як на позицію ON так і на позицію OFF
Виміряти час перемикання реле з пощмції ON в OFF між відповідними виводами
*/

#include <Arduino.h>

#define SIGNAL 12
#define RIGHT_SENS 17
#define LEFT_SENS 18

volatile uint32_t RelayOff = 0;
volatile uint32_t RelayOn = 0;

class ReadData{
  
    public:
    bool flag = false;

    // функція що кидає маркер коли на uart надходять будь які данні
  bool ReadInput() {

    if (Serial.available() > 0){ 
      flag = true;

      //перечитуєсо весь буфер щоб зкинулись всі данні, інакше нам весь час будуть слати що данні в буфері
      // після перечитування данних буфер uart зкидується в 0
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

// set static data to zero
uint32_t CounterTimeMashine::_startPointMC = 0;
uint32_t CounterTimeMashine::_startPointML = 0;
uint32_t CounterTimeMashine::ReturnCounterML = 0;
uint32_t CounterTimeMashine::ReturnCounterMC = 0;

CounterTimeMashine C_T_M_;

/*
створюємо переривання де фіксуємо час коли спрацювання а нуль виставляємо власноруч
*/
void IRAM_ATTR RightSensorInterapt(){
  RelayOn = C_T_M_.MCRcountMashin();
}
void IRAM_ATTR LeftSensorInterapt(){
  RelayOff = C_T_M_.MCRcountMashin();
}


void setup() {
  
  Serial.begin(115200); // Додаємо монітор порту для діагностики та виводу інформації
  
  pinMode(SIGNAL, OUTPUT); // config relay control pin

  pinMode(RIGHT_SENS, INPUT_PULLDOWN);  // config relay read pin ON mode 
  pinMode(LEFT_SENS, INPUT_PULLDOWN);   // config relay read pin OFF mode

  attachInterrupt(digitalPinToInterrupt(RIGHT_SENS), RightSensorInterapt, RISING);
  attachInterrupt(digitalPinToInterrupt(LEFT_SENS), LeftSensorInterapt, RISING);
}


bool dataReady = false; // Flag to resive any terminal data
uint8_t StateStatus = 1; // relay mode flag ON/OFF

uint32_t deltaON = 0;
uint32_t deltaOFF = 0;
uint32_t deltaLocal = 0;
bool flagON = false;
bool flagOFF = false;

void loop() {

  // set zero pint CounterMashine to zero
  C_T_M_.setZeroMC();
  
  // read data in terminal; When we have the bool is TRUE, esle nothing data the bool is FALSE
  dataReady = terminal.ReadInput();
  

  // detect and meas relay switch ON
  if(dataReady == true && StateStatus == 1){
    
    digitalWrite(SIGNAL, HIGH);

    Serial.printf("Time to ON   relay -> ");
    Serial.printf("%d",RelayOn);
    Serial.printf(" micro_sec\n");

    StateStatus = 2;
    dataReady = false;

    deltaOFF = RelayOff;
    flagOFF = true;
  }

  // detect and meas relay switch OFF
  if(dataReady == true && StateStatus == 2){
    digitalWrite(SIGNAL, LOW);

    Serial.printf("Time to OFF relay -> ");
    Serial.printf("%d",RelayOff);
    Serial.printf(" micro_sec\n");

    StateStatus = 1;
    dataReady = false;

    deltaON= RelayOn;
    flagON = true;
  }

  // meas delta time between relay position
  if(flagOFF == true && flagON == true){
    deltaLocal = deltaOFF + deltaON;
    flagOFF = false;
    flagON = false;
    Serial.printf("Час між режимом ON та OFF -> ");
    Serial.printf("%d\n\n",deltaLocal);
  }
  


 
}


