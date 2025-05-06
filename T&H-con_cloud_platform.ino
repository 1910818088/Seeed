#include <DHT.h>      
#include <Wire.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//LED灯模块配置
#define LED_PIN 7  // 定义LED引脚

//温湿度模块配置
#define DHTPIN 8           // 定义温湿度传感器引脚
#define DHTTYPE DHT11      // 指定温湿度传感器为DHT11类型
DHT dht(DHTPIN, DHTTYPE);  // 初始化DHT11传感器对象

//光敏模块配置
#define LIGHT_SENSOR_PIN 9     // 光敏模块信号引脚连接至9引脚
#define DARK_THRESHOLD 4000    // 弱光阈值（根据实际调整）
#define BRIGHT_THRESHOLD 1000  // 强光阈值（根据实际调整）

//OLED配置
#define SCREEN_WIDTH 128       // OLED显示宽度
#define SCREEN_HEIGHT 32       // OLED显示高度
#define OLED_RESET -1          // 重置引脚（-1表示共享Arduino复位引脚）
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//WiFi配置
const char* ssid = "luoyi";         //WiFi账号
const char* password = "88888888";  //WiFi密码

//阿里云IoT三元组配置
#define PRODUCT_KEY "gmsc1t6K6WG"   //产品密钥
#define DEVICE_NAME "Seeed"         //设备名称
#define DEVICE_SECRET "4d917a37aa5e933a96bb8275c6589e20"  //设备密钥

//MQTT配置
const char* mqttServer = "iot-06z00fvge4b2wv4.mqtt.iothub.aliyuncs.com";                            //MQTT服务器
const int mqttPort = 1883;                                                                          //端口号
String clientId = "gmsc1t6K6WG.Seeed|securemode=2,signmethod=hmacsha256,timestamp=1746431333269|";  //客户端ID
String username = "Seeed&gmsc1t6K6WG";                                                              //用户名
String passwordStr = "7d2f5f9a8b5efb807f27be798545ae2314bb732120a1a0ca4c1ff6bf28b373c5";            //密钥

WiFiClient espClient;             //使用普通WiFiClient
PubSubClient client(espClient);   //client对象用于连接MQTT服务器并发布/订阅消息

void setup() {
  Serial.begin(9600); //初始化串口通信

  dht.begin();                      //启动温湿度传感器
  delay(500);                       //延迟等待温湿度传感器稳定
  
  pinMode(LED_PIN, OUTPUT);         //将LED灯引脚设置为输出模式 
  digitalWrite(LED_PIN, LOW);       //给LED引脚输出低电平（默认LED灯为关闭状态）

  pinMode(LIGHT_SENSOR_PIN, INPUT); //设置光敏模块引脚为输入模式

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306分配失败"));
    for(;;);  //死循环，阻止程序继续执行
  }

  display.clearDisplay();               //清空屏幕缓冲区
  display.setTextColor(SSD1306_WHITE);  //白色文本
  display.setTextSize(0.5);               //切换到大字体
  display.setCursor(30,12);             //调整位置
  display.println("Seeed Studio");      //输出字符
  display.display();                    //将缓冲区内容输出到屏幕
  delay(2000);

  connectWiFi();                        //执行WiFi连接函数
  connectAliyunMQTT();                  //执行MQTT连接函数
}

void loop() {
  //检查MQTT客户端连接状态
  if (!client.connected()) {   //如果检测到客户端未连接
    nonBlockingReconnect();    //执行重连操作
  }
  client.loop();               //必须保持client循环执行
  
  display.clearDisplay();                        //清空屏幕缓冲区
  
  int rawValue = analogRead(LIGHT_SENSOR_PIN);   //读取模拟值（0-1023）
  float voltage = rawValue * (5.0 / 1023.0);     //计算电压值（0-5V）

  //设备数据定时上报逻辑（5秒一次）
  static unsigned long lastMsg = 0;     //静态变量，记录上次发送时间（保持状态持久化）
  if (millis() - lastMsg > 5000) {      //时间差检测（非阻塞式延迟）
    lastMsg = millis();                 //更新时间戳，重置计时器
    publishSensorData();                //执行传感器数据发布函数（需实现具体逻辑）
  }
}

//WiFi连接函数
void connectWiFi() {
  Serial.print("Connecting to WiFi");          //启动连接提示
  WiFi.begin(ssid, password);                  //初始化WiFi连接
  //阻塞式连接等待循环
  while (WiFi.status() != WL_CONNECTED) {      //检测连接状态
    delay(500);                                //延时避免快速轮询（500ms检测间隔）
    Serial.print(".");                         //连接进度指示
  }
  Serial.println("\nConnection successful!");  //连接成功提示
}

