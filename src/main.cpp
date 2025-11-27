#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

using namespace std;

#define TAMANHO_MATRIZ 4

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Telegram BOT Token
#define BOT_TOKEN ""
// Seu chat id
#define CHAT_ID ""

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
String buffer;

// Seu IP local
char ipServer[] = "";
char botao[] = "SAFEWAY ativado por botão";
char request[] = "SAFEWAY ativado por localização";
int i = 0;

double rota[][TAMANHO_MATRIZ] = {
  {-47.78748035430909, -21.20823890345836},
  {-47.78599441051483, -21.20858397865198},
  {-47.78641283512116, -21.207608764067334},
  {-47.78758764266968, -21.20796884404864},
};

const int redLed = 13;
const int greenLed = 12;
const int sw1 = 2;
const int sw2 = 5;
const int buzzer = 4;
const int port = 8081;


WiFiClient client;

void setup() {
  // Inicialização serial
  Serial.begin(115200);
  Serial.println("Hello, ESP32-S2!");

  // Configurar cliente SSL como inseguro
  secured_client.setInsecure();

  // Conectar ao WiFi
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org");
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  if(client.connect(ipServer, port)){
    printf("Conectado ao servidor Express\n");
  }

  pinMode(sw1, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed,OUTPUT);
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
}

// som de ponto
void ponto() {
  digitalWrite(redLed, HIGH);
  tone(buzzer, 1000);
  delay(100);

  noTone(buzzer);
  digitalWrite(redLed, LOW);
}

// som de traço
void traco() {
  digitalWrite(redLed, HIGH);
  tone(buzzer, 1000);
  delay(300);

  noTone(buzzer);
  digitalWrite(redLed, LOW);
}

// função para que o buzzer toque um som como o SOS
void alarme() {
  digitalWrite(greenLed, LOW);
  for (int i = 0; i < 3; i++) {
    ponto();
    delay(1000);
  }

  for (int i = 0; i < 3; i++) {
    traco();
    delay(1000);
  }

  for (int i = 0; i < 3; i++) {
    ponto();
    delay(1000);
  }
  digitalWrite(greenLed, HIGH);
}


void ativarSafeway(char *message){
  alarme();
  bot.sendMessage(CHAT_ID, message, "");
}

String latLongToJson(double* coordinates){
  StaticJsonDocument<200> geoLatLong;
  geoLatLong["lon"] = coordinates[0];
  geoLatLong["lat"] = coordinates[1];
  
  String jsonString = ""; 
  serializeJson(geoLatLong, jsonString);
  return jsonString;
}

void loop() {

  
  if(client.available()){
    buffer = client.readString();
    buffer.trim();
  }

  if(i < TAMANHO_MATRIZ){
    String json = latLongToJson(rota[i]);
    
    Serial.printf("Enviando %s\n", json.c_str());
    
    client.write(json.c_str());
    
    i++;
  }
  
  if(buffer == "ATIVAR_SAFEWAY"){
    Serial.println("Requisição recebida! Enviando mensagem...");
    ativarSafeway(request);
  }
  
  if (digitalRead(sw1) == LOW && digitalRead(sw2) == LOW) {
    Serial.println("Botões pressionados! Enviando mensagem...");
    ativarSafeway(botao);
  }
  
  buffer = "";
  
  delay(2000);
}
