#include <Servo.h>
Servo myservo;

#include <SoftwareSerial.h>
SoftwareSerial ESPport(12, 13); 

int ledPin = 2;
int trigPin = 3;    //Триггер – зеленый проводник
int echoPin = 4;    //Эхо – желтый проводник
int servoPin = 5; 

int enA = 6;
int in1 = 7;
int in2 = 8;
// второй двигатель
int enB = 11;
int in3 = 9;
int in4 = 10;

int state = 0;

void setup() {

//Serial Port begin
Serial.begin (9600);  
ESPport.begin(9600); // ESP8266  

Serial.println(GetResponse("AT+RST",3400)); // перезагрузка ESP
Serial.println(GetResponse("AT+CWMODE_CUR=2",1000));
Serial.println(GetResponse("AT+CWSAP_CUR=\"ESP8266\",\"1234567890\",5,3",1000));
Serial.println(GetResponse("AT+CIPAP_CUR=\"192.168.1.13\"",1000));
Serial.println(GetResponse("AT+CIPSTART=\"UDP\",\"192.168.1.13\",1000,1000",1000));
Serial.println(GetResponse("AT+CWLIF",1000));

//Инициализирум входы и выходы


myservo.attach(servoPin);  

pinMode(trigPin, OUTPUT);
pinMode(ledPin, OUTPUT);

pinMode(enA, OUTPUT);
pinMode(enB, OUTPUT);
pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);

pinMode(echoPin, INPUT);
}

enum ServoState
{
  INIT,
  POSITION,
  WAIT
};
const int servoInitPosition = 90;
int servoPosition = servoInitPosition;
ServoState servoState = ServoState::INIT;

void SetServoPosition(int position)
{
  if(position < 0 || position > 180){
    servoState = ServoState::INIT;
    }
  else{
    servoPosition = position;
    servoState = ServoState::POSITION;
  }
}

void ServoLoop(){
  switch(servoState)
  {
    case ServoState::INIT:
      servoPosition = servoInitPosition;
      servoState = ServoState::POSITION;
      break;
    case ServoState::POSITION:
      myservo.write(servoPosition);
      servoState = ServoState::WAIT;
      break;    
    }
}

long duration, cm;

void EchoSensorLoop(){
// Датчик срабатывает и генерирует импульсы шириной 10 мкс или больше
// Генерируем короткий LOW импульс, чтобы обеспечить «чистый» импульс HIGH:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Считываем данные с ультразвукового датчика: значение HIGH, которое
  // зависит от длительности (в микросекундах) между отправкой
  // акустической волны и ее обратном приеме на эхолокаторе.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  // преобразование времени в расстояние
  cm = (duration/2) / 29.1;  
}

enum WheelsState{
  IDLE,
  STOP,
  FORWARD,
  BACK,
  ROTATION,
  ROTATION_ACTION,
  };

WheelsState prevWheelsState = WheelsState::STOP;
WheelsState wheelsState = WheelsState::STOP;
int wheelsSpeed = 0;
int wheelsAcc = 5;
int currentLeftSpeed = 0;
int currentRightSpeed = 0;
int int_state[4] = {LOW, LOW, LOW, LOW};
int rotationTime = 0;
int rotationSpeed = 0;
const int maxSpeed = 255;

void SetWeelsState(WheelsState state, int arg0 = 0, int arg1 =0)
{
  prevWheelsState = wheelsState;
  wheelsState = state;
  switch(wheelsState)
  {
    case WheelsState::FORWARD:
    case WheelsState::BACK:
      wheelsSpeed = arg0;
    break;
    case WheelsState::ROTATION:
    int_state[0] = digitalRead(in1);
    int_state[1] = digitalRead(in2);
    int_state[2] = digitalRead(in3);
    int_state[3] = digitalRead(in4);
    rotationTime = arg0;
    rotationSpeed = arg1;
    break;
    }
}

