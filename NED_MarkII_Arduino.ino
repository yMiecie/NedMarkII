/****************************/
/* Author:  MIECIELICA YANN */
/* Date:    08/11/2012 20:22*/
/* P-Name:  NED Mark 2      */
/****************************/


#include <Keypad.h> // inclusion de la librairie pour clavier matriciel

#include <BSerial.h> // inclusion de la librairie pour LCD Controller BV4308 with VT100a interface

#include <Event.h>
#include <Timer.h>

// Serial LCD
#define rxPin 1
#define txPin 0

BSerial _lcd(rxPin, txPin); // RX, TX

// MACRO des STEP
const int STEP_INIT    = 0;
const int STEP_Q_TIMER = 1;
const int STEP_Q_PIN   = 2;
const int STEP_Q_WIRE  = 3;
const int STEP_Q_DIFF  = 4;
const int STEP_Q_CONFIRM = 5;
const int STEP_C_TIMER = 6;
const int STEP_C_PIN   = 7;
const int STEP_C_WIRE  = 8;
const int STEP_C_DIFF  = 9;
const int STEP_C_CONFIRM = 10;
const int STEP_START   = 11;
const int STEP_RUN     = 12;
const int STEP_STOP    = 13;

const int DIFF_EASY   = 1;
const int DIFF_MEDIUM = 2;
const int DIFF_HARD   = 3;

// Macro for wire pin
const char WIRE_1 = 9;
const char WIRE_2 = 10;
const char WIRE_3 = 11;
const char WIRE_4 = 12;

const char WIRE_HALF  = 'H';
const char WIRE_SPEED = 'S';
const char WIRE_DIFFU = 'D';
const char WIRE_BURST = 'B';

// Macro for buzzer and LED pin
const int LED          = 6;
const int BUZZER_BEEP  = 7;
const int BUZZER_BOOM  = 8;

// var for diffusing pin
const int NUMBER_OF_PIN_DIGIT = 8;
String _pin = "********";
String _tipingPin = "********";

// Difficulty Mode
int  _difficulty;

// Wire
String _wire = "0000";

// Macro for matrix Keypad pin
const int L4  = 2;
const int L3  = 3;
const int L2  = 4;
const int L1  = 5;
const int C4  = 16;
const int C3  = 17;
const int C2  = 18;
const int C1  = 19;

//--- Constantes utilisées avec le clavier 4x4
const byte LIGNS = 4;
const byte COLUMNS = 4;

//--- Définition des touches
char keys[LIGNS][COLUMNS] = {
  {
    '1','2','3', 'A'  }
  ,
  {
    '4','5','6', 'B'  }
  ,
  {
    '7','8','9', 'C'  }
  ,
  {
    '*','0','#', 'D'  }
};

// tableaux de lignes et colonnes
byte pinLigns[LIGNS] = {
  L1, L2, L3, L4}; //connexions utilisées pour les broches de lignes du clavier
byte pinColumns[COLUMNS] = {
  C1, C2, C3, C4}; //connexions utilisées pour les broches de colonnes du clavier

char  _key; // variable de stockage valeur touche appuyée

int   _step;

/*volatile int seconde=0; */

int  _hours;
int  _minutes;
int  _seconds;
int  _milliseconds;
int  _beepSpeed;
int  _beep;

Timer t;
int   t_ID;
int   _timeSpeed = 100;

// création d'un objet keypad
Keypad keyPad = Keypad( makeKeymap(keys), pinLigns, pinColumns, LIGNS, COLUMNS );

