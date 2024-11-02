#include <Adafruit_SSD1306.h>
#include <DHT_U.h>

#define OLED_I2C_ADDRESS 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define DHTPIN 4
#define DHTTYPE DHT11

const int pinTrigger=2;
const int pinEcho=3;
const int pinRed=5;
const int pinGreen=6;
const int pinBlue=7;

DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT);

void setup() {
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    while (true);
  }
  oled.clearDisplay();
  showHeader();
  oled.display();
  pinMode(pinTrigger, OUTPUT);
  pinMode(pinEcho, INPUT);
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  digitalWrite(pinTrigger, LOW);
  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrigger, LOW);
  float durata = pulseIn(pinEcho, HIGH);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float x = 1 / 58.31;
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    float vs = (331.45 + (0.62 * event.temperature)) * 0.0001;
    x = vs / 2;
    Serial.println("Velocita' Suono: " + String(vs) + " T: " + String(event.temperature) + " °C");
  }
  float distanza = durata * x;
  if (durata > 38000) {
    Serial.println("> 600 cm");
    showValue("> 600 cm");
    oled.display();
  } else {
    Serial.print("Distanza: ");
    Serial.print(distanza);
    Serial.println(" cm");
    showValue(String(distanza) + " cm");
    oled.display();
  }
  setLed(distanza);
  delay(500);
}

void showHeader() {
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(4, 4);
  oled.print("Distanza");
  oled.setCursor(112, 4);
  oled.drawLine(0, 15, 128, 15, WHITE);
}

void showValue(String value) {
  oled.fillRect(0, 16, 128, 48, BLACK);
  oled.setTextSize(2);
  oled.setCursor(OLED_WIDTH / 2, OLED_HEIGHT / 2 + 8);
  printCenteredText(value);
}

void printCenteredText(String text) {
  int16_t x = 0, y = 0;
  uint16_t w = 0, h = 0;
  int16_t cursorX = oled.getCursorX();
  int16_t cursorY = oled.getCursorY();
  oled.getTextBounds(text, 0, 0, &x, &y, &w, &h);
  oled.setCursor(cursorX - x - w / 2, cursorY - y - h / 2);
  oled.print(text);
}

void setLedColor(bool red, bool green, bool blue) {
  digitalWrite(pinRed, red);
  digitalWrite(pinGreen, green);
  digitalWrite(pinBlue, blue);
}

void setLed(float distanza) {
  if (distanza < 20) {
    setLedColor(HIGH, LOW, LOW);
  } else if (distanza >= 20 && distanza <= 30) {
    setLedColor(HIGH, HIGH, LOW);
  } else {
    setLedColor(LOW, HIGH, LOW);
  }
}