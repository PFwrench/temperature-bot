# Temperature-bot
This firmware allows an ESP8266 to publish to a MQTT server using [PlatformIO] (platformio.org). The goal of this project was to have ESP8266's set up around the building and get a more accurate idea of the temperature in the building than just what the thermostat read.


###Setup
1. We used a used the [WeMos D1 mini] (http://www.wemos.cc/Products/d1_mini.html) with the compatable [DHT Pro Shield] (http://www.wemos.cc/Products/dht_pro_shield.html).
2. Solder the shield onto the D1 mini.
3. Download [PlatformIO] (platformio.org) and start a new project, selecting the proper board and directory.
4. Install proper libraries as listed below.

### Libraries
- [Adafruit MQTT] (https://github.com/adafruit/Adafruit_MQTT_Library), Platformio ID#1092
- [Adafruit DHT] (https://github.com/adafruit/DHT-sensor-library), Platformio ID#19
- [Adafruit Unified Sensor] (https://github.com/adafruit/Adafruit_Sensor), Platformio ID#31