void setup()
{ 
  pinMode(BUZZER_BOOM, OUTPUT);
  pinMode(BUZZER_BEEP, OUTPUT); 
  pinMode(LED, OUTPUT);
  
  pinMode(WIRE_1, INPUT);
  pinMode(WIRE_2, INPUT);
  pinMode(WIRE_3, INPUT);
  pinMode(WIRE_4, INPUT);
  
  digitalWrite(WIRE_1, HIGH) ;  
  digitalWrite(WIRE_2, HIGH) ;  
  digitalWrite(WIRE_3, HIGH) ;  
  digitalWrite(WIRE_4, HIGH) ;

  randomSeed(analogRead(0));

  _lcd.baud(9600);
  _lcd.putch(13);
  delay(200);
  _lcd.puts ("\e[2m");
  _step = STEP_INIT;
  _beepSpeed = 9;
  _beep = 0;
} 

void loop()
{
  if (_step == STEP_INIT)
    stepInit();
  else if (_step == STEP_Q_TIMER || _step == STEP_C_TIMER)
    stepTimer();
  else if (_step == STEP_Q_PIN || _step == STEP_C_PIN)
    stepPin();
  else if (_step == STEP_Q_WIRE || _step == STEP_C_WIRE)
    stepWire();
  else if (_step == STEP_Q_DIFF || _step == STEP_C_DIFF)
    stepDiff();
  else if (_step == STEP_Q_CONFIRM || _step == STEP_C_CONFIRM)
    stepConfirm();
  else if (_step == STEP_START)
    stepStart();
  else if (_step == STEP_RUN)
    stepRun();
}

void  stepInit()
{
  displayLCD(String("    NED MkII     Pwrd by Fagnar "));
  delay(2000);
  displayLCD(String(" Type of Start   Quick:A Cust:B"));
  while (_step == STEP_INIT)
  {
    _key = keyPad.getKey();
    if (_key != NO_KEY)
    {
      if (_key == 'A')
      {
        effectSoundAndLight();
        _step = STEP_Q_TIMER;
      }
      else if (_key == 'B')
      {
        effectSoundAndLight();
        _step = STEP_C_TIMER;
      }
    }
  }
}

void  stepTimer()
{
  displayLCD(String(" Timer: hh:mm:ss Prev:A  Next:B"));
  _hours         = 0;
  _minutes       = 0;
  _seconds       = 0;
  _milliseconds  = 0;
  int i = 0;
  while (_step == STEP_Q_TIMER || _step == STEP_C_TIMER)
  {
    _key = keyPad.getKey();
    if (_key != NO_KEY)
    {
      if (_key == 'A')
      {

        effectSoundAndLight();
        _step = STEP_INIT;
      }
      else if (_key == 'B')
      {
        if (i == 6)
        {
          if (_step == STEP_Q_TIMER)
          {

            effectSoundAndLight();
            _step = STEP_Q_PIN;
          }
          else if (_step == STEP_C_TIMER)
          {
            effectSoundAndLight();
            _step = STEP_C_PIN;
          }
          i++;
        }
      }
      else if (_key >= '0' && _key <= '9')
      {
        if (i == 0)
        {
          effectSoundAndLight();
          _hours = (int(_key - '0')) * 10;
          displayAtLCD(1, 9, String(_hours / 10));
          i++;
        }
        else if (i == 1)
        {
          effectSoundAndLight();
          _hours += (int(_key - '0'));
          displayAtLCD(1, 10, String(_hours % 10));
          i++;
        }
        else if (i == 2 && int(_key - '0') <= 5)
        {
          effectSoundAndLight();
          _minutes = (int(_key - '0')) * 10;
          displayAtLCD(1, 12, String(_minutes / 10));
          i++;
        }
        else if (i == 3)
        {
          effectSoundAndLight();
          _minutes += (int(_key - '0'));
          displayAtLCD(1, 13, String(_minutes % 10));
          i++;
        }
        else if (i == 4 && int(_key - '0') <= 5)
        {
          effectSoundAndLight();
          _seconds = (int(_key - '0')) * 10;
          displayAtLCD(1, 15, String(_seconds / 10));
          i++;
        }
        else if (i == 5)
        {
          effectSoundAndLight();
          _seconds += (int(_key - '0'));
          displayAtLCD(1, 16, String(_seconds % 10));
          i++;
        }
      }
    }
  }
}