void WeelsLoop()
{
  switch(wheelsState)
  {
    case WheelsState::IDLE:
    break;
    case WheelsState::STOP:
      wheelsSpeed = currentLeftSpeed = currentRightSpeed = rotationTime = 0;
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      analogWrite(enA, 0);
      analogWrite(enB, 0);
      SetWeelsState(WheelsState::IDLE);
    break;
    case WheelsState::FORWARD:
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      currentLeftSpeed += wheelsAcc;
      if( currentLeftSpeed < 100)
      {
        currentLeftSpeed = 100;
      }
      currentRightSpeed = currentLeftSpeed;
      
      if(currentRightSpeed > wheelsSpeed)
      {
          SetWeelsState(WheelsState::IDLE);
      }
      else
      {
          analogWrite(enA, currentLeftSpeed);
          analogWrite(enB, currentRightSpeed);
      }
    break;

    case WheelsState::BACK:
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      currentLeftSpeed += wheelsAcc;
      if( currentLeftSpeed < 100)
      {
        currentLeftSpeed = 100;
      }
      currentRightSpeed = currentLeftSpeed;
      

      if(currentRightSpeed > wheelsSpeed)
      {
          SetWeelsState( WheelsState::IDLE);
      }
      else{
          analogWrite(enA, currentLeftSpeed);
          analogWrite(enB, currentRightSpeed);
      }
    break;
    case WheelsState::ROTATION:
    
      rotationTime --;
      if((currentLeftSpeed + rotationSpeed) > 0)
      {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        analogWrite(enA, currentLeftSpeed + rotationSpeed);
      }
      else
      {
        analogWrite(enA, (currentLeftSpeed + rotationSpeed)*(-1));
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
      }

      if((currentRightSpeed - rotationSpeed) > 0)
      {
          analogWrite(enB, currentRightSpeed - rotationSpeed);
          digitalWrite(in3, LOW);
          digitalWrite(in4, HIGH);
      }
      else
      {
          analogWrite(enB, (currentRightSpeed - rotationSpeed) *(-1));
          digitalWrite(in3, HIGH);
          digitalWrite(in4, LOW);
      }
      
      if(rotationTime <= 0)
      {
          digitalWrite(in1, int_state[0]);
          digitalWrite(in2, int_state[1]);
          digitalWrite(in3, int_state[2]);
          digitalWrite(in4, int_state[3]);
          analogWrite(enA, currentLeftSpeed);
          analogWrite(enB, currentRightSpeed);
          SetWeelsState( WheelsState::IDLE);
      }
      break;
    }
}



enum RobotState {
  FINISH,
  DELAY,
  GO_WHY_BORDER  
};

RobotState robotState = RobotState::GO_WHY_BORDER;

void SetRobotState(int state)
{
  robotState = (RobotState)state;
  if(robotState == RobotState::FINISH)
  {
    SetWeelsState(WheelsState::STOP);
  }
}

int delay1 = 0;
int cm_l = 0;
int cm_r = 0;
int cm_f = 0;
int bibRot = 0;
void RobotLoop()
{
  switch(robotState)
  {
    case RobotState::GO_WHY_BORDER:
       if( cm < 50 && delay1 >= 40)
       {
          SetWeelsState(WheelsState::STOP);
          digitalWrite(ledPin, HIGH);
          delay1 = 0;
          bibRot = cm_f = cm_r = cm_l = 0;
       }
       else{
        delay1 ++;
        if(delay1 == 1){
          cm_f = cm;
        }
        else if(delay1 == 2)
        {
          SetServoPosition(30);
        }
        else if(delay1 == 5)
        {
          cm_l = cm;
        }        
        else if(delay1 == 6)
        {
          SetServoPosition(150);
        }
        else if(delay1 == 10)
        {
          cm_r = cm;
          SetServoPosition(90);
          if( cm_r > cm_l && cm_r > cm_f && cm_r > 50)
          {
            SetWeelsState(WheelsState::ROTATION, 7, -100);
          }
          else if( cm_l > cm_r && cm_l > cm_f && cm_l > 50)
          {
            SetWeelsState(WheelsState::ROTATION, 7, 100);
          }
          else 
          {
              bibRot = cm_l > cm_r ? 1 : -1;
              SetWeelsState(WheelsState::BACK, 100);
          }
       }
       else if(delay1 == 25)
       {
           SetWeelsState(WheelsState::STOP);
       }
       else if(delay1 == 27 && bibRot!=0)
       {
          SetWeelsState(WheelsState::ROTATION, 10, 150 *bibRot);
       }            
    }        
        
       if(cm > 50 && currentLeftSpeed == 0 && delay1 >= 40)
       {
           digitalWrite(ledPin, LOW);
           SetWeelsState(WheelsState::FORWARD, 110);
       }
    break;
  }  
}

