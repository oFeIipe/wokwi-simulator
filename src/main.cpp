#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>

using namespace std;

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Telegram BOT Token
#define BOT_TOKEN "8293542075:AAEXyHq-2Bhrw5lYAtFWf6DlqqNWJA3p7D8"
#define CHAT_ID "8317542821"
#define TAMANHO_MATRIZ 5

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
TinyGPSPlus gps;

String buffer;
char ipServer[] = "192.168.0.78";
char alert[] = "Mensagem de teste";
char botao[] = "Botão pressionado";
char request[] = "Mensagem vinda de requisição do servidor";
int i = 0;

double rota[][TAMANHO_MATRIZ] = {
  {-21.208411441155967, -47.785964906215675},
  {-21.20765127355529, -47.786847352981574},
  {-21.208784021873214, -47.787220180034645},
  {-21.218760833638278, -47.79200792312623},
};

const int ledPin = 18;
const int buttonPin = 2;
const int buzzer = 4;
const int port = 8081;
int lastButtonState = HIGH;

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

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);
}

void ativarSafeway(char *message){
  tone(buzzer, 262, 3000);
  bot.sendMessage(CHAT_ID, message, "");
}

void printDistance(double distancia_entre_pontos){
  Serial.printf("Distância entre os dois pontos %f\n", distancia_entre_pontos);
  delay(2000);
}

String latLongToJson(double* coordinates){
  StaticJsonDocument<200> geoLatLong;
  geoLatLong["lat"] = coordinates[0];
  geoLatLong["lon"] = coordinates[1];
  
  String jsonString = ""; 
  serializeJson(geoLatLong, jsonString);
  return jsonString;
}

void loop() {
  int currentButtonState = digitalRead(buttonPin);
  
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
  
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("Botão pressionado! Enviando mensagem...");
    digitalWrite(ledPin, HIGH);
    ativarSafeway(botao);
  }
  
  lastButtonState = currentButtonState;
  buffer = "";
  
  delay(2000);
}