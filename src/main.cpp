#include <Arduino.h>
#include <Wire.h>//引入I2C通信库
#include <Adafruit_GFX.h>//引入Adafruit GFX库，提供图形绘制功能
#include <Adafruit_SSD1306.h>//引入Adafruit SSD1306库，提供对SSD1306 OLED显示屏的支持

#define WIDTH 128//定义显示屏的宽度为128像素
#define HEIGHT 64//定义显示屏的高度为64像素
#define SCL_PIN 11//定义I2C通信的时钟引脚为11
#define SDA_PIN 12//定义I2C通信的数据引脚为12

Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, -1);//创建一个Adafruit_SSD1306对象，命名为display，使用之前定义的宽度、高度、I2C对象和复位引脚
//在setup函数中初始化串口通信、I2C通信和OLED显示屏，并在显示屏上显示"Hello ESP32"文本
void setup() {
  Serial.begin(115200);//初始化串口通信，设置波特率为115200
  Wire.begin(SDA_PIN, SCL_PIN);//初始化I2C通信，指定数据引脚和时钟引脚
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))//初始化OLED显示屏，使用内部电源和I2C地址0x3C，如果初始化失败则进入死循环
  {
    while(1);
  }
  display.clearDisplay();//清空显示屏
  display.setTextSize(1);//设置文本大小为1
  display.setTextColor(WHITE);//设置文本颜色为白色
  display.setCursor(0,10);//设置文本光标位置为(0,10)
  display.println("Hello ESP32");//在显示屏上打印"Hello ESP32"文本
  display.display();//将缓冲区的内容显示在屏幕上
}

void loop()//主循环函数为空，因为我们只需要在setup函数中显示文本一次
{
  
}