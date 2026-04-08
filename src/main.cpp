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



rtos main_timer;

uint8_t set_state_color(uint16_t);

void release_state_color(uint8_t);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(RED_CAR, OUTPUT);
  pinMode(YELLOW_CAR, OUTPUT);
  pinMode(GREEN_CAR, OUTPUT);

  pinMode(RED_PEPLE, OUTPUT);
  pinMode(GREEN_PEOPLE, OUTPUT);

  

  main_timer.setTime(100); // задаємосновний час роботи
}


uint16_t itr = 1;

void loop() {
  uint32_t test_counter = set_state_color(itr);
  release_state_color(test_counter);

  if(main_timer.mainrtos()){
    itr ++;
            Serial.printf("itr %d \n", itr);
            Serial.printf("test_counter %d \n", test_counter);
  }




  if(digitalRead(RED_CAR) == HIGH){
    digitalWrite(GREEN_PEOPLE, HIGH);
    digitalWrite(RED_PEPLE, LOW);
  }

  if(digitalRead(GREEN_CAR) == HIGH){
    digitalWrite(RED_PEPLE, HIGH);
    digitalWrite(GREEN_PEOPLE, LOW);
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

  // yellow
  if((time_to_red <= count) && (count <= time_to_redyellowgreen)){
    state_color = 2;
    return state_color; 
  }

  // green
  if((time_to_redyellowgreen <= count) && (count <= time_to_green)){
    state_color = 3;
    return state_color;
  }
  
  // blinkgreen
  

  // yellow2red
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
    digitalWrite(RED_CAR,LOW);
    digitalWrite(YELLOW_CAR,HIGH);
    digitalWrite(GREEN_CAR,LOW);
    break;

  case 3:
    //code
    digitalWrite(RED_CAR,LOW);
    digitalWrite(YELLOW_CAR,LOW);
    digitalWrite(GREEN_CAR,HIGH);
    break;

  case 5:
    // code
    digitalWrite(RED_CAR,LOW);
    digitalWrite(YELLOW_CAR,HIGH);
    digitalWrite(GREEN_CAR,LOW);
    break;  
  }

};