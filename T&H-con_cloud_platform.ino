#include <DHT.h>
#include <Wire.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 温湿度传感器配置
#define DHTPIN  D8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//OLED配置
#define SCREEN_WIDTH 128  // OLED显示宽度
#define SCREEN_HEIGHT 32  // OLED显示高度
#define OLED_RESET     -1  // 重置引脚（-1表示共享Arduino复位引脚）

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi配置
const char* ssid = "luoyi";
const char* password = "88888888";

//阿里云IoT配置
#define PRODUCT_KEY "gmscK9MZk5R"
#define DEVICE_NAME "XIAO-ESP32-S3"
#define DEVICE_SECRET "3f8a4371fbb42c85ef883cece78c5516"

// MQTT配置
const char* mqttServer = "iot-06z00fvge4b2wv4.mqtt.iothub.aliyuncs.com";
const int mqttPort = 1883;
String clientId = "gmscK9MZk5R.XIAO-ESP32-S3|securemode=2,signmethod=hmacsha256,timestamp=1744802975573|";
String username = "XIAO-ESP32-S3&gmscK9MZk5R";
String passwordStr = "2ba71bb4f00fd8798e943dd63c87953e25cc5853965570a9040ef6335c327e63";  // 直接使用设备密钥

WiFiClient espClient;  // 使用普通WiFiClient
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  dht.begin();
  // 初始化OLED（I2C地址通常为0x3C或0x3D）
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306分配失败"));
    for(;;); // 死循环，阻止程序继续执行
  }
  // 清空屏幕缓冲区
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);   // 白色文本
  display.setTextSize(0.5);             // 切换到大字体
  display.setCursor(0,10);            // 调整位置
  display.println("Seeed Studio");
  
  display.display();  // 将缓冲区内容输出到屏幕
  delay(2000);

  connectWiFi();
  connectAliyunMQTT();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  // 显示动态数据示例
  display.clearDisplay();

  // 示例：每2分钟发送一次数据
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    DHT dht(DHTPIN, DHTTYPE);
    publishSensorData();
  }
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void connectAliyunMQTT() {
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId.c_str(), username.c_str(), passwordStr.c_str())) {
      Serial.println("connected!");
      client.subscribe("/gmscK9MZk5R/XIAO-ESP32-S3/thing/service/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishSensorData() {

  
  float h = dht.readHumidity();//读湿度
  float t = dht.readTemperature();//读温度(摄氏度)

// 显示文本
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temp: ");
  display.print(t);
  display.println(" C");

  display.setTextSize(0.5);
  display.setCursor(0,20);
  display.print("Hum:");
  display.print(h);
  display.println(" %");
  display.display();
  delay(500);

  DynamicJsonDocument doc(1024);
  doc["id"] = millis();
  doc["version"] = "1.0";
  JsonObject params = doc.createNestedObject("params");

  params["CurrentTemperature"] = t;
  params["CurrentHumidity"] = h;

  char jsonBuffer[1024];
  serializeJson(doc, jsonBuffer);

  String topic = "/sys/gmscK9MZk5R/XIAO-ESP32-S3/thing/event/property/post";
  client.publish(topic.c_str(), jsonBuffer);
  Serial.println("Data published");
}