参数 1：供电驱动模式
SSD1306_SWITCHCAPVCC：屏幕内部升压供电（绝大多数 0.96 寸 OLED 默认用这个）
SSD1306_EXTERNALVCC：外部 3.3V 独立供电（极少用）
参数 2：I2C 设备地址
常见：0x3C / 0x3D，屏幕背面丝印可查，不对会初始化失败卡死
返回值：布尔型
true = 初始化成功；false = 通信失败、地址错误、接线错误
功能：启动 OLED 通信、点亮屏幕供电、初始化显存缓冲区
2. 清屏函数 display.clearDisplay()
无参数
功能：把屏幕显存全部置 0（黑屏清空所有文字图案）
注意：只清内存缓冲区，必须搭配display.display()才会刷新到屏幕
3. 文字大小设置 display.setTextSize(放大倍数)
参数：放大倍数（正整数）
1 = 标准 88 像素小字；2=1616；3=24*24，数字越大字体越大
功能：全局设置后续打印文字的缩放尺寸
4. 文字颜色 display.setTextColor(颜色值, [背景色])
参数 1：前景文字颜色
WHITE = 白色点亮（OLED 黑底白字，最常用）
BLACK = 黑色熄灭
可选参数 2：文字背景填充色
不填 = 透明背景；填BLACK= 文字带黑色底色块
5. 光标定位 display.setCursor(X坐标, Y坐标)
参数 1 X：横向起点像素坐标（范围 0 ~ WIDTH-1，0 = 最左边）
参数 2 Y：纵向起点像素坐标（范围 0 ~ HEIGHT-1，0 = 最顶部）
功能：设定下一行println()打印文字的起始位置
例：setCursor(0,10) 左上角往右 0、往下 10 像素开始写字
6. 打印文字 display.println("内容") / display.print("内容")
参数：要输出的内容
字符串："Hello ESP32"
数字变量：int num=123; display.println(num);
println()：打印完自动换行，光标跳到下一行开头
print()：打印完光标停在文字末尾，不换行
7. 刷新屏幕 display.display()
无参数
核心关键函数：前面所有清屏、写字、画图操作都只改内存缓存，调用这个才会一次性把缓存画面推送到硬件屏幕显示
高频刷新场景（比如实时数值）不要疯狂循环调用，会占用 ESP32 性能
拓展常用绘图函数（备用，后续扩展会用到）
画矩形：display.drawRect(x,y,宽,高,颜色)
填充矩形：display.fillRect(x,y,宽,高,颜色)
画直线：display.drawLine(x1,y1,x2,y2,颜色)
画圆：display.drawCircle(圆心X,圆心Y,半径,颜色)
填充圆：display.fillCircle(圆心X,圆心Y,半径,颜色)
示例代码：占用cpu打印文字

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
void setup() 
{
  Serial.begin(115200);//初始化串口通信，设置波特率为115200
  Wire.begin(SDA_PIN, SCL_PIN);//初始化I2C通信，指定数据引脚和时钟引脚
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))//初始化OLED显示屏，使用内部电源和I2C地址0x3C，如果初始化失败则进入死循环
  {
    while(1);
  }
  display.setTextSize(2);//设置文本大小为2倍
  display.setTextColor(WHITE);//设置文本颜色为白色
}

void loop()//主循环函数为空，因为我们只需要在setup函数中显示文本一次
{
  display.clearDisplay();//清空显示屏
  display.setCursor(0,10);//设置文本光标位置为(0,10)
  display.println("Hello\nDu zhengming");//在显示屏上打印"Hello\nDu zhengming"文本
  display.display();//更新显示屏以显示新的内容
  delay(500);//等待2秒钟
}