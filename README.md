项目概述
# ESP32-S3 环境监测与远程控制系统
本项目是基于ESP32的多功能环境监测与控制系统，集成温湿度传感、光照检测、OLED显示和物联网通信功能。通过连接阿里云IoT平台，实现环境数据的实时上报和远程设备控制。系统每5秒上报传感器数据，并支持云端指令控制LED开关。

## 主要功能
- 🌡️ **温湿度监测**：DHT11传感器实时采集环境温湿度
- 🌞 **光照检测**：光敏电阻模块监测环境光照强度（模拟值+等级判断）
- 📺 **OLED显示**：SSD1306屏幕实时显示传感器数据及设备状态
- 🖲️ **远程控制**：支持通过MQTT指令控制LED开关（LightSwitch字段）
- ☁️ **云端连接**：数据上报至阿里云IoT平台（遵循物模型规范）
- 🔄 **自动重连**：具备WiFi/MQTT断线自动重连机制

## 硬件要求
| 组件               | 型号/参数          |
|--------------------|-------------------|
| 主控板             | ESP32-S3系列开发板 |
| 温湿度传感器       | DHT11             |
| 光敏模块           | 模拟输出型         |
| OLED显示屏         | SSD1306 128x32    |
| LED模块            | 普通LED（带限流电阻）|
| 连接线             | 杜邦线若干         |

一、硬件要求

1.开发板：Xiao ESP32 S3，体积小巧，易于集成到各种应用场景中。

获取链接：https://www.seeedstudio.com/catalogsearch/result/?q=ESP32S3

产品图示：![image](https://github.com/user-attachments/assets/84304c53-8858-4781-a13b-809273d7295f)

使用介绍：https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/

2.温湿度传感器：DHT11，连接到开发板的 D8 引脚。

获取链接：https://www.seeedstudio.com/Grove-Temperature-Humidity-Sensor-DHT11.html

产品图示：![image](https://github.com/user-attachments/assets/f1667fab-b430-4f7c-9507-c9ed5d544060)

使用介绍：https://wiki.seeedstudio.com/Grove-TemperatureAndHumidity_Sensor/

3. OLED显示屏：128x32 分辨率的 SSD1306 （若无可用以下型号代替），通过 I2C 接口连接。

获取链接：https://www.seeedstudio.com/Grove-OLED-Display-0-66-SSD1306-v1-0-p-5096.html

产品图示：![image](https://github.com/user-attachments/assets/7b949ebd-b958-4176-afdc-9205ac07011f)

使用介绍：https://wiki.seeedstudio.com/Grove-OLED-Display-0.66-SSD1306_v1.0/

4.接线：![image](https://github.com/user-attachments/assets/5ccba8f2-d976-4e13-b4ce-22e4c14b955d)

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

![image](https://github.com/user-attachments/assets/df4ff42c-b756-46ce-a017-b59a58905b51)

3.添加传感器的参数

![image](https://github.com/user-attachments/assets/46308bda-9bd7-4776-9875-cda666c2b773)

六、适用场景

·智能家居：实时监测室内温湿度，为智能家居系统提供数据支持，实现自动调节空调、加湿器等设备。

·农业大棚：监测大棚内的温湿度环境，帮助农民及时调整种植环境，提高农作物的产量和质量。

·工业环境：对工业生产环境中的温湿度进行监测，确保生产过程的稳定性和产品质量。

七、后续展望

后续可以考虑增加更多的传感器，如光照传感器、气压传感器等，采集更丰富的环境数据。同时，优化数据处理和展示方式，提供更个性化的数据分析服务，同时还可以基于 Ardroid Studio 设计一个简易的 APP 来实现设备的远程数据传输与控制。

八、注意事项

·要保证硬件连接无误，特别是 DHT11 传感器和 OLED 显示屏的引脚连接。

·若使用的是不同的开发板，可能需要调整引脚定义。

·要确保阿里云 IoT 平台的设备配置与代码中的配置一致。

九、最终效果

![image](https://github.com/user-attachments/assets/87d4e997-258c-42d8-94d4-e0cee2fc98f6)

![996eace5571f4a2670f595187a7b38f](https://github.com/user-attachments/assets/5d370783-b648-4e9e-a240-03f3637dc400)


