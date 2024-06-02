#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define countSensorTrigPin 2  // 쓰레기 개수 센서의 Trig 핀
#define countSensorEchoPin 3  // 쓰레기 개수 센서의 Echo 핀
#define fullSensorTrigPin 4  // 쓰레기통 상태 센서의 Trig 핀
#define fullSensorEchoPin 5  // 쓰레기통 상태 센서의 Echo 핀
#define endbuttonPin 6  // 버튼 핀 종료 버튼
#define startbuttonPin 7  // 버튼 핀 시작 버튼
#define piezobuzzer 8 //피에조 부저
#define motorPin 9  // 모터 핀

const int threshold = 45;  // 쓰레기 개수 센서의 임계값
const int fullThreshold = 10;  // 쓰레기통 상태 센서의 임계값

int trashCount = 0;  // 쓰레기 개수
bool isFull = false;  // 쓰레기통이 꽉 찼는지 여부
Servo motor;  // 서보 모터 객체 생성

LiquidCrystal_I2C lcd(0x27,16,2); //lcd 객체 생성

void setup() {
  pinMode(countSensorTrigPin, OUTPUT);
  pinMode(countSensorEchoPin, INPUT);
  pinMode(fullSensorTrigPin, OUTPUT);
  pinMode(fullSensorEchoPin, INPUT);
  pinMode(endbuttonPin, INPUT_PULLUP);
  pinMode(startbuttonPin, INPUT_PULLUP);
  
  motor.attach(motorPin);  // 모터 핀 번호 설정
  motor.write(0);  // 모터 초기 위치 설정

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("set lcd"); //lcd 초기 설정
  
  Serial.begin(9600);
}

void loop() {
  //lcd 센서 설정
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Save the earth");

  
  // 쓰레기 개수 센서로 거리 측정
  long countDuration, countDistance;
  digitalWrite(countSensorTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(countSensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(countSensorTrigPin, LOW);
  countDuration = pulseIn(countSensorEchoPin, HIGH);
  countDistance = countDuration * 0.034 / 2;
  
  // 거리 값 출력
  Serial.print("Count Sensor Distance: ");
  Serial.println(countDistance);

   
  

  // 시작 버튼이 눌렸을 때 동작
  if (digitalRead(startbuttonPin) == HIGH) {
    Serial.println(" START Button Pressed");
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("START");
    
    motor.write(0);  // 모터를 원래 위치로 되돌림
    trashCount = 0;  // 쓰레기 개수 초기화
    
    delay(1000);  // 버튼 입력 후 잠시 딜레이를 줌으로써 반복 입력 방지
    // 쓰레기통 상태 센서로 거리 측정
    long fullDuration, fullDistance;
    digitalWrite(fullSensorTrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(fullSensorTrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(fullSensorTrigPin, LOW);
    fullDuration = pulseIn(fullSensorEchoPin, HIGH);
    fullDistance = fullDuration * 0.034 / 2;
  
    // 거리 값 출력
    Serial.print("Full Sensor Distance: ");
    Serial.println(fullDistance);

    // 쓰레기통이 꽉 찼는지 확인
    if (fullDistance < fullThreshold) {
      isFull = true;
    } else {
      isFull = false;
    }
    

           

    // 쓰레기 개수 카운트
    while (digitalRead(endbuttonPin) == LOW) {
      // 쓰레기 개수 센서로 거리 측정
      digitalWrite(countSensorTrigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(countSensorTrigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(countSensorTrigPin, LOW);
      countDuration = pulseIn(countSensorEchoPin, HIGH);
      countDistance = countDuration * 0.034 / 2;

      if (countDistance < threshold) {
        trashCount++;
        Serial.print("Trash Count: ");
        Serial.println(trashCount);
        lcd.setCursor(0,0);
        lcd.clear();
        lcd.print("Trash Count: ");
        lcd.print(trashCount);
        playSound();
        delay(1000);  // 중복 카운트 방지를 위한 딜레이
      }

          // 쓰레기통이 꽉 찼는지 확인
      digitalWrite(fullSensorTrigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(fullSensorTrigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(fullSensorTrigPin, LOW);
      fullDuration = pulseIn(fullSensorEchoPin, HIGH);
      fullDistance = fullDuration * 0.034 / 2;
      
    if (isFull) {
      Serial.println("Trash bin is full!");
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("Bin is full!");
      playWarningSound();
    } 
    else{
      isFull = false;
    }
   }
  

  }

  // 종료 버튼이 눌렸을 때 동작
  if (digitalRead(endbuttonPin) == HIGH) {
    Serial.println(" END Button Pressed");
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("END");
    
    // 쓰레기 개수만큼 모터 동작
    for (int i = 0; i < trashCount; i++) {
      motor.write(180);  // 모터를 90도로 회전
      delay(5000);  // 모터 동작 후 잠시 대기
      motor.write(0);  // 모터를 원래 위치로 되돌림
      delay(500);  // 모터 동작 간 딜레이
    }
    
    trashCount = 0;  // 쓰레기 개수 초기화
    Serial.println("Motor Rotated");
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("Motor Rotated");
    delay(1000);  // 버튼 입력 후 잠시 딜레이를 줌으로써 반복 입력 방지
  }
  
  delay(100);
}


// 효과음을 재생 함수
void playSound() {
  tone(piezobuzzer, 1000, 100); // Play a sound at 1kHz for 100ms
  delay(100);
  noTone(piezobuzzer); // Stop the sound
}

// 경고음을 재생하는 함수
void playWarningSound() {
  tone(piezobuzzer, 2000, 500); // 2kHz의 소리를 500ms 동안 재생
  delay(500);
  noTone(piezobuzzer); // 소리 재생 중지
  
}
