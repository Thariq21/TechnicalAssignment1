#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 33
#define TYPE DHT11

DHT dht(DHTPIN, TYPE);

const char* ssid = "Ariq";
const char* password = "012345678";
const char* serverURL = "http://192.168.23.116:5000/sensor/data"; // Ganti dengan IP lokal komputer Anda dan pastikan endpoint benar

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.println("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(0, OUTPUT);
  setup_wifi();
  dht.begin();
}

String readTemperatureHumidity() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  String msg = "";

  if (isnan(temperature) || isnan(humidity)) {
    return "Tidak ada data";
  } else {
    msg += "{\"temperature\": ";
    msg += String(temperature, 2); // Mengatur agar hanya dua desimal
    msg += ", \"kelembapan\": ";
    msg += String(humidity, 2); // Mengatur agar hanya dua desimal
    msg += "}";
  }
  return msg;
}

void publishMessage(const char* message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    Serial.print("Menghubungkan ke server: ");
    Serial.println(serverURL);
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(message);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Pesan terkirim ke server lokal");
      Serial.println(httpResponseCode);
      Serial.println(response);
      digitalWrite(0, LOW);
      digitalWrite(2, HIGH);
    } else {
      Serial.print("Error pada pengiriman: ");
      Serial.println(httpResponseCode);
      digitalWrite(2, LOW);
      digitalWrite(0, HIGH);
    }
    http.end();
  } else {
    Serial.println("Gagal mengirim pesan. WiFi tidak terhubung.");
    digitalWrite(2, LOW);
    digitalWrite(0, HIGH);
  }
  delay(2000);
  digitalWrite(2, LOW);
  digitalWrite(0, LOW);
}

void loop() {
  String msg = readTemperatureHumidity();
  Serial.print("Pesan yang dikirim: ");
  Serial.println(msg);
  publishMessage(msg.c_str());
  delay(5000); // Mengirimkan data setiap 5 detik
}