void  stepPin()
{
  for (int i = 0; i < _pin.length(); i++)
    _pin.setCharAt(i, '-');
  if (_step == STEP_Q_PIN)
  {
    for (int i = 0; i < _pin.length(); i++)
    {
      _pin.setCharAt(i, char('0' + random(9)));
      delay(10);
    }
    displayLCD(String(" Pin:  " + _pin + "  Prev:A  Next:B "));
    while (_step == STEP_Q_PIN)
    {
      _key = keyPad.getKey();
      if (_key != NO_KEY)
      {
        if (_key == 'A')
        {
          effectSoundAndLight();
          _step = STEP_Q_TIMER;
        }
        else if (_key == 'B')
        {
          effectSoundAndLight();
          _step = STEP_Q_WIRE;
        }
      }
    }
  }
  else if (_step == STEP_C_PIN)
  {
    displayLCD(String(" Pin:  " + _pin + "  Prev:A  Next:B "));
    int i = 0;
    while (_step == STEP_C_PIN)
    {
      _key = keyPad.getKey();
      if (_key != NO_KEY)
      {
        if (_key == 'A')
        {
          effectSoundAndLight();
          _step = STEP_C_TIMER;
        }
        else if ( _key == 'B')
        {
          if (i == NUMBER_OF_PIN_DIGIT)
          {

            effectSoundAndLight();
            _step = STEP_C_WIRE;
          }
        }
        else if (_key >= '0' && _key <= '9' && i <= NUMBER_OF_PIN_DIGIT - 1)
        {
          effectSoundAndLight();
          _pin.setCharAt(i, _key);
          displayAtLCD(1, (8 + i), String(_key));
          i++;
        }
      }
    }
  }
}

void  stepWire()
{
  for (int i = 0; i < 4; i++)
    _wire.setCharAt(i, '0');
  if (_step == STEP_Q_WIRE)
  {
    char tmp[] = "HSDB";
    for (int i = 0; i < _wire.length();)
    {
      int iTmp = random(3);
      if (_wire[iTmp] == '0')
        _wire.setCharAt(iTmp, tmp[i++]);
      else if ((iTmp + 1) < 4 && _wire[(iTmp + 1)] == '0')
        _wire.setCharAt((iTmp + 1), tmp[i++]);
      else if ((iTmp + 2) < 4 && _wire[(iTmp + 2)] == '0')
        _wire.setCharAt((iTmp + 2), tmp[i++]);
      else if ((iTmp + 3) < 4 && _wire[(iTmp + 3)] == '0')
        _wire.setCharAt((iTmp + 3), tmp[i++]);
      delay(10);
    }
    displayLCD(String("Wire 1" + String(_wire[0]) + "/2" + String(_wire[1]) + "/3" + String(_wire[2]) + "/4" + String(_wire[3]) + " Prev:A  Next:B "));
    while (_step == STEP_Q_WIRE)
    {
      _key = keyPad.getKey();
      if (_key != NO_KEY)
      {
        if (_key == 'A')
        {
          effectSoundAndLight();
          _step = STEP_Q_PIN;
        }
        else if (_key == 'B')
        {
          effectSoundAndLight();
          _step = STEP_Q_DIFF;
        }
      }
    }
  }
  else if (_step == STEP_C_WIRE)
  {
    displayLCD(String("Wire H_/S_/D_/B_ Prev:A  Next:B "));
    int i = 0;
    char tmp[] = "HSDB";
    while (_step == STEP_C_WIRE)
    {
      _key = keyPad.getKey();
      if (_key != NO_KEY)
      {
        if (_key == 'A')
        {
          effectSoundAndLight();
          _step = STEP_C_PIN;
        }
        else if ( _key == 'B')
        {
          if (i == 4)
          {
            effectSoundAndLight();
            _step = STEP_C_DIFF;
          }
        }
        else if (i <= 3 && _key >= '1' && _key <= '4' && _wire[_key - '1'] == '0') //else if (i <= 3 && _key >= '1' && _key <= '4' && _wire.indexOf(_key) == -1)
        {
          effectSoundAndLight();
          _wire.setCharAt(_key - '1', tmp[i]);
         // _wire.setCharAt(i, _key);
          displayAtLCD(1, (7 + (3 * i)), String(_key));
          i++;
        }
      }
    }
  }
}

