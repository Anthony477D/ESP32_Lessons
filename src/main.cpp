/*
Створюємо програмний PWM але замість керування частоти блимання окремих світлодіодів куруємо кольорами RGB світлодіода
кожен PWM канал керує окремим кольором. Значення задаються з терміналу у відсотках скважності 0-100
*/

#include <Arduino.h>

#define LED_RED 16
#define LED_GREEN 17
#define LED_BLUE 18

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


class SoftPWM{
  private:
    uint8_t _PWMpin; 
    uint16_t _range;  
    uint16_t _segment;
    
    bool _setzero; // маркер який вказує що діпазон модуляції закінчився

    uint32_t _time_zero;  // нульова точка відліку часу
    uint32_t _time_count_external; // на майбутнє, cюди модна привязати фунецію часу та вибирати між мілісек та мікросек

    uint32_t _segment_time; // час за який модулюється один сегмент PWM сигналу
    uint16_t _countSegmentON; // кількість сегментів які маєть бути в положенні HIGH

    uint32_t _timePWM_HIGH; 



  /*    налаштовуємо PWM на піні
  PWMpin  ->  вказуємо який пін буде використаний для генерації PWM
  range   ->  параметер що зажає дрвжину модуляції PWM в мс
  segment ->  параметер, що задає на скільки сегментів буде розбитий відрізок PWM  
  */ 
  void setPin(uint8_t PWMpin, uint16_t range, uint16_t segment){
    _PWMpin = PWMpin;
    _range = range;
    _segment = segment;
    pinMode(_PWMpin, OUTPUT);

    // розраховуємо час одного сегменту модуляції
    _segment_time = _range / _segment;

  }

  void PWM_Main(uint8_t duty){
    //****************************************************************************************************************** 
    // якщо час перілду модуляцї вийшов скидуємо внутріщній таймер в 0 для наступног нового періоду
    if( _setzero == true){
      _time_zero = millis();
      _setzero = false;
    }


    //****************************************************************************************************************** 
    // рохраховуємо скільки цілих сегментів мають положення HIGH
    /*
    кількість_сегментів_HIGH = ( загальна_кількість_сегментів / 100_відсотків) * відсоткове_значення_PWM_в_положенні_HIGH 
    */
    _countSegmentON = (_segment/100) * duty;

    // маючи кількість сегментів та час одного з них, вираховужмо час який потрібно тпимати HIGH
    _timePWM_HIGH = _countSegmentON * _segment_time; 

    // якщо час який потрібно давати HIGH менший за пройдений час від початку періоду, пін в полоденні HIGH
    if( _timePWM_HIGH < (millis() - _range)){
      digitalWrite(_PWMpin, HIGH);
    }else{
      digitalWrite(_PWMpin, LOW);
    }




    //****************************************************************************************************************** 
    /* перевірка чи не вичирпався період модуляції
    (фактичний_час_опорного_нуля + час_періоду_модуляції_PWM)
                          >=
    (фактичний_час_зовнішнього_таймера - час_встановлений_за_нульова_точку_відліку)
    *///якщо вичерпався ставимо марек для скидання нульової точки відліку часу
    if( (_time_zero + _range) >= (millis() - _time_zero)){
      _setzero = true;
    }

  }

};

/*
створюємо переривання де фіксуємо час коли спрацювання а нуль виставляємо власноруч
*/



void setup() {
  
  Serial.begin(115200); // Додаємо монітор порту для діагностики та виводу інформації
  
  pinMode(LED_BLUE, OUTPUT); // config relay control pin
  pinMode(LED_GREEN, OUTPUT); 
  pinMode(LED_RED,OUTPUT);

}




void loop() {

}


