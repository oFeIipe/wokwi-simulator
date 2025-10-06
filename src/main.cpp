#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Wifi network station credentials
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Telegram BOT Token
#define BOT_TOKEN "CHAVE_API"
#define CHAT_ID "SEU_ID"

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

String buffer;

char ipServer[] = SEU_IP_PRIVADO;

char alert[] = "Mensagem de teste";
char botao[] = "Botão pressionado";
char request[] = "Mensagem vinda de requisição do servidor";


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
}

void ativarSafeway(char *message){
  tone(buzzer, 262, 3000); 
  bot.sendMessage(CHAT_ID, message, "");
}

void loop() {
  int currentButtonState = digitalRead(buttonPin);

  if(client.available()){
    buffer = client.readString();
    buffer.trim();
  }

  if(buffer == "ATIVAR_SAFEWAY"){
    Serial.println("Requisição recebida! Enviando mensagem...");
    ativarSafeway(request);
  }

 if (lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("Botão pressionado! Enviando mensagem...");
    ativarSafeway(botao);
  }
  lastButtonState = currentButtonState;

  buffer = "";
  delay(10);
}