void  stepDiff()
{
  _difficulty = DIFF_EASY;
  displayLCD(String("Diff E:1 M:2 H:3 Prev:A  Next:B "));
  _lcd.puts("\e[4m");
  _lcd.puts("\e[1;8H");
  while (_step == STEP_C_DIFF || _step == STEP_Q_DIFF)
  {
    _key = keyPad.getKey();
    if (_key != NO_KEY)
    {
      if (_key == 'A')
      {
        if (_step == STEP_C_DIFF )
        {
          effectSoundAndLight();
          _step = STEP_C_WIRE;
        }
        else if (_step == STEP_Q_DIFF )
        {
          effectSoundAndLight();
          _step = STEP_Q_WIRE;
        }
      }
      else if ( _key == 'B')
      {
        if (_step == STEP_C_DIFF )
        {
          effectSoundAndLight();
          _step = STEP_C_CONFIRM;
        }
        else if (_step == STEP_Q_DIFF )
        {
          effectSoundAndLight();
          _step = STEP_Q_CONFIRM;
        }
      }
      else if (_key >= '1' && _key <= '3')
      {
        char    buff[32];
        String  tmp;


        effectSoundAndLight();
        tmp = "\e[1;" + String(8 + (4  * (_key - '1'))) + "H";
        tmp.toCharArray(buff, 32);
        _lcd.puts(buff);
        _difficulty = _key - '0';
      }
    }
  }
  _lcd.puts("\e[2m");
}

void  stepConfirm()
{
  displayLCD(String("   Cancel: A       Start : B    "));
  while (_step == STEP_C_CONFIRM || _step == STEP_Q_CONFIRM)
  {
    _key = keyPad.getKey();
    if (_key != NO_KEY)
    {
      if (_key == 'A')
      {
        if (_step == STEP_C_CONFIRM )
        {
          effectSoundAndLight();
          _step = STEP_C_DIFF;
        }
        else if (_step == STEP_Q_CONFIRM)
        {

          effectSoundAndLight();
          _step = STEP_Q_DIFF;
        }
      }
      else if ( _key == 'B')
      {
        effectSoundAndLight();
        _step = STEP_START;
      }
    }
  }
}

void  stepStart()
{
  displayLCD(String("To Start, press:A:_ B:_ C:_ D:_ "));
  int i = 0;
  while (_step == STEP_START)
  {
    _key = keyPad.getKey();
    if (_key != NO_KEY)
    {
      if (_key == 'A' && i == 0)
      {
          effectSoundAndLight();
          displayAtLCD(2, 3, String("x"));
          i++;
      }
      else if (_key == 'B' && i == 1)
      {
          effectSoundAndLight();
          displayAtLCD(2, 7, String("x"));
          i++;
      }
      else if (_key == 'C' && i == 2)
      {
          effectSoundAndLight();
          displayAtLCD(2, 11, String("x"));
          i++;
      }
      else if (_key == 'D' && i == 3)
      {
          effectSoundAndLight();
          displayAtLCD(2, 15, String("x"));
          _step = STEP_RUN;
      }
      else
      {
        i = 0;
        displayAtLCD(2, 3, String("_"));
        displayAtLCD(2, 7, String("_"));
        displayAtLCD(2, 11, String("_"));
        displayAtLCD(2, 15, String("_"));
      }
    }
  }
}

