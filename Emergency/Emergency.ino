#include <MsTimer2.h>

#include <SR04.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define Echo 3
#define Trig 4
#define Inf_PIN 2
#define Servo_PIN 9
#define BT_TX 11
#define BT_RX 12
SR04 sr04=SR04(Trig,Echo);
SoftwareSerial BT (12,13);
long count=0;
Servo servo;
int posi=0;
int check_val;
long ave[90];
long err[90];
int val;
//pinMode(5,OUTPUT);

void setup() {
servo.attach(Servo_PIN);
servo.write(0);
pinMode(Inf_PIN,INPUT);
pinMode(11,OUTPUT);
Serial.begin(38400); //Serial for debug
BT.begin(38400);
digitalWrite(11,LOW);
 LearningTask();
 digitalWrite(11,HIGH);
  MsTimer2::set(100,TIM_task);
  MsTimer2::start();
}

//传进来的数据是什么样的？如何处理传入数据不完全的问题？解决方案1 encryption
void TIM_task(){
  if(BT.available()){
  val=BT.read();
  //Serial.print(val);
  if(val==120){
    digitalWrite(11,LOW);
  }
  if (val==0){
    digitalWrite(11,HIGH);
  }
  }
}
void loop()
{
 // if(val==120)
 //{
 //if(sr04.GetDistance()<100 && digitalRead(2)==HIGH)
 //{
 // MsTimer2::stop();
 // digitalWrite(11,HIGH);
 //}
 //else{
 // digitalWrite(11,LOW);
 Detecttask();
 //}
 }
 //Serial.println(digitalRead(Inf_PIN));
 }

/*int Maxmin(long variable[40],minmax *mnmx)
{
    long Max1=variable[0];
    long Min1=variable[0];
    for(int i=0;i<40;i++)
    {
      if(variable[i]>Max1)
      {
        Max1=variable[i];
      }
      if(variable[i]<Min1)
      {
        Min1=variable[i];
      }
    }
    mnmx->Min=Min1;
    mnmx->Max=Max1;
}*/

int LearningTask()
{
    long a1;
    long averageGet[2][90];//40 180
    while (1) {
      for(int i=0;i<90;i++)
      {
       servo.write(2*i);
       averageGet[0][i]=sr04.GetDistance();
       //Serial.print( sr04.GetDistance());
       Serial.println(averageGet[0][i]);
       delay(150);
      }
      delay(300);
      for(int j=90;j>0;j--)
      {
       servo.write(2*j);
       averageGet[1][j-1]=sr04.GetDistance();
       //Serial.println(averageGet[1][j-1]);
       delay(150);
      }
      if(count<=4)
      {
       count++;
       for(int data=0;data<90;data++)
       {
         ave[data]+=(averageGet[0][data]+averageGet[1][data]);
         err[data]+=(abs(averageGet[0][data]-(ave[data])/(2))+abs(averageGet[1][data]-(ave[data])/(2))); //计算每次数据与当时平均数的偏差值
       }
       
      }
      else
      {
      for(int d;d<90;d++)
      {
        ave[d]=ave[d]/10;
        err[d]=err[d]/10;
      }
      break;
      }
    }    
}

void DetactTask()
{
      MsTimer2::stop();     
      long Template[90];
      int pin_value;
      int counter;
      int direct;
again1: counter=0;
        for(int angle;angle<180;angle+=2)
        {
          servo.write(angle);
          Template[angle]=sr04.GetDistance();
          pin_value=digitalRead(Inf_PIN);
          delay(160);
          if ((Template[angle]-ave[angle])>err[angle])
          {
            counter++;
          }
          if(counter>2 && pin_value==1)
          {
            direct=1;
            Avoidtask(angle,direct);
            goto again1;
          }
          else{counter--;}      
         }
         delay(1000);
again2: counter=0;
        for(int angle=180;angle>0;angle-=2)
        {
          servo.write(angle);
          Template[angle]=sr04.GetDistance();
          pin_value=digitalRead(Inf_PIN);
          delay(160);
          if ((Template[angle]-ave[angle])>err[angle])
          {
            counter++;
          }
          if(counter>2 && (pin_value==1))
          {
            direct=2;
            Avoidtask(angle,direct);
            goto again2;
          }      
          else
          {
            counter--;
          }
         }
MsTimer2::start(); 
delay(1000);        
}

void Avoidtask(int angle,int direct) //For simplicity only can detact and avoid one person
{
  if(direct==1)
  {
    for(int ag=angle;ag>0;ag--)
    {
      servo.write(ag);
      delay(35);
    }
  }
  if (direct==2)
  {
    for(int ag=angle;ag<180;ag++)
    {
      servo.write(ag);
      delay(35);
    }
  }
}
//与动态内存应用作斗争的一段程序
