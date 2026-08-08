# 🛡️ SAFEWAY — Firmware ESP32

> Firmware do microcontrolador ESP32 do projeto SAFEWAY, simulado no Wokwi. Responsável pela coleta de coordenadas, comunicação com o servidor e acionamento do alarme sonoro.

---

## 📋 Sobre

O SAFEWAY é um dispositivo de segurança portátil para crianças que se locomovem sozinhas até a escola. Este repositório contém o **firmware do ESP32**, que gerencia o alarme sonoro, o envio de coordenadas GPS ao servidor e as notificações via Telegram.

O servidor de geofencing está em: [oFeIipe/wokwi-server](https://github.com/oFeIipe/wokwi-server)

---

## ⚙️ Como funciona

O dispositivo possui duas formas de ativação:

**1. Geofencing (automático)**
O ESP32 envia coordenadas ao servidor via Socket TCP a cada 2 segundos. Se o servidor detectar que a criança saiu da área segura, envia `ATIVAR_SAFEWAY` de volta e o alarme é acionado automaticamente.

**2. Botão físico (manual)**
A criança pressiona o botão no dispositivo e o alarme é acionado imediatamente, independentemente da localização ou resposta do servidor.

Em ambos os casos, ao ser ativado, o dispositivo:
- Aciona o buzzer com o código **SOS em morse**
- Envia uma mensagem de alerta ao tutor via **bot do Telegram**

```
┌─────────────────────────────────────────┐
│              Loop ESP32                 │
│                                         │
│  Envia coordenadas → Servidor (2s)      │
│         │                               │
│  ┌──────┴──────┐   ┌─────────────┐     │
│  │ Recebe      │   │ Botão       │     │
│  │ ATIVAR_SW   │   │ Pressionado │     │
│  └──────┬──────┘   └──────┬──────┘     │
│         └────────┬─────────┘            │
│              ativarSafeway()            │
│         ┌────────┴─────────┐            │
│      alarme()         Telegram          │
│     (SOS morse)      (alerta)           │
└─────────────────────────────────────────┘
```

---

## 🔧 Hardware simulado (Wokwi)

| Componente | Função |
|---|---|
| ESP32 | Microcontrolador principal |
| Buzzer | Alarme sonoro (SOS em morse) |
| Botão | Ativação manual pela criança |
| LED verde | Indicador de status do sistema |

> Simulação disponível em: [wokwi.com/projects/448006743777900545](https://wokwi.com/projects/448006743777900545)

---

## 🚀 Como rodar

### Pré-requisitos

- Conta no [Wokwi](https://wokwi.com) para simulação
- Servidor Node.js rodando ([wokwi-server](https://github.com/oFeIipe/wokwi-server))
- Bot do Telegram configurado

### Configuração

Antes de rodar, substitua no código:

```cpp
#define WIFI_SSID "Wokwi-GUEST"       // SSID da rede
#define WIFI_PASSWORD ""               // Senha da rede

#define BOT_TOKEN "SEU_TOKEN_AQUI"    // Token do bot Telegram
#define CHAT_ID "SEU_CHAT_ID_AQUI"    // Chat ID do tutor

char ipServer[] = "IP_DO_SERVIDOR";   // IP do servidor Node.js
```

### Criando o bot do Telegram

1. Acesse o [@BotFather](https://t.me/BotFather) no Telegram
2. Crie um novo bot com `/newbot`
3. Copie o token gerado e substitua em `BOT_TOKEN`
4. Envie uma mensagem para o bot e acesse `https://api.telegram.org/bot<TOKEN>/getUpdates` para obter o `CHAT_ID`

---

## 📡 Comunicação com o servidor

O ESP32 armazena uma rota pré-definida como matriz de coordenadas e as envia ao servidor em formato JSON via Socket TCP:

```cpp
double rota[][TAMANHO_MATRIZ] = {
    {-47.78748035430909, -21.20823890345836},
    {-47.78599441051483, -21.20858397865198},
    // ...
};
```

O microcontrolador fica escutando o servidor continuamente. Ao receber `ATIVAR_SAFEWAY`, aciona imediatamente o alarme:

```cpp
void ativarSafeway(char *message) {
    alarme();
    bot.sendMessage(CHAT_ID, message, "");
}
```

---

## 🛠️ Tecnologias

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white)

**Bibliotecas:** `WiFi.h` · `WiFiClientSecure.h` · `UniversalTelegramBot.h` · `ArduinoJson.h`

---

## 🔗 Repositórios do projeto

| Repositório | Descrição |
|---|---|
| [wokwi-simulator](https://github.com/oFeIipe/wokwi-simulator) | Este repositório — firmware ESP32 |
| [wokwi-server](https://github.com/oFeIipe/wokwi-server) | Servidor Node.js com geofencing |

---

## 👥 Equipe

Projeto extensionista desenvolvido por alunos do curso de Ciência da Computação — Centro Universitário Estácio de Ribeirão Preto.

| Membro | Contribuição |
|---|---|
| Cristian Alves de Sousa | Protótipo Wokwi · API Telegram · Relatório |
| Diogo Onofre Junior | Identidade visual · Formulário · Entrevista |
| Felipe Costa de Carvalho | Protótipo Wokwi · API Telegram · Rastreamento GPS |
| Vinicius de Paulo Costa | Rastreamento GPS · Formulário · Entrevista |

---

<div align="center">
  <sub>Projeto SAFEWAY — Segurança a caminho da escola · Estácio Ribeirão Preto · 2025</sub>
</div>
