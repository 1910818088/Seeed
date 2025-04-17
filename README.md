项目概述
# ESP32-S3 智能环境监测系统
此项目利用 Arduino IDE 开发环境，具体使用 DHT11 温湿度传感器采集温湿度数据，通过 OLED 显示屏实时展示数据，同时利用 WiFi 连接网络，经 MQTT 协议将数据上传至阿里云 IoT 平台。
![image](https://github.com/user-attachments/assets/2147c8ee-82c6-465c-915a-eda8201e4c69)

一、硬件要求

1.开发板：Xiao ESP32 S3，体积小巧，易于集成到各种应用场景中。

获取链接：https://www.seeedstudio.com/catalogsearch/result/?q=ESP32S3

产品图示：![image](https://github.com/user-attachments/assets/71429a45-5f00-4107-9c78-bbb89a7747cc)

使用介绍：https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/

2.温湿度传感器：DHT11，连接到开发板的 D8 引脚。

获取链接：https://www.seeedstudio.com/Grove-Temperature-Humidity-Sensor-DHT11.html

产品图示：![image](https://github.com/user-attachments/assets/54fb0083-8472-4ef7-a465-7c5f4b250530)


使用介绍：https://wiki.seeedstudio.com/Grove-TemperatureAndHumidity_Sensor/

3. OLED显示屏：128x32 分辨率的 SSD1306 （若无可用以下型号代替），通过 I2C 接口连接。

获取链接：https://www.seeedstudio.com/Grove-OLED-Display-0-66-SSD1306-v1-0-p-5096.html

产品图示：![image](https://github.com/user-attachments/assets/ea48e5d8-9971-4638-b3b3-ef3133eb34be)

使用介绍：https://wiki.seeedstudio.com/Grove-OLED-Display-0.66-SSD1306_v1.0/

4.接线：![image](https://github.com/user-attachments/assets/ee1a4201-64f2-4937-b5dd-f127a3408875)

5.其他：笔记本一台、Arduino IDE软件、USB-TypeC数据线一根、面包板1个、线材若干。

二、软件依赖

代码中使用了多个库，你需在 Arduino IDE 中安装这些库：

①DHT：用于读取 DHT11 传感器的数据。

②Wire：提供 I2C 通信功能。

③WiFi：支持开发板连接 WiFi 网络。

④ArduinoJson：用于处理 JSON 数据。

⑤PubSubClient：实现 MQTT 协议通信。

⑥Adafruit_GFX 和 Adafruit_SSD1306：用于控制 OLED 显示屏。

三、配置步骤

1.WiFi 配置：

在代码里找到 ssid 和 password 变量，把它们修改成你的 WiFi 网络名称和密码。

const char* ssid = "你的WiFi名称";

const char* password = "你的WiFi密码";

2.阿里云 IoT 配置：

要在阿里云 IoT 平台创建产品和设备，获取对应的 PRODUCT_KEY、DEVICE_NAME 和 DEVICE_SECRET，并更新代码中的相关变量。

#define PRODUCT_KEY "你的产品密钥"

#define DEVICE_NAME "你的设备名称"

#define DEVICE_SECRET "你的设备密钥"

3.MQTT 配置：

依据阿里云 IoT 平台的设置，更新 mqttServer、clientId、username 和 passwordStr。

const char* mqttServer = "你的MQTT服务器地址";

String clientId = "你的客户端ID";

String username = "你的用户名";

String passwordStr = "你的MQTT密码";

四、函数功能

1.setup() 函数：

①初始化串口通信、DHT11 传感器和 OLED 显示屏。

②显示初始信息。

③连接 WiFi 网络和阿里云 MQTT 服务器。

2.loop() 函数：

①检查 MQTT 连接状况，若断开则尝试重新连接。

②每 5 秒读取一次 DHT11 传感器的数据，将数据显示在 OLED 上，并通过 MQTT 协议

发布到阿里云 IoT 平台。

3.connectWiFi() 函数：

①尝试连接到指定的 WiFi 网络。

4.connectAliyunMQTT() 函数：

①配置 MQTT 服务器和回调函数。

5.callback() 函数：

①处理接收到的 MQTT 消息。

6.reconnect() 函数：

①尝试重新连接到 MQTT 服务器。

7.publishSensorData() 函数：

①读取 DHT11 传感器的温湿度数据。

②在 OLED 显示屏上显示温湿度数据。

③把数据封装成 JSON 格式，通过 MQTT 协议发布到阿里云 IoT 平台。

五、云平台配置

1.云平台链接：https://iot.console.aliyun.com/product

2.创建产品和添加设备获取配置信息

![image](https://github.com/user-attachments/assets/b6b369ac-28bf-4cab-9d62-2240c57e86dd)

3.添加传感器的参数

![image](https://github.com/user-attachments/assets/6ad41cd3-9e86-41ba-bf93-4e396699c386)

六、适用场景

智能家居：实时监测室内温湿度，为智能家居系统提供数据支持，实现自动调节空调、加湿器等设备。

农业大棚：监测大棚内的温湿度环境，帮助农民及时调整种植环境，提高农作物的产量和质量。

工业环境：对工业生产环境中的温湿度进行监测，确保生产过程的稳定性和产品质量。

七、后续展望

后续可以考虑增加更多的传感器，如光照传感器、气压传感器等，采集更丰富的环境数据。同时，优化数据处理和展示方式，提供更个性化的数据分析服务，同时还可以基于 Ardroid Studio 设计一个简易的 APP 来实现设备的远程数据传输与控制。

八、注意事项

要保证硬件连接无误，特别是 DHT11 传感器和 OLED 显示屏的引脚连接。

若使用的是不同的开发板，可能需要调整引脚定义。

要确保阿里云 IoT 平台的设备配置与代码中的配置一致。
