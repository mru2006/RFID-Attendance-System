#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MFRC522.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ===== Pins =====
#define SS_PIN D8
#define RST_PIN D3
#define PIR_PIN D2
#define LED_PIN D4
#define BUZZER_PIN D1

MFRC522 rfid(SS_PIN, RST_PIN);
ESP8266WebServer server(80);

// ===== WiFi =====
const char* ssid = "Redmi 13 5G";
const char* password = "mrunal2006@";

// ===== Google Apps Script URL =====
const String GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/AKfycbydcfoDnVMNtAh1HNNaANRMzuSHVktklKBWyUXEeDbOjqH5z4yyg4oPSaQVvP_8z4Wo/exec";

// ===== NTP =====
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);  // IST offset +5:30

// ===== Students =====
struct Student {
  String name;
  String uid;
  bool present;
  String lastScan;
};

Student students[] = {
  {"Mrunal", "53551A10", false, ""},
  {"Divya",  "5CCF964", false, ""},
  {"Diya",   "13AD85", false, ""}
};
const int studentCount = sizeof(students) / sizeof(students[0]);

// ===== Send to Google Sheet =====
void sendToGoogleSheet(String rfid, String name) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String postData = "rfid=" + rfid + "&name=" + name;

  if (http.begin(client, GOOGLE_SCRIPT_URL)) {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int code = http.POST(postData);
    if (code > 0) {
      Serial.printf("📤 Sent! Code:%d\n", code);
      Serial.println(http.getString());
    } else {
      Serial.printf("❌ Send error:%d\n", code);
    }
    http.end();
  } else {
    Serial.println("⚠️ Bad URL");
  }
}

// ===== Dashboard =====
String getHTML() {
  String html = "<html><head><meta http-equiv='refresh' content='5'><title>Attendance</title>"
                "<style>body{font-family:Arial;text-align:center;background:#f7f7f7}"
                "table{margin:auto;border-collapse:collapse;width:85%}"
                "th,td{border:1px solid #444;padding:10px}"
                "th{background:#222;color:#fff}"
                "td.present{background:#c8e6c9}"
                "td.absent{background:#ffcdd2}</style></head><body>";
  html += "<h2>ESP8266 RFID Attendance</h2><table><tr><th>Name</th><th>UID</th><th>Status</th><th>Last Scan</th></tr>";
  for (int i = 0; i < studentCount; i++) {
    html += "<tr><td>" + students[i].name + "</td><td>" + students[i].uid + "</td>";
    html += students[i].present ? "<td class='present'>Present</td>" : "<td class='absent'>Absent</td>";
    html += "<td>" + (students[i].lastScan == "" ? "-" : students[i].lastScan) + "</td></tr>";
  }
  html += "</table></body></html>";
  return html;
}

void handleRoot() { server.send(200, "text/html", getHTML()); }

// ===== Setup =====
void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  SPI.begin();
  rfid.PCD_Init();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n✅ WiFi connected: " + WiFi.localIP().toString());

  timeClient.begin();
  server.on("/", handleRoot);
  server.begin();
  Serial.println("📡 Server started!");
}

// ===== Loop =====
void loop() {
  server.handleClient();
  timeClient.update();

  if (digitalRead(PIR_PIN) == HIGH) {
    Serial.println("👀 Motion detected!");
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);

    unsigned long start = millis();
    while (millis() - start < 10000) {
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        String uid = "";
        for (byte i = 0; i < rfid.uid.size; i++) uid += String(rfid.uid.uidByte[i], HEX);
        uid.toUpperCase();

        Serial.println("🎫 UID: " + uid);
        bool known = false;

        for (int i = 0; i < studentCount; i++) {
          if (uid == students[i].uid) {
            students[i].present = true;

            time_t raw = timeClient.getEpochTime();
            struct tm* ti = localtime(&raw);
            char buf[11];
            strftime(buf, sizeof(buf), "%Y-%m-%d", ti);
            String today = String(buf);

            students[i].lastScan = today;
            sendToGoogleSheet(uid, students[i].name);
            Serial.println("✅ Marked present: " + students[i].name);
            known = true;
            break;
          }
        }

        if (!known) Serial.println("⚠️ Unknown card!");

        digitalWrite(BUZZER_PIN, HIGH);
        delay(300);
        digitalWrite(BUZZER_PIN, LOW);

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        break;
      }
    }
    Serial.println("🕐 Waiting for next motion...");
  }
}
