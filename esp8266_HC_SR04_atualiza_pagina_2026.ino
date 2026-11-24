

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// ==========================================
// CONFIGURAÇÃO WIFI
// ==========================================

#define AP_SSID "EACI_ESP8266"
#define AP_PASSWORD "senha123"

#define WEB_PORT 80

// ==========================================
// CONFIGURAÇÃO HC-SR04
// ==========================================

#define TRIGGER_PIN 5    // GPIO5 (D1)
#define ECHO_PIN 4       // GPIO4 (D2)

#define MAX_DISTANCE 200  // Distância máxima em mm

ESP8266WebServer server(WEB_PORT);

// ==========================================
// VARIÁVEIS GLOBAIS
// ==========================================

long distance = 0;
long duration = 0;

unsigned long previousMillis = 0;

const unsigned long interval = 1000; // 1 segundo

// ==========================================
// SETUP
// ==========================================

void setup() {

  Serial.begin(9600);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIGGER_PIN, LOW);

  // Criar ponto de acesso WiFi
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  IPAddress ip = WiFi.softAPIP();

  Serial.println();
  Serial.println("================================");
  Serial.println("       ESP8266 INICIADO");
  Serial.println("================================");

  Serial.print("Rede WiFi: ");
  Serial.println(AP_SSID);

  Serial.print("IP: ");
  Serial.println(ip);

  // Configurar servidor Web
  server.on("/", HTTP_GET, handleRequest);
  server.on("/update", HTTP_GET, handleUpdate);

  server.begin(WEB_PORT);

  Serial.println("Servidor Web iniciado!");
  Serial.println("A aguardar medicoes...");
  Serial.println("================================");

}

// ==========================================
// LOOP
// ==========================================

void loop() {

  // Atender pedidos da página Web
  server.handleClient();

  // Medir distância a cada segundo
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    distance = measureDistance();

    // Apresentar no Monitor Série
    Serial.print("Distancia: ");

    if (distance == 0) {

      Serial.println("Sem eco ou fora do alcance");

    } else {

      Serial.print(distance);
      Serial.println(" mm");
    }

  }

}

// ==========================================
// ATUALIZAR DISTÂNCIA NA PÁGINA WEB
// ==========================================

void handleUpdate() {

  // Devolver a última medição
  // Não efetua uma nova medição

  server.send(200, "text/plain", String(distance));

}

// ==========================================
// PÁGINA WEB
// ==========================================

void handleRequest() {

  String response = "<!DOCTYPE html>";
  response += "<html>";
  response += "<head>";

  response += "<meta charset='UTF-8'>";
  response += "<meta name='viewport' content='width=device-width, initial-scale=1'>";

  response += "<title>Medição de Distância - 2026</title>";

  // CSS
  response += "<style>";

  response += "body {";
  response += "text-align:center;";
  response += "font-family:Arial,sans-serif;";
  response += "background:#f2f2f2;";
  response += "margin:30px;";
  response += "}";

  response += "h1 {";
  response += "font-size:32px;";
  response += "color:#333;";
  response += "}";

  response += ".distance {";
  response += "font-size:40px;";
  response += "font-weight:bold;";
  response += "color:#222;";
  response += "}";

  response += ".progress {";
  response += "width:70%;";
  response += "height:40px;";
  response += "margin:20px auto 5px auto;";
  response += "border:2px solid #999;";
  response += "border-radius:8px;";
  response += "background:#ddd;";
  response += "overflow:hidden;";
  response += "}";

  response += ".progress-bar {";
  response += "width:0%;";
  response += "height:100%;";
  response += "background:green;";
  response += "transition:width 0.3s;";
  response += "}";

  response += ".scale {";
  response += "width:70%;";
  response += "margin:auto;";
  response += "display:flex;";
  response += "justify-content:space-between;";
  response += "font-size:14px;";
  response += "color:#444;";
  response += "}";

  response += "</style>";
  response += "</head>";

  // HTML
  response += "<body>";

  response += "<h1>Medição de Distância - 2026</h1>";

  response += "<p>Distância ao objeto:</p>";

  response += "<div class='distance'>";
  response += "<span id='distance'>--</span> mm";
  response += "</div>";

  // Barra de progresso
  response += "<div class='progress'>";
  response += "<div class='progress-bar' id='progress-bar'></div>";
  response += "</div>";

  // Escala de 0 a 20 cm
  response += "<div class='scale'>";
  response += "<span>0</span>";
  response += "<span>5</span>";
  response += "<span>10</span>";
  response += "<span>15</span>";
  response += "<span>20</span>";
  response += "</div>";

  // JavaScript
  response += "<script>";

  response += "setInterval(function() {";

  response += "var xhttp = new XMLHttpRequest();";

  response += "xhttp.onreadystatechange = function() {";

  response += "if (this.readyState == 4 && this.status == 200) {";

  response += "var distance = parseInt(this.responseText);";

  response += "var progressBar = document.getElementById('progress-bar');";

  // Calcular percentagem (0 a 200 mm)
  response += "var percent = Math.min(100, Math.max(0, (distance / 200) * 100));";

  response += "progressBar.style.width = percent + '%';";

  // Apresentar distância
  response += "document.getElementById('distance').innerHTML = distance;";

  response += "}";

  response += "};";

  response += "xhttp.open('GET', '/update', true);";

  response += "xhttp.send();";

  response += "}, 1000);";

  response += "</script>";

  response += "</body>";
  response += "</html>";

  server.send(200, "text/html", response);

}

// ==========================================
// MEDIR DISTÂNCIA
// ==========================================

long measureDistance() {

  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIGGER_PIN, LOW);

  // Timeout de 1000 microssegundos
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Sem eco
  if (duration == 0) {
    return 0;
  }

  // Converter para milímetros
  long measuredDistance = duration * 0.343 / 2;

  // Limitar a 200 mm
  if (measuredDistance > MAX_DISTANCE) {
    return 0;
  }

  return measuredDistance;

}

