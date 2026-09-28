#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <PMS.h>
#include <s8_uart.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

// --- CONFIGURACIÓN WIFI ---
const char* ssid = "MiFibra-81D2";
const char* password = "vFQ3pED2";

WebServer server(80);

// --- SENSORES ---
SoftwareSerial s8Serial(27, 26); 
S8_UART s8(s8Serial);
SoftwareSerial pmsSerial(17, 16); 
PMS pms(pmsSerial);
Adafruit_BME680 bme;

void handleRoot() {
  if (!bme.performReading()) return;
  int co2 = s8.get_co2();
  PMS::DATA data;
  bool azulOk = pms.readUntil(data, 2000);

  // --- LÓGICA DE TRADUCCIÓN (Business Logic) ---
  String co2Status = (co2 < 800) ? "<span style='color:green'>Excelente</span>" : (co2 < 1200) ? "<span style='color:orange'>Aceptable</span>" : "<span style='color:red'>VENTILA YA</span>";
  String pmStatus = (data.PM_AE_UG_2_5 < 12) ? "<span style='color:green'>Limpio</span>" : "<span style='color:red'>Alerta Polución</span>";
  float gasK = bme.gas_resistance / 1000.0;
  String gasStatus = (gasK > 50) ? "Limpio" : "Viciado/Químicos";

  // --- CONSTRUCCIÓN DE LA WEB ---
  String html = "<html><head><meta charset='UTF-8' http-equiv='refresh' content='10'>";
  html += "<title>Estación Calidad Aire</title>";
  html += "<style>body{font-family:sans-serif; background:#f0f2f5; padding:20px;} .card{background:white; border-radius:15px; padding:15px; margin-bottom:15px; box-shadow:0 4px 6px rgba(0,0,0,0.1);} h1{color:#1a73e8;} .val{font-size:1.8em; font-weight:bold;}</style></head><body>";
  
  html += "<h1>Informe de Calidad del Aire</h1>";

  // Tarjeta CO2
  html += "<div class='card'><h3>Sensor Sueco (CO2)</h3><div class='val'>" + String(co2) + " ppm</div><p>Estado: " + co2Status + "</p></div>";

  // Tarjeta Partículas (TODAS)
  html += "<div class='card'><h3>Sensor Azul (Partículas)</h3>";
  if(azulOk) {
    html += "<p><b>PM 1.0:</b> " + String(data.PM_AE_UG_1_0) + " ug/m3 (Finas)</p>";
    html += "<p class='val'><b>PM 2.5:</b> " + String(data.PM_AE_UG_2_5) + " ug/m3</p>";
    html += "<p><b>PM 10.0:</b> " + String(data.PM_AE_UG_10_0) + " ug/m3 (Polvo)</p>";
    html += "<p>Análisis: " + pmStatus + "</p>";
  } else { html += "<p>Esperando datos del sensor...</p>"; }
  html += "</div>";

  // Tarjeta Clima y Gases
  html += "<div class='card'><h3>Sensor Morado (Clima y COVs)</h3>";
  html += "<p><b>Temperatura:</b> " + String(bme.temperature, 1) + " °C</p>";
  html += "<p><b>Humedad:</b> " + String(bme.humidity, 1) + " %</p>";
  html += "<p><b>Gases (VOC):</b> " + String(gasK, 1) + " KOhms (" + gasStatus + ")</p></div>";

  html += "<p><i>La página se actualiza sola cada 10 segundos</i></p></body></html>";
  
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  s8Serial.begin(9600);
  pmsSerial.begin(9600);
  if (!bme.begin(0x77)) { bme.begin(0x76); }
  bme.setGasHeater(320, 150);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  Serial.println("\nWiFi Conectado!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}