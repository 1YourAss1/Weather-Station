#include <DHT.h>
#include <ESP8266WiFi.h>

#define DHTPIN 5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Xiaomi_2.4G";
const char* password = "byzon123";
const char* host = "192.168.31.59";
const int port = 80;

const int period = 10; // Minutes

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Ждем инициализации последовательной коммуникации:
  while (!Serial) { }

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("WiFi connected");
  dht.begin();
}

void loop() {
  delay(2000);
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  // Провека полученных показателей
  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Ошиибка считывания");
    return;
  }

  Serial.print("connect to YourAss server ");
  Serial.println(host);

  // Используем WiFi клиент
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }

  // Создаем URL с запросом для сервера
  String url = "/api/weather?temp=";
  url += temp;
  url += "&humidity=";
  url += humidity;
  url = String("PUT ") + url + " HTTP/1.1\r\n" + "Host: " + host + ":" + port + "\r\n" + "Connection: close\r\n\r\n";

  client.print(url);
  Serial.println(url);
  delay(10);

  // ответ сервера
  while (client.available()) {
    String req = client.readStringUntil('\r');
    Serial.println(req);
  }

  Serial.println("Go to sleep");
  ESP.deepSleep(30e6);
}