void SerialLoop()
{
    if (Serial.available() > 0) {
    // get incoming byte:
    char inByte = Serial.read();
    switch(inByte)
    {
      case '0':
      digitalWrite(ledPin, LOW);
      break;
      case '1':
      digitalWrite(ledPin, HIGH);
      break;
      case '2':
      SetServoPosition(servoPosition + 30);
      break;
      case '3':
      SetServoPosition(servoPosition - 30);
      break;
      case '4':
      Serial.print("cm = ");
      Serial.println(cm);
      break;
      case '5':
      SetServoPosition(90);
      break;
      case '6':
      SetWeelsState(WheelsState::FORWARD, 150);
      break;
      case '7':
      SetWeelsState(WheelsState::STOP);
      break;
      case '8':
      SetWeelsState(WheelsState::BACK, 150);
      break;
      case '9':
      SetWeelsState(WheelsState::ROTATION, 5, 100);
      break;
      case '-':
      SetWeelsState(WheelsState::ROTATION, 5, -100);
      break;
      case 'q':
      SetRobotState(RobotState::FINISH);
      break;
      case 'w':
      SetRobotState(RobotState::GO_WHY_BORDER);
      break;
    }
  }
}

unsigned char buffer[20];
int _left = 0;
int _leftSpeed = 0;
int _right = 0;
int _rightSpeed = 0;
int _servoPosition = 0;
int _ledState = 0;
int _avtomat = 0;
void SerialLoop1()
{
    while ( ESPport.available() > 0 ) 
    {
      int b = ESPport.read();
      for(int i = 0; i < 10; i++)
      {
        buffer[i] = buffer[i+1];
      }
      buffer[9] = b;
  
      if (strncmp((char*)(&buffer[0]), "CMD:", 4) == 0) {
          int left = buffer[4];
          int leftSpeed = buffer[5];
          int right = buffer[6];
          int rightSpeed = buffer[7];
          int servoPosition = buffer[8];
          int ledState = buffer[9];

          if( _left != left)
          {
            _left = left;
            Serial.println("left:");
            Serial.print(left);
            if(left == 0) { digitalWrite(in1, LOW); digitalWrite(in2, LOW); }
            if(left == 1) { digitalWrite(in1, LOW); digitalWrite(in2, HIGH);}
            if(left == 2) { digitalWrite(in1, HIGH); digitalWrite(in2, LOW);}
            if(left != 0)
            {
              Serial.println("leftSpeed:");
              Serial.print(leftSpeed);
              analogWrite(enA, leftSpeed);
            }
          }
          if( _right != right)
          {
            _right = right;
            Serial.println("right:");
            Serial.print(right);
            if(right == 0) { digitalWrite(in3, LOW); digitalWrite(in4, LOW); }
            if(right == 1) { digitalWrite(in3, LOW); digitalWrite(in4, HIGH);}
            if(right == 2) { digitalWrite(in3, HIGH); digitalWrite(in4, LOW);}
            if(right != 0)
            {
              Serial.println("rightSpeed:");
              Serial.print(rightSpeed);
              analogWrite(enB, rightSpeed);
            }
          }
               
          if(  _leftSpeed != !leftSpeed)
          {
            Serial.println("leftspeed:");
            Serial.print(leftSpeed);
            _leftSpeed = leftSpeed;
            analogWrite(enA, leftSpeed);
          }
          if(  _rightSpeed != !rightSpeed)
          {
            Serial.println("rightSpeed:");
            Serial.print(rightSpeed);
            _rightSpeed = rightSpeed;
            analogWrite(enB, rightSpeed);
          }
          if(_ledState != ledState)
          {
            _ledState = ledState;
            Serial.println("ledState:");
            Serial.print(ledState);
            digitalWrite(ledPin, ledState);
          }
          if(_servoPosition != servoPosition)
          {
            _servoPosition = servoPosition;
            Serial.println("servoPosition:");
            Serial.print(servoPosition);
            myservo.write(servoPosition);
          }
          
      }      
      Serial.write(b);
        
    }
}
  
int tick = 0;
void loop()
{
  EchoSensorLoop();  
  SerialLoop1();

  if(_avtomat)
  {
    delay(50);
    ServoLoop();
    WeelsLoop();
    RobotLoop();
  }
}

String GetResponse(String AT_Command, int wait)
{
  String tmpData;
  
  ESPport.println(AT_Command);
  delay(wait);
  while (ESPport.available() >0 )  
   {
    char c = ESPport.read();
    tmpData += c;
    
    if ( tmpData.indexOf(AT_Command) > -1 )         
      tmpData = "";
    else
      tmpData.trim();       
          
   }
  return tmpData;
}