void  stepRun()
{
  String displayed = "   ";
  displayed += ((_hours > 0) ? ((_hours > 9) ? String(_hours) : String("0" + String(_hours))) : 
  "00");
  displayed += ":" + ((_minutes > 0) ? ((_minutes > 9) ? String(_minutes) : String("0" + String(_minutes))) : 
  "00");
  displayed += ":" + ((_seconds > 0) ? ((_seconds > 9) ? String(_seconds) : String("0" + String(_seconds))) : 
  "00");
  displayed += ".0   ";
  displayed += "    ********    ";
  displayLCD(displayed);
 // MsTimer2::start(); // active Timer 2
  t_ID = t.every(_timeSpeed, timerManager);
  while (_step == STEP_RUN)
  {
    t.update();
   // delay(100);
    //timerManager();
//    if (_beepSpeed <= _beep)
//    {
//      effectSoundAndLight();
//      _beep = 0;
//    }
//
//    _key = keyPad.getKey();
//    if (_key != NO_KEY)
//    {
//      if (_key >= '0' && _key <= '9')
//      {
//        if (_difficulty == DIFF_EASY)
//          _step = diffuseEasy();
//        if (_difficulty == DIFF_MEDIUM)
//          _step = diffuseMedium();
//        if (_difficulty == DIFF_HARD)
//          _step = diffuseHard();
//      }
//    }
  }
 // MsTimer2::stop();
  //delay(5000);
  
  
}

int   diffuseEasy()
{
  int tmp = _tipingPin.indexOf('*');
  if (_key == _pin[tmp])
  {
    _tipingPin.setCharAt(tmp, _key);
    displayAtLCD(2, (5 + tmp), String(_key));
    if (_tipingPin.indexOf('*') == -1)
      return (STEP_STOP);
  }
  return (STEP_RUN);
}

int   diffuseMedium()
{
  int tmp = _tipingPin.indexOf('*');
  if (_key == _pin[tmp])
  {
    _tipingPin.setCharAt(tmp, _key);
    displayAtLCD(2, (5 + tmp), String(_key));
    if (_tipingPin.indexOf('*') == -1)
      return (STEP_STOP);
  }
  else
  {
    for (int i = 0; _tipingPin[i] != '*' && i < _tipingPin.length(); i++)
    {
      _tipingPin.setCharAt(i, '*');
      displayAtLCD(2, (5 + i), String("*"));
    }
  }
  return (STEP_RUN);
}

int   diffuseHard()
{
  int tmp = _tipingPin.indexOf('*');
  _tipingPin.setCharAt(tmp, _key);
  displayAtLCD(2, (5 + tmp), String(_key));
  if (_tipingPin.indexOf('*') == -1)
  {
    int i = 0;
    while (i < _tipingPin.length())
    {
      if (_tipingPin[i] != _pin[i])
        {
          for (int i = 0; _tipingPin[i] != '*' && i < _tipingPin.length(); i++)
          {
            _tipingPin.setCharAt(i, '*');
            displayAtLCD(2, (5 + i), String("*"));
          }
          return (STEP_RUN);
        }
      i++;
    }
    return (STEP_STOP);
  }
  return (STEP_RUN);
}

