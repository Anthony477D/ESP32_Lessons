#include <Arduino.h>

// Clear project from new lessons created on bace platform in arduino
/*
Домашня робота по освоєнню C++ та робота з світлодіодом на основі методів, обєктів та класів

*/

// створюємо дкфайни для пінів які ми використовуємо
#define LED_PIN 14 
#define BUTTON_PIN 3

#define USER_DELAY 500


// створюємо клас перерахування статусів в яких може знаходитись наш світлодіод
/*
enum можна використовувати і без class, але такий метод дозволяж створити декілька перерахувань з одними змінними
якщо без class то область видимості змінних в перерахуванні глобальна
*/
enum class LedState {
  On,
  Off
};

/*
Створюємо клас Config в якому зберігаються конфігурацційні змінні 
*/
class Config{
  public:
    static constexpr uint8_t LED = LED_PIN;
    static constexpr uint8_t BUTTON = BUTTON_PIN;
    static constexpr uint16_t BLINK_DELAY = USER_DELAY;
};


/*
створюємо клас Led, по образу і подобі. якого будуть створювати елементи класу
*/
class Led{
  // вказуємо приватні змінні, область видимості яких буде лоступний лише для обєкту
  private:
    uint8_t _pin;
  
  // робимо параметер класу Led публічний для того щоб вносити данні ззовні
  public:
    Led(uint8_t pin_number_OutClass){
      _pin = pin_number_OutClass; // присвоємо зовніній параметер внутріщній змінні
    }
     
  // робимо функцію ініціалізації для піна мікроконтроллера
  void init() { 
    pinMode(_pin,OUTPUT);
  }

  // робимо функціб для налаштування обєкту класу
  // в параметрах функції вказуємо обєет enum з якого буде взята змінна, та називоємо її state для подільної роботи з нею
  // в порівняннях, вказуємо фіксоване значення LedState, та те що ми отримуємо коли викликаємо функцію обєкта
  void set(LedState state){ 
    if(state == LedState::On){
      digitalWrite(_pin, HIGH);
    } 
    if(state == LedState::Off){
      digitalWrite(_pin, LOW);
    }
  }
};

// створюємо обєкт MyLed класу Led
Led MyLed(Config::LED);


void setup() {
  MyLed.init();

}

void loop() {

  while(digitalRead(Config::BUTTON) == HIGH ){
    MyLed.set(LedState::On);
    delay(Config::BLINK_DELAY);

    MyLed.set(LedState::Off);
    delay(Config::BLINK_DELAY);
  } 

  MyLed.set(LedState::Off);


  /* тест роботи світлодіода напряму 
  digitalWrite(LED_PIN , LOW);
  delay(500);

  digitalWrite(LED_PIN , HIGH);
  delay(500);

  */
  
}

