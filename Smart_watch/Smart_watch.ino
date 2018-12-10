#include <dht11.h>

//库函数包括区
#include "U8glib.h"
#include <dht11.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MsTimer2.h>
//全局宏定义区
#define Register_ID 0
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37
#define MENU_ITEMS 4
#define BT_TX 6
#define BT_RX 7
#define SWITCH_PIN 3
#define RESISTOR_PIN A1
#define NULL_VAL 0
#define WAKE_VAL 1
#define SELECT_VAL 2
#define CONF_VAL 3
#define SLEEP_MODE 2
//面向对象实例存放区
U8GLIB_SSD1306_128X64 u8g(13, 11, 10, 9);  // SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
dht11 DHT;
SoftwareSerial BT(BT_TX,BT_RX);
//结构体定义区

int Xg;
int Yg;
int Zg;
float A;
//char s[4];
  //char f[4];
String mark="?";
int temperature;
int humidity;
//常量变量声明区


int ADXAddress = 0xA7>>1;  //转换为7位地址
int reading = 0;
int val = 0;
int X0,X1,X_out;
int Y0,Y1,Y_out;
int Z1,Z0,Z_out;
uint8_t menu_current = 0;
uint8_t menu_redraw_required = 0;
char *menu_strings[MENU_ITEMS] = { "RETURN HOME", "GO TO SLEEP", "LEAVE  HOME" };
int TXD;
int RXD;
int resis=0;
uint8_t check_val=1;
int night_counter=0;
int m;
int minutes=0;
int minutes1=0;
int hours=20;
int timer=0;
int timer2=0;
int buf;
char s;
char f;
int modec=0;
int map_val=0;
//任务函数区
int draw() 
{
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0, 30); 
  u8g.print("Time: ");
  if(hours<10)
    {u8g.print(0);
     u8g.print(hours);
    }
   else{ u8g.print(hours);}
  u8g.print(" : ");
  if(minutes<10)
    {u8g.print(0);
     u8g.print(minutes);
    }
  else{ u8g.print(minutes);}
  
  //u8g.println("Temp");
  //u8g.println(temperature);
  //u8g.println("Hum");
  //u8g.println(humidity);
  
}

int draw2()
{
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0, 30);
  if(menu_current==0)
  {
    u8g.print("Msg Sent");
    u8g.print("T:");
    u8g.print(temperature);
    u8g.print("H:"); 
    u8g.print(humidity);
  }
  if((menu_current==1)||(menu_current==2))
  {
    u8g.print("   ");
    u8g.print("Msg Sent");
  }
  
}

void switch_task() //use different number represent different tasks
{
  
   
  detachInterrupt(SWITCH_PIN);
  delay(5);
  if(digitalRead(SWITCH_PIN)<5)
  {
     
  if(check_val==WAKE_VAL)
  {
    check_val=SELECT_VAL;
    return;
  }
  if(check_val==SELECT_VAL)
  {int value;
     value=map(analogRead(A1),0,1023,1,4);
    check_val=CONF_VAL;
    if(value==1)
    {
     modec=0; 
     BT.print(temperature+mark+humidity+mark+10.14+mark+modec);
     return;
     }
    if(value==2)
    {
    modec=2;
     BT.print(temperature+mark+humidity+mark+9.87+mark+modec); 
     return; 
    }
   if(value==3) 
   {
    modec=1;
     BT.print(temperature+mark+humidity+mark+9.92+mark+modec);
     return;
   }
    return;
    
  }
  if(check_val==CONF_VAL)
  {
   check_val=WAKE_VAL;
   return;
  }
  }
}

void drawMenu(int menu_current) {
  uint8_t i, h;
  u8g_uint_t w, d;
  //menu_current=1;
  u8g.setFont(u8g_font_6x13);
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();
  
  h = u8g.getFontAscent()-u8g.getFontDescent();
  w = u8g.getWidth();
   d = (w-u8g.getStrWidth("Menu"))/2;
  u8g.drawStr(d, 0, "Menu");
  for( i = 0; i < MENU_ITEMS; i++ ) {
    d = (w-u8g.getStrWidth(menu_strings[i]))/2;
    u8g.setDefaultForegroundColor();
    if ( i == menu_current ) {
      u8g.drawBox(0, (i+2)*h+1, w, h);
      u8g.setDefaultBackgroundColor();
    }
    u8g.drawStr(d, (i+2)*h, menu_strings[i]);
  }
}

