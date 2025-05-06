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

## 开发板介绍
Xiao-ESP32-S3，体积小巧，易于集成到各种应用场景中。

获取链接：https://www.seeedstudio.com/catalogsearch/result/?q=ESP32S3

产品图示：![image](https://github.com/user-attachments/assets/84304c53-8858-4781-a13b-809273d7295f)

使用介绍：https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/

## 软件依赖
- [DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)
- WiFi.h 提供Wi-Fi通信功能(内置库)
- Wire.h 提供 I2C 通信功能(内置库)。

安装方法：通过Arduino IDE库管理器搜索安装

## 功能详解
1.环境监测模块
- 温湿度采集：通过DHT11传感器获取温度（摄氏度）和湿度（百分比）
- 体感温度计算：使用DHT库内置算法计算热指数
- 光照强度检测：
- 采集0-1023的模拟值
- 通过阈值判断实现三级光照等级：
  - 黑暗（>4000）
  - 明亮（1000-4000）
  - 强光（<1000）

2.显示系统
- OLED显示屏实时显示：
- 当前温度（T: xx.xC）
- 当前湿度（H: xx%）
- 光照强度值（L: xxxx）
- 光照等级（Dark/Bright/Glaring）
- LED状态（ON/OFF）

3.网络通信
- WiFi连接：自动连接指定SSID的WiFi网络
- MQTT通信：
- 云端：阿里云IoT平台
  - 发布主题：/sys/gmsc1t6K6WG/Seeed/thing/event/property/post
  - 数据格式：符合阿里云物模型规范的JSON数据包

4.远程控制
支持通过MQTT消息实现LED远程控制：
  - 开灯指令：包含"LightSwitch":1的JSON消息
  - 关灯指令：包含"LightSwitch":0的JSON消息

## 代码结构说明
1.库文件

![image](https://github.com/user-attachments/assets/4d742ee1-c9b2-44cc-925f-a6531fd9048c)

2.定义&配置

![image](https://github.com/user-attachments/assets/e04a0167-3fff-42fa-aa0d-7f5d0b229ee6)

3.setup()函数

![image](https://github.com/user-attachments/assets/23edfaf9-cf63-428e-8b0e-baf8d5fbdfaf)

4.loop()函数

![image](https://github.com/user-attachments/assets/bb515dd1-8b78-4f22-b85e-c3a6443fc62a)

5.WiFi连接函数

![image](https://github.com/user-attachments/assets/1c62b268-b7bb-4b39-9abf-bc78d9df6696)

6.MQTT连接函数

![image](https://github.com/user-attachments/assets/b13b96d1-1ad3-404b-9948-cfb03a68640f)

7.接收消息回调函数

![image](https://github.com/user-attachments/assets/a7cd64e7-4d5d-4aec-9da7-8528528c3fb5)

8.MQTT重连服务函数

![image](https://github.com/user-attachments/assets/4018f955-1875-4154-b4d2-08901755ad81)

9.数据采集&上报函数

![image](https://github.com/user-attachments/assets/ac3b8bfb-22a3-4386-906a-8007cda1a4ef)

## 数据采集与处理
采样频率：每5秒自动上传一次传感器数据

数据内容：
{
 
  "id": 123456789,
  
  "version": "1.0",
  
  "params": {
  
    "CurrentHumidity": 45.0,
    
    "CurrentTemperature": 25.0,
    
    "CurrentheatIndex": 26.5,
    
    "CurrentrawValue": 3000,
    
    "CurrentlightLevel": "明亮",
    
    "LightSwitch": 1
    
  }
  
}

## 适用场景
1.智能家居：实时监测室内温湿度，为智能家居系统提供数据支持，实现自动调节空调、加湿器等设备。

2.农业大棚：监测大棚内的温湿度环境，帮助农民及时调整种植环境，提高农作物的产量和质量。

3.工业环境：对工业生产环境中的温湿度进行监测，确保生产过程的稳定性和产品质量。

## 后续展望
后续可以考虑增加更多的传感器，如光照传感器、气压传感器等，采集更丰富的环境数据。同时，优化数据处理和展示方式，提供更个性化的数据分析服务，同时还可以基于 Ardroid Studio 设计一个简易的 APP 来实现设备的远程数据传输与控制。

## 最终效果

![image](https://github.com/user-attachments/assets/aa5f8339-4855-4725-925f-4b3588867850)

![c3aa19b6a91fd22376228ad3bc10d80](https://github.com/user-attachments/assets/ff8038c2-7a15-466f-a39a-52ad69189888)

https://github.com/user-attachments/assets/91b46f3c-7751-4cf4-8a8b-b43003999870
