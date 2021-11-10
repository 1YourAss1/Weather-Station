#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#include "index.h"

#define reset_btn 4

ESP8266WebServer server(80);
int timer = 10; // 10 minutes sleep by default

String essid, epass, etimer, ehost;
String st;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);

  Serial.println("\n\nStarting Weather Station by YourAss\n");
  //===============================================================
  // Read EEPROM for ssid/pass and host
  //===============================================================
  // SSID
  Serial.print("Reading EEPROM");
  essid = "";
  for (int i = 0; i < 32; ++i) {
    essid += char(EEPROM.read(i));
  }
  essid = essid.c_str();
  Serial.print(".");
  // Password
  epass = "";
  for (int i = 32; i < 96; ++i) {
    epass += char(EEPROM.read(i));
  }
  epass = epass.c_str();
  Serial.print(".");
  // Timer
  etimer = "";
  for (int i = 96; i < 112; ++i) {
    etimer += char(EEPROM.read(i));
  }
  etimer = etimer.c_str();
  timer = etimer.toInt();
  Serial.print(".");
  // Host
  ehost = "";
  for (int i = 112; i < 144; ++i) {
    ehost += char(EEPROM.read(i));
  }
  ehost = ehost.c_str();
  Serial.println(".");
  //===============================================================
  // Read EEPROM end
  //===============================================================


  //===============================================================
  // Check button state (if pressed, then start AP mode else send
  // data to host )
  //===============================================================
  pinMode(reset_btn, INPUT_PULLUP);
  if (digitalRead(reset_btn) == LOW)  {
    startAP();
  } else {
    if (essid.length() > 1 && ehost.length() > 1) {
      WiFi.begin(essid, epass);
      if (testWifi(essid)) {
        int c = 0;
        while (c < 5) {
          if (sendDataToHost(ehost)) return;
          delay(500);
          c++;
        }
        Serial.println("Go to deep sleep. Bye bye!");
        ESP.deepSleep(30e6);
      }
    } else Serial.println ("ERROR: SSID or host not defined! Restart in AP mode to define settings.\n");
  }
  //===============================================================
  // Check end
  //===============================================================
}

bool sendDataToHost(String ehost) {
  String host = ehost.substring(0, ehost.indexOf(":"));
  int port = ehost.substring(ehost.indexOf(":") + 1, ehost.length()).toInt();
  Serial.println("WiFi connected");
  DHT dht(5, DHT22); // pin and type
  dht.begin();
  delay(2000);
  // Get data from sensor
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  // Check received data
  if (isnan(temp) || isnan(humidity)) {
    Serial.println("ERROR: Sensor error!");
    return false;
  }

  Serial.println("Connect to server: " + ehost);
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("ERROR: Connection failed!");
    return false;
  }

  // Create URL
  String url = "/weather?temp=";
  url += temp;
  url += "&humidity=";
  url += humidity;
  url = String("PUT ") + url + " HTTP/1.1\r\n" + "Host: " + host + ":" + port + "\r\n" + "Connection: close\r\n\r\n";
  // Send URL to host
  client.print(url);
  Serial.println(url);
  delay(100);

  // Server response
  while (client.available()) {
    String req = client.readStringUntil('\r');
    Serial.println(req);
  }
  return true;
}



void startAP() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.print("\nScan networks done: ");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.println(n + " networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");

  st = "";
  for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
    // <option selected class="pswd" value="SSID1">Wifi1 &#128274;</option>
    st += "<option";
    st += (WiFi.SSID(i) == essid) ? " selected " : "";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "" : " class=\"pswd\" ";
    st += "value=\"" + WiFi.SSID(i) + "\">";
    st += WiFi.SSID(i);
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "" : " &#128274;";
    st += "</option>";
  }
  delay(100);

  WiFi.softAPConfig(IPAddress(192, 168, 26, 88), IPAddress (192, 168, 26, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP("Weather_Station");
  delay(100);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/wifi_save", HTTP_POST, handleSaveWifi);
  server.on("/reboot", HTTP_GET, handleReboot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("\nHTTP server started at 192.168.26.88");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String content = FPSTR(MAIN_page1);
  // Insert list of wifi devices
  content += st;
  content += FPSTR(MAIN_page2);
  // Insert timer list
  content += "<option " + String( (timer == 1) ? "selected " : "" ) + "value=\"1\">1 min</option>";
  content += "<option " + String( (timer == 5) ? "selected " : "") + "value=\"5\">5 min</option>";
  content += "<option " + String( (timer == 10) ? "selected " : "") + "value=\"10\">10 min</option>";
  content += FPSTR(MAIN_page3);
  // Insert host value
  content += " value = \"" + ehost + "\" ";
  content += FPSTR(MAIN_page4);
  server.send(200, "text/html", content);
}

void handleSaveWifi() {
  if (!server.hasArg("ssid") || !server.hasArg("timer") || !server.hasArg("address")
      || server.arg("ssid") == NULL || server.arg("timer") == NULL || server.arg("address") == NULL) {
    server.send(400, "text/plain", "400: Invalid Request");
  } else {
    // Clear EEPROM
    Serial.println("\nUpdating EEPROM");
    for (int i = 0; i < 144; ++i) {
      EEPROM.write(i, 0);
    }
    // Get arguments from request
    String nssid = server.arg("ssid");
    String npass = server.arg("pass");
    String ntimer = server.arg("timer");
    String nhost = server.arg("address");
    //===============================================================
    // Write ssid/pass and host to EEPROM
    //===============================================================
    for (int i = 0; i < nssid.length(); ++i) EEPROM.write(i, nssid[i]);
    for (int i = 0; i < npass.length(); ++i) EEPROM.write(32 + i, npass[i]);
    for (int i = 0; i < nhost.length(); ++i) EEPROM.write(96 + i, ntimer[i]);
    for (int i = 0; i < nhost.length(); ++i) EEPROM.write(112 + i, nhost[i]);
    EEPROM.commit();
    //===============================================================
    // Write EEPROM end
    //===============================================================
    Serial.println("\nUpdate done. Need to reboot");
  }
  // Send wifi_save page
  String content = FPSTR(SAVE_page);
  server.send(200, "text/html", content);
  //  server.send(200, "text/plain", "The data was saved successfully. Reboot station.");
  //  delay(2000);
  // ESP.restart();
}

void handleReboot() {
  delay(2000);
  ESP.restart();
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

void eraseEEPROM() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  if (digitalRead(reset_btn) == LOW)  {
    Serial.print("\n\nReseting EEPROG");
    for (int i = 0; i < 512; i++) {
      digitalWrite(LED_BUILTIN, LOW);
      EEPROM.write(i, 0);
      if (i % 100 == 0) Serial.print(".");
      digitalWrite(LED_BUILTIN, HIGH);
    }
    Serial.println("done!\n");
    EEPROM.end();
  }
}

bool testWifi(String essid) {
  int c = 0;
  Serial.print("\nTesting connection to " + essid);
  while (c < 20) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Success!\n");
      return true;
    }
    Serial.print(".");
    delay(500);
    c++;
  }
  Serial.println("\nConnect timed out, opening AP\n");
  return false;
}
