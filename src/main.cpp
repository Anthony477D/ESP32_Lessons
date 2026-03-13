/*
Створюємо програмний PWM але замість керування частоти блимання окремих світлодіодів керуємо кольорами RGB світлодіода
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
    uint32_t _time_count_external; // на майбутнє, cюди можна привязати функцію часу та вибирати між мілісек та мікросек

    uint32_t _segment_time; // час за який модулюється один сегмент PWM сигналу
    uint16_t _countSegmentON; // кількість сегментів які маєть бути в положенні HIGH

    uint32_t _timePWM_HIGH; 

    //volatile uint32_t _time_core = millis();

  public: // без цього всі функції класу стають приватними(

  /*    налаштовуємо PWM на піні
  PWMpin  ->  вказуємо який пін буде використаний для генерації PWM
  range   ->  параметр що задає довжину модуляції PWM в мс
  segment ->  параметр, що задає на скільки сегментів буде розбитий відрізок PWM  
  */ 
  void setPin(uint8_t PWMpin, uint16_t range, uint16_t segment){
    _PWMpin = PWMpin;
    _range = range;
    _segment = segment;
    pinMode(_PWMpin, OUTPUT);

    // розраховуємо час одного сегменту модуляції
    _segment_time = _range / _segment;

  }

  void PWM_Update(){

     if( _timePWM_HIGH < (micros() - _time_zero + _segment_time)){
      digitalWrite(_PWMpin, LOW);
    }else{
      digitalWrite(_PWMpin, HIGH);
    }
    //millis; 

    if( _setzero == true){
      _time_zero = micros();
      _setzero = false;
    }

    if( _range <= (micros() - _time_zero)){
      _setzero = true;
    }


   

    
  }

  void PWM_Main(uint8_t duty){
    //****************************************************************************************************************** 
    // якщо час періоду модуляції вийшов скидуємо внутріщній таймер в 0 для наступного нового періоду
    
    if( _setzero == true){
      _time_zero = micros();
      _setzero = false;
    }


    //****************************************************************************************************************** 
    // розраховуємо скільки цілих сегментів мають положення HIGH
    /*
    кількість_сегментів_HIGH = ( загальна_кількість_сегментів / 100_відсотків) * відсоткове_значення_PWM_в_положенні_HIGH 
    */
    _countSegmentON = (_segment*duty) / 100;

    // маючи кількість сегментів та час одного з них, вираховужмо час який потрібно тпимати HIGH
    _timePWM_HIGH = _countSegmentON * _segment_time; 

    // якщо час який потрібно давати HIGH менший за пройдений час від початку періоду, пін в положенні HIGH
    if( _timePWM_HIGH < (micros() - _time_zero)){
      digitalWrite(_PWMpin, HIGH);
    }else{
      digitalWrite(_PWMpin, LOW);
    }




    //****************************************************************************************************************** 
    /* перевірка чи не вичерпався період модуляції
    (фактичний_час_опорного_нуля + час_періоду_модуляції_PWM)
                          >=
    (фактичний_час_зовнішнього_таймера - час_встановлений_за_нульову_точку_відліку)
    *///якщо вичерпався ставимо маркер для скидання нульової точки відліку часу
    if( _range <= (micros() - _time_zero)){
      _setzero = true;
    }

  }

};

/*
створюємо переривання де фіксуємо час коли спрацювання а нуль виставляємо власноруч
*/

SoftPWM RGB_LedGreen;


void setup() {
  
  Serial.begin(115200); // Додаємо монітор порту для діагностики та виводу інформації
  
  pinMode(LED_BLUE, OUTPUT); // config relay control pin
  pinMode(LED_GREEN, OUTPUT); 
  pinMode(LED_RED,OUTPUT);

  RGB_LedGreen.setPin(LED_GREEN, 1000, 50);

}


bool flag = true;

void loop() {

  if(flag == true){

    flag = false;
    RGB_LedGreen.PWM_Main(50);
  }

  RGB_LedGreen.PWM_Update();
  
  //digitalWrite(LED_BLUE, HIGH);
  //delay(250);
  //digitalWrite(LED_BLUE, LOW);
  //delay(250);
  //Serial.printf("I am loop");
}


class corectPWM{
  private:
  uint8_t _PWMFreq; // частота ШІМ сигналу в герцах. Максимум 255Гц, це дасть ширину фрагмента в 3.9 мілісекунди
  uint8_t _PWMPin;

  uint32_t _timeSegment; // час одного фрейму PWM сигналу
  uint32_t _UPPWMTime;

  public:
    
  void init_PWM(uint8_t PinGPIO,uint8_t FreqPWM){
    // запис вхідних данниї в обєкт класу для подальщої роботи
    _PWMPin = PinGPIO;
    _PWMFreq = FreqPWM; // частота до 255Гц

    // ініціалізація піна, та всталовлення його в режим LOW
    pinMode(_PWMPin, OUTPUT);
    digitalWrite(_PWMPin,LOW);

    // розрахунок часу тривалорсті одного фрейму PWM сигналу
    uint32_t onesecond = 1000000; // значення однієї секунди в тактах для нашої опорної частоти
    _timeSegment = onesecond / _PWMFreq;
  }

  // для збільшення чутливості, duty задається в форматі цілого числа, але з сотою частиною відсотка
  // 1% -> 100 / 5% -> 500 / 30% -> 3000 / 100% -> 10000
  // це дозволяє на порядок збільшити чутливість PWM, а int формат - задає простоту розрахунків відносно float
  void update_soft_PWM(uint16_t duty){
    _UPPWMTime = (_timeSegment * duty) / 10000; // розраховуємо час тривалості логічної 1 в PWM фреймі
  }

  // для не тревіальних завдань, значкння в відсотках від 1 - 100
  void update_hard_PWM(uint8_t duty){
    _UPPWMTime = (_timeSegment * duty) / 100; // розраховуємо час тривалості логічної 1 в PWM фреймі
  }

  void PWMmain(){
    
  }

  

  

};