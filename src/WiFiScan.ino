/* DHT Pro Shield - Simple
 *
 * Depends on Adafruit DHT Arduino library
 * https://github.com/adafruit/DHT-sensor-library
 */
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define DHTPIN D4     // what pin we're connected to

// Uncomment whatever type you're using
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "...";
const char* password = "...";

const char* aio_username = "...";
const char* aio_server = "io.adafruit.com";
const char* aio_key = "...";
const char* CLIMATE_FEED = "path/to/feed";
const char* topic = "...";


const char* MQTT_SERVER  = aio_server;
const char* MQTT_USERNAME = aio_username;
const char* MQTT_KEY = aio_key;

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, 1883, MQTT_USERNAME, MQTT_KEY);

Adafruit_MQTT_Publish climate = Adafruit_MQTT_Publish(&mqtt, CLIMATE_FEED);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.println(topic);
  Serial.write(payload, length);
  Serial.println("");
}


void setup() {
    Serial.begin(9600);
    delay(1000);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void MQTT_connect() {
    int8_t ret;

    // Stop if already connected.
    if (mqtt.connected()) {
        return;
    }

    Serial.print("Connecting to MQTT... ");

    uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
            Serial.println(mqtt.connectErrorString(ret));
            Serial.println("Retrying MQTT connection in 5 seconds...");
            mqtt.disconnect();
            delay(5000);  // wait 5 seconds
            retries--;
            if (retries == 0) {
                // basically die and wait for WDT to reset me
                while (1);
            }
        }
        Serial.println("MQTT Connected!");
}

void loop() {
    // Wait 45 seconds between measurements to minimize the heat from the D1 mini interfering skewing the DHT sensor reading
    delay(45000);

    // Ensure the connection to the MQTT server is alive (this will make the first
    // connection and automatically reconnect when disconnected).  See the MQTT_connect
    // function definition further below.
    MQTT_connect();

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" percent \t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    Serial.print(F("\nAttempting to publish"));
    Serial.print("...");
    if (! climate.publish(f)) {
        Serial.println(F("Failed"));
    } else {
        Serial.println(F("OK!"));
    }

}