//MQTT连接函数
void connectAliyunMQTT() {
  client.setServer(mqttServer, mqttPort);      //设置MQTT服务器地址和端口
  client.setCallback(callback);                //绑定消息到达时的回调函数
}

//接收消息回调函数
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: "); 
  
  //原始字节流转换为字符串（简易实现）
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];      //逐个字节拼接字符串
  }
  Serial.println(message);            //显示接收消息回调的数据
  //检查开关指令
  if (message.indexOf("\"LightSwitch\":1") != -1) {
    digitalWrite(LED_PIN, HIGH);   //判断指令为1则执行开灯指令
    Serial.println("Successfully opened!");
  } 
  else if (message.indexOf("\"LightSwitch\":0") != -1) {
    digitalWrite(LED_PIN, LOW);    //判断指令为0则执行关灯指令
    Serial.println("Close successfully!");
  }
}

//MQTT重连服务函数
unsigned long lastReconnectAttempt = 0;

void nonBlockingReconnect() {
  if (!client.connected() && millis() - lastReconnectAttempt > 5000) {
    lastReconnectAttempt = millis();
    if (connectWithRetry()) {
      lastReconnectAttempt = 0;  //连接成功后执行初始化操作
    }
  }
}

bool connectWithRetry() {
  uint8_t retries = 3;
  while (retries--) {
    if (client.connect(clientId.c_str(), username.c_str(), passwordStr.c_str())) {
      return true;
    }
    delay(1000 * (3 - retries));   //指数退避延迟
  }
  return false;
}

void publishSensorData() {
  // 读取温湿度传感器数据
  float Humidity = dht.readHumidity();                                   //读取湿度（%）
  float Temperature = dht.readTemperature();                             //读取温度（摄氏度）
  float heatIndex = dht.computeHeatIndex(Temperature, Humidity, false);  //计算体感温度（热指数）
  
  //读取光敏模块数据
  int rawValue = analogRead(LIGHT_SENSOR_PIN);                           //读取光敏模块模拟值（0-1023）

  //显示文本
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("T: ");
  display.print(Temperature);      //显示温度
  display.println("C");

  display.setTextSize(1);
  display.setCursor(60,0);
  display.print("H: ");
  display.print(Humidity);         //显示湿度
  display.println("%");

  display.setTextSize(1);
  display.setCursor(0,12);
  display.print("L: ");
  display.print(rawValue);         //显示光敏模块模拟值

  display.setTextSize(1);
  display.setCursor(60,12);
  display.print("L: ");
  //光照等级判断
  String lightLevel;
  if (rawValue > DARK_THRESHOLD) { //显示光照等级
    lightLevel = "黑暗";
    display.print("Dark");      
  } else if (rawValue < BRIGHT_THRESHOLD) {
    lightLevel = "强光";
    display.print("Glaring");
  } else {
    lightLevel = "明亮";
    display.print("Bright");
  }

  display.setTextSize(1);
  display.setCursor(0,24);
  display.print("S: ");
  if(digitalRead(LED_PIN)==1){   //显示开关状态
    display.print("ON");
  }else{
    display.print("OFF");
  }

  display.display();             //刷新物理显示
  delay(500);                    //显示保持时间

  //数据包装
  DynamicJsonDocument doc(1024);                         //动态JSON文档
  doc["id"] = millis();                                  //消息ID
  doc["version"] = "1.0";                                //协议版本
  JsonObject params = doc.createNestedObject("params");  //阿里云物模型规范

  params["CurrentHumidity"] = Humidity;                  //湿度
  params["CurrentTemperature"] = Temperature;            //温度 
  params["CurrentheatIndex"] = heatIndex;                //体感温度
  params["CurrentrawValue"] = rawValue;                  //光照模拟量
  params["CurrentlightLevel"] = lightLevel;              //光照等级
  params["LightSwitch"] = digitalRead(LED_PIN);          //LED灯开光状态

  //数据序列化与发布
  char jsonBuffer[1024];                                 //JSON字符缓冲区
  serializeJson(doc, jsonBuffer);                        //序列化操作

  //阿里云标准主题格式：/sys/${productKey}/${deviceName}/thing/event/property/post
  String topic = "/sys/gmsc1t6K6WG/Seeed/thing/event/property/post";
  client.publish(topic.c_str(), jsonBuffer);             //MQTT发布
  Serial.println("Data published");                      //调试输出
}
