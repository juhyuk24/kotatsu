#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <sound_effect.h>
#include <thermistor.h>
#include <MsTimer2.h>

LiquidCrystal_I2C lcd(0x27,16,2);

int PotenTiometer = A0; //가변저항
int PotenTiometer_val;  //가변저항 값
int Thermistor = A1;    //서미스터
int Relay_Heating = 5;  //릴레이
int Heating_LED = 2;    //LED
int System_Switch = 4;  //스위치
int Speaker = 12;       //스피커
float Temp;             //온도
int this_state = 0;     //0: 켜진 상태, 1: 꺼진 상태
int cnt = 0;            //시간 카운트

uint8_t clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};  //시계 특수문자
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};   //하트 특수문자

void setup() {
  pinMode(Heating_LED, OUTPUT);
  pinMode(System_Switch, INPUT_PULLUP);
  pinMode(Relay_Heating , OUTPUT);
  pinMode(Speaker, OUTPUT);         //핀모드 설정

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  delay(1000);
  
  lcd.print("Hello ^0^");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("KOTACHU IoT TEAM");
  seSqueak(Speaker);

  lcd.createChar(2, clock);
  lcd.createChar(3, heart); //lcd.print할때 2, 3 넣으면 특수문자 출력
  
  MsTimer2::set(14400000, timeout); //4시간마다 timeout()가 실행되어 전구와 LED가 꺼짐
  MsTimer2::start();
}


void loop() {
  delay(1000);

  if(ThermistorToC(Temp) > 50) { //현재 온도가 50도 넘을 때
    lcd.clear();
    lcd.print("Warning");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("High Temperature");
    seAlarm(Speaker);
    digitalWrite(Relay_Heating, LOW);
  }

  if(digitalRead(System_Switch) == LOW) {   //스위치가 꺼진 상태면(아래로 향할 때)
    PotenTiometer_val = analogRead(PotenTiometer);
    PotenTiometer_val = map(PotenTiometer_val, 0, 1023, 30, 41);  //30~40
    Temp = analogRead(Thermistor);

    lcd.clear();
    lcd.print("Temp : ");
    lcd.print(ThermistorToC(Temp));
    lcd.print("(");
    lcd.print(PotenTiometer_val);
    lcd.print(")");
    lcd.setCursor(0, 1);  //현재 온도 출력 후 lcd커서 위치변경
    
    if(ThermistorToC(Temp) < PotenTiometer_val && this_state == 0) { 
      //현재 온도가 가변저항 값보다 작을 때
      cnt = 0;
      digitalWrite(Relay_Heating, HIGH);
      digitalWrite(Heating_LED, HIGH);  //전구와 LED를 켬
      lcd.print("Heating ON ");
      lcd.write(3);
    }
    else if(ThermistorToC(Temp) < PotenTiometer_val && cnt > 30) { //전구 꺼지고 30초 지난 경우
        this_state = 0;
    }
    else {  //전구가 꺼져야 하는 경우
      this_state = 1;
      cnt++;
      digitalWrite(Relay_Heating, LOW);
      digitalWrite(Heating_LED, LOW);
      lcd.print("Heating OFF ");
      lcd.write(2);
    }
  }

  else { //스위치가 켜진 상태면(위로 향할 때)
    lcd.clear();
    digitalWrite(Heating_LED, LOW);
    digitalWrite(Relay_Heating, LOW);
    lcd.print("bye ^0^");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Sleep Mode");
  }
}

void timeout() {  //전구와 LED를 끄고 lcd 초기화
  digitalWrite(Relay_Heating, LOW);
  digitalWrite(Heating_LED, LOW);
  lcd.clear();
}