/*
Створюємо програмний PWM але замість керування частоти блимання окремих світлодіодів керуємо кольорами RGB світлодіода
кожен PWM канал керує окремим кольором. Значення задаються з терміналу у відсотках скважності 0-100
*/

#include <Arduino.h>

#define LED_RED 18
#define LED_GREEN 17
#define LED_BLUE 16

// rtos -> testin on future code
class rtos {
  private:
    uint32_t _zeroPoint;
    uint16_t _holdTime;

  public:
    bool _flag;

  // set rtos time delay in millis
  void setTime (uint16_t holdTime){
    _zeroPoint = micros();
    _holdTime = holdTime;
  };

  // chech time out in millis
  bool mainrtos (){
    
    if( (millis() - _zeroPoint) >= _holdTime){
      _zeroPoint = millis();

      _flag = true;
      return _flag;

    }else{
      _flag = false;
      return _flag;
    }

  };
};
rtos task_1;
rtos task_2;
rtos task_3;
rtos task_4;
rtos task_5;

class corectPWM{
  private:
  uint8_t _PWMFreq; // частота ШІМ сигналу в герцах. Максимум 255Гц, це дасть ширину фрагмента в 3.9 мілісекунди
  uint8_t _PWMPin;

  uint32_t _timeSegment; // час одного фрейму PWM сигналу
  uint32_t _UPPWMTime;

  bool _flag;
  bool _lastPosition;

  uint32_t _zeroPoint;
  uint32_t _curentTime;

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

    _flag = false;
    _lastPosition = false;
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
    _curentTime = micros();
    // перевірка лічильника фрейма
    // якщо нульова точка + час фреймк >= за фактичний час то перезаписуємо час нульової точки
    if( (_curentTime - _zeroPoint) >= _timeSegment ){
      _zeroPoint = _curentTime;
    }

    // перевірка лічильника високого рівня фрейму
    // якщо нуль + час високого рівня < таймера ставимо маркер на підняття рівня
    // якщо менше -> маркер на зменшення рівня
    if( (_curentTime - _zeroPoint) <= _UPPWMTime ){
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

corectPWM ledRed;
corectPWM ledGreen;
corectPWM ledBlue;


void setup() {
  
  Serial.begin(115200); // Додаємо монітор порту для діагностики та виводу інформації
  
  ledRed.init_PWM(LED_RED,100);
  ledBlue.init_PWM(LED_BLUE,100);
  ledGreen.init_PWM(LED_GREEN,100);



  task_1.setTime(100);
  task_2.setTime(100);
  task_3.setTime(100);

  task_4.setTime(15000);
  task_5.setTime(1000);

  //ledRed.update_hard_PWM(10);
  //ledBlue.update_hard_PWM(10);
  //ledGreen.update_hard_PWM(10);
}


int flag = false;

int i = 0;
int j = 0;
int k = 0;

void loop() {

  if( (task_1.mainrtos()) == true){
    i = i + 3;
    if(i <= 100){
      ledRed.update_hard_PWM(i);
    } else{
      i = 0;
    }
    
  }

  if( (task_2.mainrtos()) == true){
    j = j + 5;
    if(j <= 100){
      ledGreen.update_hard_PWM(i);
    } else{
      j = 0;
    }
    
  }

  if( (task_3.mainrtos()) == true){
    k = k + 7;
    if(k <= 100){
      ledBlue.update_hard_PWM(i);
    } else{
      k = 0;
    }
    
  }
  
  ledRed.PWMmain();
  ledBlue.PWMmain();
  ledGreen.PWMmain();
  

  if(task_4.mainrtos()){
    Serial.printf("TIME_PUT_15_SECOND\n");
  }
  if(task_5.mainrtos()){
    Serial.printf("i-> %d. j-> %d. k-> %d. \n", i,j,k);
  }
  
 
}


