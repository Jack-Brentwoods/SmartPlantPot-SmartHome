#define BLYNK_TEMPLATE_ID "TMPL68wawUncQ"
#define BLYNK_TEMPLATE_NAME "haha"
#define BLYNK_AUTH_TOKEN "WxvBqt6oYUpUS3izyIh-5W_mKDc41mdu"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Định nghĩa Virtual Pins trên Blynk
#define VIRTUAL_TEMP V0
#define VIRTUAL_HUMID V1                      
#define VIRTUAL_MOIST V2                      
#define VIRTUAL_LED V3
#define VIRTUAL_PUMP V4

// Khai báo biến chứa data của các cảm biến
int temperature;
int humidity;
int moisture;

// WiFi & MQTT credentials
const char *ssid = "3003";
const char *pswrd = "lam010171";

char auth[] = "WxvBqt6oYUpUS3izyIh-5W_mKDc41mdu";
char wifi[] = "3003";
char psw[] = "lam010171";

// Chân kết nối thiết bị
#define LED_PIN 4
#define DHTPIN 5        // Chân GPIO kết nối DHT11
#define DHTTYPE DHT11   // Loại cảm biến DHT11
#define PUMP_PIN 18       // Chân GPIO cho động cơ
const int moisturePin = 34;  // Chân ADC kết nối với AO của cảm biến


void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pswrd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

// Nhận tín hiệu từ Button Blynk để điều khiển LED
BLYNK_WRITE(VIRTUAL_LED) {  
  int ledState = param.asInt(); // Nhận giá trị từ Button (0 hoặc 1)
  digitalWrite(LED_PIN, ledState);
  Serial.println("LED: " + String(ledState));
}


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  Blynk.begin(auth, wifi, psw);
  
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
}

void loop() {
  Blynk.run();
  
  // Đọc cảm biến độ ẩm đất (Analog)
  int soilMoistureValue = analogRead(moisturePin);
  moisture = map(soilMoistureValue, 4095, 0, 0, 100); 
  
  // Đọc nhiệt độ và độ ẩm từ DHT11
  temperature = dht.readTemperature(); // Đọc nhiệt độ (°C)
  humidity = dht.readHumidity();       // Đọc độ ẩm không khí (%)

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Moisture: ");
  Serial.println(moisture);

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(VIRTUAL_TEMP, temperature);
  Blynk.virtualWrite(VIRTUAL_HUMID, humidity);
  Blynk.virtualWrite(VIRTUAL_MOIST, moisture);

  if (moisture <= 10) {
    digitalWrite(PUMP_PIN, HIGH); // Bật bơm
    Serial.println("🟢 Bật máy bơm - Đất khô!");
    Blynk.virtualWrite(VIRTUAL_PUMP, 1);
  } 
  else if (moisture >= 30) {
    digitalWrite(PUMP_PIN, LOW); // Tắt bơm
    Serial.println("🔴 Tắt máy bơm - Đất đủ ẩm!");
    Blynk.virtualWrite(VIRTUAL_PUMP, 0);
  }

  delay(2000);
}