void  checkWire()
{
 // Macro for wire pin
//const char WIRE_1 = 7;
//const char WIRE_2 = 8;
//const char WIRE_3 = 9;
//const char WIRE_4 = 10;
//
//const char WIRE_HALF  = 'H';
//const char WIRE_SPEED = 'S';
//const char WIRE_DIFFU = 'D';
//const char WIRE_BURST = 'B'; 
  int wire[4];
  wire[0] = digitalRead(WIRE_1);
  wire[1] = digitalRead(WIRE_2);
  wire[2] = digitalRead(WIRE_3);
  wire[3] = digitalRead(WIRE_4);

  int tmp = _wire.indexOf(WIRE_HALF);
  if (tmp != -1 && wire[tmp]== 1)
  {
   unsigned int totalSec = (_hours * 3600) + (_minutes * 60) + _seconds;
   totalSec = totalSec / 2;
    
    _hours = totalSec / 3600;
    _minutes = (totalSec - (_hours * 3600)) / 60;
    _seconds = totalSec - (_hours * 3600 + _minutes * 60 );

   // _hours /= 2;
   // _minutes /= 2;
   // _seconds /= 2;
    displayAtLCD(1, 4,  ((_hours > 0) ? ((_hours > 9) ? String(_hours) : String("0" + String(_hours))) : "00"));
    displayAtLCD(1, 7, ((_minutes > 0) ? ((_minutes > 9) ? String(_minutes) : String("0" + String(_minutes))) : "00") );
    displayAtLCD(1, 10, ((_seconds > 0) ? ((_seconds > 9) ? String(_seconds) : String("0" + String(_seconds))) : "00"));
    _wire.setCharAt(tmp, '0');
  }
  tmp = _wire.indexOf(WIRE_SPEED);
  if (tmp != -1 && wire[tmp]== 1)
  {
    _wire.setCharAt(tmp, '0');
    t.stop(t_ID);
    _timeSpeed = 50;
    t_ID = t.every(_timeSpeed, timerManager);
  }
  tmp = _wire.indexOf(WIRE_DIFFU);
  if (tmp != -1 && wire[tmp]== 1)
  {
    _wire.setCharAt(tmp, '0');
    _step = STEP_STOP;
  }
  tmp = _wire.indexOf(WIRE_BURST);
  if (tmp != -1 && wire[tmp]== 1)
  {
    _wire.setCharAt(tmp, '0');
    burstEffect();
    _step = STEP_STOP;
  }

}

void  timerManager()
{
  checkWire();
  
  if (_beepSpeed <= _beep)
  {
     effectSoundAndLight();
    _beep = 0;
  }
    
  _key = keyPad.getKey();
  if (_key != NO_KEY)
  {
    if (_key >= '0' && _key <= '9')
    {
      if (_difficulty == DIFF_EASY)
        _step = diffuseEasy();
      if (_difficulty == DIFF_MEDIUM)
        _step = diffuseMedium();
      if (_difficulty == DIFF_HARD)
        _step = diffuseHard();
    }
  }
    
  _milliseconds=_milliseconds-1; // décrémentation des millisecondes... 

 if (_hours == 0 && _minutes == 0)
 { 
    if (_seconds <= 59 && _beepSpeed == 9 && _milliseconds == -1)   //9
        _beepSpeed--;
    if (_seconds <= 49 && _beepSpeed == 8 && _milliseconds == -1)    //8
        _beepSpeed--;
    if (_seconds <= 39 && _beepSpeed == 7 && _milliseconds == -1)    //7
        _beepSpeed--;
    if (_seconds <= 29 && _beepSpeed == 6 && _milliseconds == -1)    //6
        _beepSpeed--;
    if (_seconds <= 19 && _beepSpeed == 5 && _milliseconds == -1)    //5
        _beepSpeed--;
    if (_seconds <= 9 && _beepSpeed == 4 && _milliseconds == -1)     //4
        _beepSpeed--;
    if (_seconds <= 5 && _beepSpeed == 3 && _milliseconds == -1)     //3
        _beepSpeed--;
    if (_seconds <= 3 && _beepSpeed == 2 && _milliseconds == -1)     //2
        _beepSpeed--;
    if (_seconds <= 1 && _beepSpeed == 1 && _milliseconds == -1)     //1
        _beepSpeed--;
 }

  _beep++;
  if (_milliseconds != -1)
    displayAtLCD(1, 13, String(_milliseconds));
  if (_milliseconds == -1)
  {
    _seconds=_seconds-1,_milliseconds=9;
    displayAtLCD(1, 13, String("9"));
    //effectSoundAndLight();
    if (_seconds != -1)
    {
      if (_seconds % 10 == 9)
        displayAtLCD(1, 10, String(_seconds / 10));
      displayAtLCD(1, 11, String(_seconds % 10));
    }
  }
  if (_seconds == -1)
  {
    _minutes=_minutes-1,_seconds=59;
    displayAtLCD(1, 10, String("59"));
    if (_minutes != -1)
    {
      if (_minutes % 10 == 9)
        displayAtLCD(1, 7, String(_minutes / 10));
      displayAtLCD(1, 8, String(_minutes % 10));
    }
  }
  if (_minutes==-1)
  {
    _hours=_hours-1, _minutes=59;
    displayAtLCD(1, 7, String("59"));
    if (_hours % 10 == 9)
      displayAtLCD(1, 4, String(_hours / 10));
    displayAtLCD(1, 5, String(_hours % 10));
  }
  
  if (_minutes == 0 && _hours == 0 && _seconds == 0 && _milliseconds == 0)
  {
    burstEffect();
    _step = STEP_STOP;
    //MsTimer2::stop();
  }
  //else if (_step == STEP_STOP)
  //  MsTimer2::stop();
}

