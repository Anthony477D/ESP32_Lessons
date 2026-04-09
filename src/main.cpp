#include <Arduino.h>

// Home_project
/*
  Зробити світлофор
   -> Ряд діодів для автомобіля(Green Yellow Red)  
   -> Ряд діодів для пішоходів(Green Red)
   -> Кнопку для вмикання зеленого сигналу для піщоходів
   -> Резстивгий датчик освітленості

  Порядок роботи для автомобілів
  - Зелений
  - Блимаючий зелений
  - Жовтий
  - Червоний
  - Червоний+Жовтий
  - Зений

  Порядок роботи для піщохожів
  - Зелегий, коли для автомобілів червоний
  - Червоний у всих інших комбінаціях
  - Блимаючий зелений перед ввімкненням для автомобілів червоний+жовтий

  Кнопка - обриває цикл, та вмикає зелений для пішоходів

  Датчик освітленості - коли день(нормальна робота), колт ніч(блимаючий жовтий)

  Коли ніч і натискається кнопка вмикаються кольори і для піщоходів зелений

*/
// define pin number to periferial component
#define RED_CAR 4
#define YELLOW_CAR 5
#define GREEN_CAR 6

#define GREEN_PEOPLE 8
#define RED_PEPLE 3

#define BUTTON 9
#define LIGHT_SENSETIVE 10


// timer define
#define RED_COLORTIME_SECOND 5
#define YELLOW_COLORTIME_SECOND 2
#define GREEN_COLORTIME_SECOND 5

#define RESOLUTIO_TIMER_mSEC 100
#define TIM2SEC 10 // = 1sec/RSOLUTION_TIMER_mSEC




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


corectPWM yellow_blink_color;
corectPWM green_blink_color;
corectPWM green_blink_people;

class rtos {
  private:
    uint32_t _zeroPoint;
    uint16_t _holdTime;

  public:
    bool _flag;

  // set rtos time delay in millis
  void setTime (uint16_t holdTime){
    _zeroPoint = millis();
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

  //set zero timer
  void setZero(){
    _zeroPoint = millis();
  }
};


// create maine base freq to iteration color car leds
rtos main_timer;
rtos check_button_timer;
rtos check_light_timer;

// function to setup color mode
uint8_t set_state_color(uint16_t);
// function to set outputs pin on led
void release_state_color(uint8_t);
// function to setup peple leds
void people_state_color(void);




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(RED_CAR, OUTPUT);
  pinMode(YELLOW_CAR, OUTPUT);
  pinMode(GREEN_CAR, OUTPUT);

  pinMode(RED_PEPLE, OUTPUT);
  pinMode(GREEN_PEOPLE, OUTPUT);

  pinMode(BUTTON, INPUT_PULLDOWN);

  yellow_blink_color.init_PWM(YELLOW_CAR,2);
  yellow_blink_color.update_hard_PWM(50);

  green_blink_color.init_PWM(GREEN_CAR,2);
  green_blink_color.update_hard_PWM(50);

  green_blink_people.init_PWM(GREEN_PEOPLE,2);
  green_blink_people.update_hard_PWM(50);

  main_timer.setTime(100); // задаємосновний час роботи
  check_button_timer.setTime(10);
  check_light_timer.setTime(500);
}


uint16_t itr = 1;
uint8_t sate_mashine;
bool button_flag;
void loop() {
  sate_mashine = set_state_color(itr);
  
  release_state_color(sate_mashine);
  people_state_color();

  if(main_timer.mainrtos()){
    itr ++;
            //Serial.printf("itr %d \n", itr);
            //Serial.printf("test_counter %d \n", sate_mashine);
  }

  if((check_button_timer.mainrtos()) && (button_flag == false) ){
            
    button_flag = digitalRead(BUTTON);

    if( (button_flag == true) && (digitalRead(GREEN_CAR) == HIGH)){
      itr = (RED_COLORTIME_SECOND+YELLOW_COLORTIME_SECOND+GREEN_COLORTIME_SECOND-2)*TIM2SEC;
    }
    //Serial.printf("button status -> %d \n",button_flag);
  }
    
}

uint8_t set_state_color(uint16_t count) {
  uint8_t state_color = 0;

  uint16_t time_to_red = RED_COLORTIME_SECOND * TIM2SEC;
  uint16_t time_to_redyellowgreen = (RED_COLORTIME_SECOND+YELLOW_COLORTIME_SECOND)*TIM2SEC;
  uint16_t time_to_green = (RED_COLORTIME_SECOND+YELLOW_COLORTIME_SECOND+GREEN_COLORTIME_SECOND)*TIM2SEC;
  uint16_t time_to_blinkgreen;
  uint16_t time_to_greenyellowred =(RED_COLORTIME_SECOND+(YELLOW_COLORTIME_SECOND*2)+GREEN_COLORTIME_SECOND)*TIM2SEC;

  // red
  if(count <= time_to_red){
    state_color = 1;
    return state_color;
  }

  // red and yellow
  if((time_to_red <= count) && (count <= time_to_redyellowgreen)){
    state_color = 2;
    return state_color; 
  }

  // green
  if((time_to_redyellowgreen <= count) && (count <= (time_to_green-(2*TIM2SEC)))){
    state_color = 3;
    return state_color;
  }
  
  // blinkgreen
  if( ((time_to_green-(2*TIM2SEC)) <= count ) && (count <= time_to_green)){
    state_color = 4;
    return state_color;
  }
  

  // yellow to red (green swich to red)
  if((time_to_green <= count) && (count <= time_to_greenyellowred)){
    state_color = 5;
    return state_color;
  }
  
  else{
    itr = 0;
    state_color = 1;
    return state_color;
  }


};


void release_state_color(uint8_t state){

  switch (state)
  {
  case 1:
    //code
    digitalWrite(RED_CAR,HIGH);
    digitalWrite(YELLOW_CAR, LOW);
    digitalWrite(GREEN_CAR,LOW);
    break;
  
  case 2:
    //code
    digitalWrite(RED_CAR,HIGH);
    digitalWrite(YELLOW_CAR,HIGH);
    digitalWrite(GREEN_CAR,LOW);
    button_flag = false;
    break;

  case 3:
    //code
    digitalWrite(RED_CAR,LOW);
    digitalWrite(YELLOW_CAR,LOW);
    digitalWrite(GREEN_CAR,HIGH);
    break;

  case 4:
    //code
    green_blink_color.PWMmain();
    break;

  case 5:
    // code
    digitalWrite(RED_CAR,LOW);
    digitalWrite(YELLOW_CAR,HIGH);
    digitalWrite(GREEN_CAR,LOW);
    break;  
  }

};


void people_state_color(void){
  if((digitalRead(RED_CAR) == HIGH) && (digitalRead(YELLOW_CAR) == LOW) ){
    digitalWrite(GREEN_PEOPLE, HIGH);
    digitalWrite(RED_PEPLE, LOW);
  }

  if(digitalRead(GREEN_CAR) == HIGH){
    digitalWrite(RED_PEPLE, HIGH);
    digitalWrite(GREEN_PEOPLE, LOW);
  }

  if((digitalRead(YELLOW_CAR) == HIGH) && (digitalRead(RED_CAR) == HIGH)){
    green_blink_people.PWMmain();
  }

}