int updateMenu(void) 
{ //map_value uncertain
  
  map_val=map(analogRead(RESISTOR_PIN),0,1023,1,4);
  switch ( map_val ) {
    case 1:
      return menu_current=0;
      break;
    case 2:
      return menu_current=1;
      break;
    case 3:
      return menu_current=2;
      break;
  }
}

void acc_task()
{
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_X0);
  Wire.write(Register_X1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2);
  if(Wire.available()<=2);
  {
    X0 = Wire.read();
    X1 = Wire.read();
    X1 = X1<<8;
    X_out = X0+X1;
  }

  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_Y0);
  Wire.write(Register_Y1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2);
  if(Wire.available()<=2);
  {
    Y0 = Wire.read();
    Y1 = Wire.read();
    Y1 = Y1<<8;
    Y_out = Y0+Y1;
  }

  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_Z0);
  Wire.write(Register_Z1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2);
  if(Wire.available()<=2);
  {
    Z0 = Wire.read();
    Z1 = Wire.read();
    Z1 = Z1<<8;
    Z_out = Z0+Z1;
  }

  Xg = (int)(X_out/25.6);//把输出结果转换为重力加速度g,精确到小数点后2位。
  Yg = (int)(Y_out/25.6);
  Zg = (int)(Z_out/25.6);
  A=sqrt(sq(Xg)+sq(Yg)+sq(Zg));
  
}
int night_wakeup()
{ if(minutes1==minutes)
  {if(abs(A-7)>5)
  {
    night_counter++;
  }
  if((night_counter>50))
  {
    return 1; 
    }  
  }
  else
  {minutes1=minutes;
   return 0;}
  }


void Get_body_param()     //取十五次的平均数，保证结果的准确性 45ms
{
  do{
   }while(DHT.read(4)!=0) ;
    temperature=DHT.temperature;
    humidity=DHT.humidity;
   
   temperature=int(temperature);
   humidity=int(humidity);
  
}

/*void Bluetooth_task
{
  
}*/
void TIM_task()
{
  minutes++;
  if(minutes==60)
  {
   hours++;
   minutes=0;
  }
  if(hours==24)
  {
    hours=0;
  }
}
void setup(void) {
  delay(100);
  Wire.begin();  //初始化I2C
  delay(100);
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_2D);
  Wire.write(8);
  Wire.endTransmission();
  
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  Serial.begin(9600);
  BT.begin(9600); //设置蓝牙的虚拟串口
  pinMode(SWITCH_PIN,INPUT_PULLUP);
  pinMode(RESISTOR_PIN,INPUT);
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN),switch_task,FALLING);
  MsTimer2::set(60000,TIM_task);
  MsTimer2::start();
  
}

void loop(void) {
    Get_body_param();
    if(check_val==SELECT_VAL)
      { timer2=0;
        timer++;
        u8g.firstPage();
        do  {
          drawMenu(m);
        } while( u8g.nextPage() );
        menu_redraw_required = 0;
        m=updateMenu();
        if(timer>=70)
        {check_val=WAKE_VAL;
         timer=0;}                    
      }
    if((check_val==WAKE_VAL)){
        timer=0;
        timer2=0;
        u8g.firstPage();
          do{
          draw();
          }while(u8g.nextPage());
         }
     if(check_val==CONF_VAL)
     {
      timer=0;
      timer2++;
      u8g.firstPage();
          do{
        
          draw2();
          delay(25);
          }while(u8g.nextPage());
          if(timer2>=5)
        {check_val=WAKE_VAL;
         timer2=0;}  
     }
     if(modec==SLEEP_MODE)
     {
      //if(night_wakeup()==1)
      //{
        //BT.write("WAKE");
      //}
     }
     //BT.write("1");
     //BT.write("3");
     delay(25);
     acc_task();
     Serial.println(A);
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN),switch_task,FALLING);
}
