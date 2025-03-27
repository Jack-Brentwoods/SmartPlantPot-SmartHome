// Định nghĩa ID và NAME của Template Blynk, kèm theo đó là code authorization
#define BLYNK_TEMPLATE_ID "TMPL6Vy3pkU65"
#define BLYNK_TEMPLATE_NAME "home"
#define BLYNK_AUTH_TOKEN "jYwfNKG-_uMf_G9NCfVjlvy-kLAa-8JF"

// Khai báo các thư viện cần thiết
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// Định nghĩa Virtual Pins trên Blynk
#define VIRTUAL_TEMP V0
#define VIRTUAL_HUMID V1                      
#define VIRTUAL_GAS V2                      
#define VIRTUAL_FLAME V3
#define VIRTUAL_LASER V4
#define VIRTUAL_DOOR V5

// Khai báo biến chứa data của các cảm biến
int temperature;
int humidity;
int gas;
int flame;
int laser;
int door;

// Thông tin Wifi
const char *ssid = "3003";
const char *pswrd = "lam010171";

// Thông tin wifi và auth để kết nối với Blynk
char auth[] = "jYwfNKG-_uMf_G9NCfVjlvy-kLAa-8JF";
char wifi[] = "3003";
char psw[] = "lam010171";


// Chân kết nối thiết bị
#define SS_PIN 5   // Chân SDA của RC522
#define RST_PIN 16 // Chân RST của RC522

#define BUZZER_PIN 12 // Chân kết nối Buzzer


MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C

// Mã UID hợp lệ (UID2: 13 A2 1A 2D)
byte validUID[4] = {0x13, 0xA2, 0x1A, 0x2D};  

// Hàm kết nối Wifi
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


void setup() {
  Serial.begin(115200);
  setup_wifi();
  Blynk.begin(auth, wifi, psw);
  
  SPI.begin();
  mfrc522.PCD_Init();

  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Scanning RFID...");

  // Cấu hình Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Scanning RFID...");
}

void beep(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

bool compareUID(byte *cardUID, byte *targetUID) {
  for (byte i = 0; i < 4; i++) {
    if (cardUID[i] != targetUID[i]) return false;
  }
  return true;
}


void loop() {
  Blynk.run(); // Chạy Blynk liên tục
  readSensors(); // Đọc dữ liệu cảm biến liên tục
  checkRFID();   // Kiểm tra thẻ RFID khi có thẻ
}

void readSensors() {
  temperature = random(1, 100);
  humidity = random(1, 100);
  gas = random(0, 1);
  flame = random(0, 1);
  laser = random(0, 1);

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Gas: ");
  Serial.println(gas);
  Serial.print("Flame: ");
  Serial.println(flame);
  Serial.print("Laser: ");
  Serial.println(laser);
  Serial.print("Door: ");
  Serial.println(door);

  Blynk.virtualWrite(VIRTUAL_TEMP, temperature);
  Blynk.virtualWrite(VIRTUAL_HUMID, humidity);
  Blynk.virtualWrite(VIRTUAL_GAS, gas);
  Blynk.virtualWrite(VIRTUAL_FLAME, flame);
  Blynk.virtualWrite(VIRTUAL_LASER, laser);
  Blynk.virtualWrite(VIRTUAL_DOOR, door);

  delay(2000); // Chờ 2 giây để tránh spam dữ liệu
}

void checkRFID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID: ");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID: ");

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    lcd.print(mfrc522.uid.uidByte[i], HEX);
    lcd.print(" ");
  }
  Serial.println();

  if (compareUID(mfrc522.uid.uidByte, validUID)) {
    Serial.println("Access Granted");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    beep(200);
    door = 1;
  } else {
    Serial.println("Access Denied");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    beep(500);
    door = 0;
  }

  mfrc522.PICC_HaltA();
}
