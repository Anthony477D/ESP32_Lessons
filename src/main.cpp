/*

*/

#include <Arduino.h>

#define LED_RED 18
#define LED_GREEN 17
#define LED_BLUE 16

#define MET_1 4 //1
/* базова реалізація кнопки без будь який налаштувань
при натисканні рахуємо кількість хибних спрацювань через переивання та виводимо в термінал число
*/ 

#define MET_2 5 //2
/* Ркалізація брязкоту контакту через переривання та затримку <50мс
*/

#define MET_3 7 //4
/* Debounce через перевірку рівня (state-based)
в основному циклі приймати подію в обробку тільки тоді коли кнопка досі натиснута / ішнорувати події при відпусканні
*/

#define MET_4 1 //5
/* Polling + debounce (без interrupts)
прибрати повністю переривання / опитувати кнопку кодні 5-10мс, реалізувати як машину станів
*/
#define MET_5 2 //7
/* Додати RC-фільтр (конденсатор 100n і резистор 100R, підтягуючий незмінний 10k)
*/

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
rtos pushBattonInform;
rtos buttoncall;
// Software PWM class
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


static uint8_t itr_1 = 0;
static uint8_t itr_2 = 0;
static uint8_t itr_3 = 0;

int32_t _setzero_1;
bool flagMetod_1 = false;
void IRAM_ATTR interaptMetod1(){
  itr_1 = itr_1 + 1;
  flagMetod_1 = true;
  _setzero_1 = millis();
}
bool flagMetod_2 = false;
int32_t _setzero_2;
void IRAM_ATTR interaptMetod2(){
  //itr_2 = itr_2 + 1;
  flagMetod_2 = true;
  _setzero_2 = millis();
}
bool flagMetod_3 = false;
void IRAM_ATTR interaptMetod3(){
  itr_3 = itr_3 + 1;
  flagMetod_3 = true;
}

void setup() {
  Serial.begin(115200); // Додаємо монітор порту для діагностики та виводу інформації
  Serial.printf("TIME_PUT_15_SECOND\n");

  task_1.setTime(1000);
  pushBattonInform.setTime(10);
  buttoncall.setTime(20);
  
  pinMode(MET_1, INPUT_PULLDOWN);
  pinMode(MET_2, INPUT_PULLDOWN);
  pinMode(MET_3, INPUT_PULLDOWN);

  pinMode(MET_4, INPUT_PULLDOWN);
  pinMode(MET_5, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(MET_1), interaptMetod1, RISING);
  attachInterrupt(digitalPinToInterrupt(MET_2), interaptMetod2, RISING);
  attachInterrupt(digitalPinToInterrupt(MET_3), interaptMetod3, RISING);

}
uint16_t itr_4_buttocall = 0;

uint8_t delayFirstMethod_ms = 250; //затримка в мілісекундах між надсиланням інйормації та скидання лічильника переривань
void loop() {  
  // first mothod
  if(flagMetod_1 == true && ((millis() - _setzero_1) >= delayFirstMethod_ms)){
    Serial.printf("кількість зафіксованих натискань на кнопку 1 (yellow) ->%d \n",itr_1);
    flagMetod_1 = false;
    itr_1 = 0;
  }
  

  // second method

  if( (flagMetod_2 == true) && ((millis() - _setzero_2) >= 50) && (digitalRead(MET_2) == true) ){
    //Serial.printf("Button 2 is PUSHEEEEED");
    itr_2 = itr_2 + 1;
    flagMetod_2 = false;

  }else if ((flagMetod_2 == true) && ((millis() - _setzero_2) >= 50) && (digitalRead(MET_2) == false) ){
    flagMetod_2 = false;
    Serial.printf("Кнопка 2 була натиснута - %d разів\n",itr_2);
    itr_2 = 0;
  }

  // metod three

  if( (flagMetod_3 == true) && (digitalRead(MET_3) == true) ){
    if(pushBattonInform.mainrtos() == true){
      Serial.printf("кнопка 3 натиснута 10мс\n");
    }
  }else{
    flagMetod_3 = false;
  }

  // metod 4


  if(buttoncall.mainrtos()){
    if(digitalRead(MET_4) == true){
      itr_4_buttocall++; 
    }else{
      // button false
    }
  }

  // metod_5 hardware solution

  if(digitalRead(MET_5) == true){
    // button enable
    
  }else{
    // button disable
  }





 if(task_1.mainrtos()){
  Serial.printf("ITR_1->%d ITR_2->%d ITR_3->%d \n", itr_1, itr_2, itr_3);
  itr_1 = 0; itr_2 = 0; itr_3 = 0; itr_4_buttocall = 0;
 }

 



}




