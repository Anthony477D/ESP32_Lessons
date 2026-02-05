//*******************************************************************************************************
//*******************************************************************************************************

// Підєднання кнопок та зчитування за допомогою цифрового аналізатора дребезгу їх контакту при натисканні

//*******************************************************************************************************



#include <Arduino.h>

//_____________________________________________________________________
// створюємо дефайни для кнопок та "ініціюємо їх"
#define BT_LEFT 10
#define BT_RIGHT 11


//_____________________________________________________________________
// створюємо змінні які будуть рахувати натискання
uint16_t counter_left = 0;
uint16_t counter_right = 0;

//_____________________________________________________________________
// створюємо маркери для переривань
bool FL_BT_left = false;
bool FL_BT_right = false;


//_____________________________________________________________________
// створюємо функції переривання та код в них
void IRAM_ATTR reaction_left(){
  counter_left++;
  FL_BT_left = true;
}

void IRAM_ATTR reaction_right(){
  counter_right++;
  FL_BT_right = true;
}


// put function declarations here:
//int myFunction(int, int);



//*******************************************************************************************************
void setup() {
  // put your setup code here, to run once:
  //int result = myFunction(2, 3);

  // налаштовуємо піни на вихід
  pinMode(BT_LEFT, INPUT);
  pinMode(BT_RIGHT, INPUT);

  // налаштовуємо переривання (вказуємо-> пін / назву фунеції / тип спрацювання)
  attachInterrupt(digitalPinToInterrupt(BT_LEFT), reaction_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(BT_RIGHT), reaction_right, FALLING);

  // налаштовуємо серівл порт
  Serial.begin(115200);

}

//*******************************************************************************************************
void loop() {
  // put your main code here, to run repeatedly:

  //printf("Hello world");
  //delay(500);

  if(FL_BT_left == true){
    Serial.println("\nЛіва кнопка натиснута ->" + String(counter_left));
    FL_BT_left = false;
    
  }

  if(FL_BT_right == true){
    Serial.println("\nПрава кнопка натиснута ->" + String(counter_right));
    FL_BT_right = false;

  }

}



//*******************************************************************************************************
//*******************************************************************************************************
/* put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}
*/