#include <Arduino.h>
#include <Button.h>
//Shift+Alt+F format
#define pin_X A0
#define pin_Y A1
#define BTN_BLD_ON 12
#define BTN_BLD_OFF 10

#define BTN_A 8
#define BTN_X 9

#define TIME_SEND       100 

#define AxeXZero 508
#define AxeYZero 558
#define k 20 //4
#define diff 70 //20
#define max_val 390 //512
#define min_val -390 //512
#define max_speed 250
#define min_speed -250

//       Y
//      900
//       |
// <<900---100>> X
//       |
//      100


#define RC_START_FRAME      0xACBD

#define BLADE_ON            0x0101
#define BLADE_OFF           0x0202

int16_t speed_left, speed_right;
int32_t pos_Y, pos_X;
uint8_t zero_count = 0;
int16_t blade_st;

typedef struct{
   uint16_t start;
   int16_t  speedR;
   int16_t  speedL;
   uint16_t  cmd0;
   uint16_t  cmd1;
   uint16_t checksum;
} RCSerialControl;
RCSerialControl RC_Command;

Button btn_blade_on(BTN_BLD_ON);
Button btn_blade_off(BTN_BLD_OFF);
Button btn_a(BTN_A);
Button btn_x(BTN_X);

void getValue()
{
  pos_Y = (analogRead(pin_Y) - AxeYZero);
  if (pos_Y < k && pos_Y > (-1 * k))
    pos_Y = 0;
  pos_X = analogRead(pin_X);
  //Serial.println(String(analogRead(pin_Y)) + " | " + String(analogRead(pin_X)));
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
  RC_Command.start  = (uint16_t)RC_START_FRAME;
  RC_Command.speedL  = -(int16_t)speed_left;
  RC_Command.speedR  = -(int16_t)speed_right;
  RC_Command.cmd0  = (uint16_t)blade_st;
  RC_Command.cmd1  = (uint16_t)0x0000;
  RC_Command.checksum = (uint16_t)(RC_Command.start ^ RC_Command.speedR ^ RC_Command.speedL ^ RC_Command.cmd0 ^ RC_Command.cmd1);
  if (speed_left != 0 || speed_right != 0)
  {
    zero_count = 0;
    Serial.write((uint8_t *) &RC_Command, sizeof(RC_Command));
  }
  else if (zero_count < 3)
  {
    zero_count++;
    Serial.write((uint8_t *) &RC_Command, sizeof(RC_Command));
  }
}
void send_button()
{
  RC_Command.start  = (uint16_t)RC_START_FRAME;
  RC_Command.speedL  = (int16_t)speed_left;
  RC_Command.speedR  = (int16_t)speed_right;
  RC_Command.cmd0  = (uint16_t)blade_st;
  RC_Command.cmd1  = (uint16_t)0x0000;
  RC_Command.checksum = (uint16_t)(RC_Command.start ^ RC_Command.cmd0 ^ RC_Command.cmd1 ^ RC_Command.speedR ^ RC_Command.speedL);
  Serial.write((uint8_t *) &RC_Command, sizeof(RC_Command));
}
void get_button()
{
  if (btn_blade_on.pressed())
  {
		blade_st = (uint16_t)BLADE_ON;
    send_button();
  }
	if (btn_blade_off.pressed())
  {
		blade_st = (uint16_t)BLADE_OFF;
    send_button();
  }
}

void setup()
{
  Serial.begin(19200);
  pinMode(pin_X, INPUT);
  pinMode(pin_Y, INPUT);
  btn_blade_on.begin();
  btn_blade_off.begin();
  btn_a.begin();
  btn_x.begin();
}

unsigned long iTimeSend = 0;

void loop()
{
  unsigned long timeNow = millis();
  get_button();
  if (iTimeSend < timeNow)
  {
    getValue();
    send_data();
    iTimeSend = timeNow + TIME_SEND;
    //Serial.println(String(speed_left) + " | " + String(speed_right));
  }  
}