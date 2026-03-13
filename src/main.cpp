/*
Створюємо програмний PWM але замість керування частоти блимання окремих світлодіодів керуємо кольорами RGB світлодіода
кожен PWM канал керує окремим кольором. Значення задаються з терміналу у відсотках скважності 0-100
*/

#include <Arduino.h>

#define LED_RED 16
#define LED_GREEN 17
#define LED_BLUE 18

class corectPWM{
  private:
  uint8_t _PWMFreq; // частота ШІМ сигналу в герцах. Максимум 255Гц, це дасть ширину фрагмента в 3.9 мілісекунди
  uint8_t _PWMPin;

  uint32_t _timeSegment; // час одного фрейму PWM сигналу
  uint32_t _UPPWMTime;

  bool _flag = false;
  bool _lastPosition = false;

  uint32_t _zeroPoint;

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
    _zeroPoint = micros(); 
  }

  // для не тревіальних завдань, значкння в відсотках від 1 - 100
  void update_hard_PWM(uint8_t duty){
    _UPPWMTime = (_timeSegment * duty) / 100; // розраховуємо час тривалості логічної 1 в PWM фреймі
    _zeroPoint = micros(); 
  }

  // основний цикл модуляції ШИМ
  void PWMmain(){

    // перевірка лічильника фрейма
    // якщо нульова точка + час фреймк >= за фактичний час то перезаписуємо час нульової точки
    if( (_zeroPoint + _timeSegment) >= micros() ){
      _zeroPoint = micros();
    }

    // перевірка лічильника високого рівня фрейму
    // якщо нуль + час високого рівня < таймера ставимо маркер на підняття рівня
    // якщо менше -> маркер на зменшення рівня
    if( (_zeroPoint + _UPPWMTime) <= micros() ){
      _flag = true;
    }else{
      _flag = false;
    }

    // перевірка модуляції сигналу
    // якщо маркер на підняття рівня стоїть та попередня позиція була низький рівень, піднімаємо рівень
    // якщо маркер на підняття рівня опущений та попередня позиція була піднята, опускаємо рівень
    // якщо ні одна умова не виконується -> нічого не робимо
    if(_flag == true && _lastPosition == false){
      digitalWrite(_PWMPin,HIGH);
      _lastPosition = true;

    }else if(_flag == false && _lastPosition == true){
      digitalWrite(_PWMPin,LOW);
      _lastPosition = false;

    }else{
      // _flag == _lastPosition -> nothing doing
    }

  }

};



void setup() {
  
  Serial.begin(115200); // Додаємо монітор порту для діагностики та виводу інформації
  
  pinMode(LED_BLUE, OUTPUT); // config relay control pin
  pinMode(LED_GREEN, OUTPUT); 
  pinMode(LED_RED,OUTPUT);

  

}



void loop() {

  
  
  //digitalWrite(LED_BLUE, HIGH);
  //delay(250);
  //digitalWrite(LED_BLUE, LOW);
  //delay(250);
  //Serial.printf("I am loop");
}