/*****************************************/
/******      Display functions      ******/
/*****************************************/

void  displayLCD(String str)
{
  char buff[32];

  str.toCharArray(buff, 32);
  _lcd.puts("\e[2J");
  _lcd.puts(buff);
  delay(10);
}

void  displayAtLCD(int row, int col, String str)
{
  String tmp;
  char   buff[50];

  tmp = String("\e[" + String(row) + ";" + String(col) + "H" + str);
  tmp.toCharArray(buff, 50);
  _lcd.puts(buff);
  //delay(40);
}

/*****************************************/
/******   Sound & Light Function    ******/
/*****************************************/

void burstEffect()
{
  digitalWrite(BUZZER_BEEP, HIGH);
  digitalWrite(BUZZER_BOOM, HIGH);
  digitalWrite(LED,HIGH); // met la broche au niveau haut (5V) – allume la LED
  delay(5000); // pause de 500 millisecondes (ms)
  digitalWrite(LED,LOW);
  digitalWrite(BUZZER_BEEP, LOW);
  digitalWrite(BUZZER_BOOM, LOW); 
}

void  effectSoundAndLight()
{
  digitalWrite(BUZZER_BEEP, HIGH);
  //analogWrite(BUZZER_BEEP, 10);
  digitalWrite(LED,HIGH); // met la broche au niveau haut (5V) – allume la LED
  delay(40); // pause de 500 millisecondes (ms)
  digitalWrite(LED,LOW);
  digitalWrite(BUZZER_BEEP, LOW);
  //analogWrite(BUZZER_BEEP, 0);
}

/*
  Serial.println(+----------------+);
 Serial.println(|    NED MkII    |);
 Serial.println(| Pwrd by Fagnar |);
 Serial.println(+----------------+);
 Serial.println(+----------------+);
 Serial.println(| Type of Start  |);
 Serial.println(| Quick:1 Cust:2 |);
 Serial.println(+----------------+);
 
 Serial.println(+----------------+);
 Serial.println(| Timer: hh:mm:ss|);
 Serial.println(| Prev:*  Next:# |);
 Serial.println(+----------------+);
 
 Serial.println(+----------------+);
 Serial.println(| Pin:  -------- |);
 Serial.println(| Prev:*  Next:# |);
 Serial.println(+----------------+);
 
 Serial.println(+----------------+);
 Serial.println(|Wire 1H/2S/3B/4D|);
 Serial.println(| Prev:*  Next:# |);
 Serial.println(+----------------+);
 
 Serial.println(+----------------+);
 Serial.println(|Diff E:1 M:2 H:3|);
 Serial.println(| Prev:*  Next:# |);
 Serial.println(+----------------+);
 
 Serial.println(+----------------+);
 Serial.println(|   Cancel: *    |);
 Serial.println(|   Start:  #    |);
 Serial.println(+----------------+);
 
 Serial.println(+----------------+);
 Serial.println(|   00:00:00.0   |);
 Serial.println(|    ********    |);
 Serial.println(+----------------+);
 
 Serial.println(+----------------+);
 Serial.println(|  --:--:00.000  |);
 Serial.println(|    ********    |);
 Serial.println(+----------------+);
 */

