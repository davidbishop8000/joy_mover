#include <Arduino.h>
//Shift+Alt+F format
#define pin_X A0
#define pin_Y A1
#define center_btn 11 //center
#define ultrasonic_btn 12 //r2
#define stop_btn 10 //r
#define light_f 4
#define light_b 5
#define ledpin 13
#define AxeXZero 508
#define AxeYZero 558
#define k 20 //4
#define diff 70 //20
#define max_val 390 //512
#define min_val -390 //512
#define max_speed 2900
#define min_speed -2900

//       Y
//      900
//       |
// <<900---100>> X
//       |
//      100


uint8_t DataSend[20] = {
    0,
};
int32_t speed_left, speed_right;
int32_t pos_Y, pos_X;
uint8_t zero_count = 0;
uint8_t forw_led_sv = 0;
uint8_t back_led_sv = 0;
void getValue()
{
  pos_Y = (analogRead(pin_Y) - AxeYZero);
  if (pos_Y < k && pos_Y > (-1 * k))
    pos_Y = 0;
  pos_X = analogRead(pin_X);
  //Serial.println(String(analogRead(pin_Y)) + " | " + String(analogRead(pin_X)));
  /*if ((pos_X < AxeXZero - k) || (pos_X > AxeXZero + k))
  {
    speed_left = pos_Y + (pos_X - AxeXZero);
    speed_right = pos_Y - (pos_X - AxeXZero);
  }
  */
  if ((pos_X < AxeXZero - k) || (pos_X > AxeXZero + k))
  {
    if (pos_Y >= 0)
    {
      speed_left = pos_Y + (pos_X - AxeXZero);
      speed_right = pos_Y - (pos_X - AxeXZero);
    }
    else
    {
      speed_left = pos_Y - (pos_X - AxeXZero);
      speed_right = pos_Y + (pos_X - AxeXZero);
    }
  }

  /*if ((pos_X > (AxeXZero + k)) || (pos_X > (AxeXZero + k)))
  {
    if (pos_Y > (AxeYZero + k))
    {
      speed_left = pos_Y + (pos_X - AxeXZero);
      speed_right = pos_Y - (pos_X - AxeXZero);
    }
    else
    {
      speed_left = pos_Y - (pos_X - AxeXZero);
      speed_right = pos_Y + (pos_X - AxeXZero);
    }
  }
  else if (pos_X < (AxeXZero - k))
  {
    if (pos_Y < (AxeYZero + k))
    {
      speed_left = pos_Y - (pos_X - AxeXZero);
      speed_right = pos_Y + (pos_X - AxeXZero);
    }
    else
    {
      speed_left = pos_Y + (pos_X - AxeXZero);
      speed_right = pos_Y - (pos_X - AxeXZero);
    }
  }*/
  else
  {
    speed_left = pos_Y;
    speed_right = pos_Y;
  }

  if (speed_left > max_val)
    speed_left = max_val;
  else if (speed_left < min_val)
    speed_left = min_val;
  else if ((speed_left > 0 && speed_left < diff) || (speed_left < 0 && speed_left > -diff))
    speed_left = 0;
  if (speed_right > max_val)
    speed_right = max_val;
  else if (speed_right < min_val)
    speed_right = min_val;
  else if ((speed_right > 0 && speed_right < diff) || (speed_right < 0 && speed_right > -diff))
    speed_right = 0;
  speed_left = map(speed_left, min_val, max_val, min_speed, max_speed);
  speed_right = map(speed_right, min_val, max_val, min_speed, max_speed);
  //Serial.println(String(speed_left) + " | " + String(speed_right));
  delay(5);
}
void send_data()
{
  DataSend[0] = 69; //E
  DataSend[1] = 53; //5
  DataSend[2] = 68; //D
  DataSend[3] = 84; //T
  uint8_t rever[4];
  int32_t *p_val = (int32_t *)&rever[0];
  *p_val = *(&speed_left);
  uint8_t i;
  for (i = 0; i < 4; i++)
  {
    DataSend[i + 4] = rever[3 - i];
  }
  *p_val = *(&speed_right);
  for (i = 0; i < 4; i++)
  {
    DataSend[i + 8] = rever[3 - i];
  }
  if (speed_left != 0 || speed_right != 0)
  {
    zero_count = 0;
    Serial.write(DataSend, 12);
    //delay(100);
  }
  else if (zero_count < 5)
  {
    zero_count++;
    Serial.write(DataSend, 12);
  }
  delay(30);
}
void get_button()
{
  static uint8_t stop_btn_press = 0;
  static uint8_t ultrasonic_btn_press = 0;
  static uint8_t center_btn_press = 0;
  static uint8_t light_f_press = 0;
  static uint8_t light_b_press = 0;
  stop_btn_press = digitalRead(stop_btn);
  ultrasonic_btn_press = digitalRead(ultrasonic_btn);
  center_btn_press = digitalRead(center_btn);
  light_f_press = digitalRead(light_f);
  light_b_press = digitalRead(light_b);
  static uint8_t stop_btn_trig = 0;
  static uint8_t ultrasonic_btn_trig = 0;
  static uint8_t center_btn_trig = 0;
  static uint8_t light_f_trig = 0;
  static uint8_t light_b_trig = 0;
  if (!stop_btn_press && stop_btn_trig == 0)
  {
    digitalWrite(ledpin, 1);
    Serial.print("E5dStop_====");
    delay(300);
    stop_btn_press = digitalRead(stop_btn);
    if (!stop_btn_press)
    {
      Serial.print("E5dStop_====");
      delay(1600);
      stop_btn_press = digitalRead(stop_btn);
      if (!stop_btn_press)
      {
        Serial.print("E5dReset====");
        digitalWrite(ledpin, 0);
        stop_btn_trig = 1;
        delay(300);
      }
      /*while (!stop_btn_press)
      {
        stop_btn_press = digitalRead(stop_btn);
        delay(50);
      }*/
    }
  }
  else if (stop_btn_press) stop_btn_trig = 0;
  digitalWrite(ledpin, 0);
  if (!ultrasonic_btn_press && ultrasonic_btn_trig == 0)
  {
    delay(30);
    ultrasonic_btn_press = digitalRead(ultrasonic_btn);
    if (!ultrasonic_btn_press)
    {
      Serial.print("E5dUltOf====");
      delay(2000);
      ultrasonic_btn_press = digitalRead(ultrasonic_btn);
      if (!ultrasonic_btn_press)
      {
        Serial.print("E5dUltOn===="); //disable ultrasonic
        ultrasonic_btn_trig = 1;
        delay(300);
      }
      /*while (!ultrasonic_btn_press)
      {
        ultrasonic_btn_press = digitalRead(ultrasonic_btn);
        delay(50);
      }*/
    }
  }
  else if (ultrasonic_btn_press) ultrasonic_btn_trig = 0;
  if (!center_btn_press && center_btn_trig == 0)
  {
    delay(250);
    center_btn_press = digitalRead(center_btn);
    if (!center_btn_press)
    {
      forw_led_sv = !forw_led_sv;
      if (forw_led_sv) Serial.print("E5dCnOn_====");
      else Serial.print("E5dCnOf_====");
      center_btn_trig = 1;
      delay(50);
      /*while (!center_btn_press)
      {
        center_btn_press = digitalRead(center_btn);
        delay(50);
      }*/
    }
  }
  else if (center_btn_press) center_btn_trig = 0;
  
  if (!light_f_press && light_f_trig == 0)
  {
    delay(250);
    light_f_press = digitalRead(light_f);
    if (!light_f_press)
    {
      forw_led_sv = !forw_led_sv;
      if (forw_led_sv) Serial.print("E5dFlOn_====");
      else Serial.print("E5dFlOf_====");
      light_f_trig = 1;
      delay(50);
    }
  }
  else if (light_f_press) light_f_trig = 0;

  if (!light_b_press && light_b_trig == 0)
  {
    delay(250);
    light_b_press = digitalRead(light_b);
    if (!light_b_press)
    {
      back_led_sv = !back_led_sv;
      if (back_led_sv) Serial.print("E5dBlOn_====");
      else Serial.print("E5dBlOf_====");
      light_b_trig = 1;
      delay(50);
    }
  }
  else if (light_b_press) light_b_trig = 0;
}

void setup()
{
  Serial.begin(19200);
  pinMode(ledpin, OUTPUT);
  pinMode(pin_X, INPUT);
  pinMode(pin_Y, INPUT);
  pinMode(stop_btn, INPUT);
  pinMode(ultrasonic_btn, INPUT);
  pinMode(center_btn, INPUT);
  pinMode(light_f, INPUT);
  pinMode(light_b, INPUT);
  pinMode(stop_btn, INPUT_PULLUP);  
  pinMode(ultrasonic_btn, INPUT_PULLUP);
  pinMode(center_btn, INPUT_PULLUP);
}

void loop()
{
  get_button();
  getValue();
  send_data();
  //Serial.println(String(speed_left) + " | " + String(speed_right));
}