// Chamando as bibliotecas
#include <WiFi.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Necessário para o Prometheus
#include <WebServer.h>  //Biblioteca para servidor http

// #Parte referente ao Firebase
#include <Firebase_ESP_Client.h>
// #Parte referente ao Firebase
#include "addons/TokenHelper.h" // Helper para mensagens de token Firebase
// #Parte referente ao Firebase
#include "addons/RTDBHelper.h"  // Helper para mensagens do Realtime Database

// Definindo os pinos utilizados:
#define tx 17
#define rx 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

float lastconcentration = 0.0; // Inicializa com 0.0 ppm

// Cria servidor http
WebServer server(80); // Porta padrão do http

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -10800, 60000); // UTC-3 para o horário de Brasília

// os pinos usados para conectar o display são SDA (Pino 21) e SCL (Pino 22)
// Cria uma instância de HardwareSerial no UART2 (pinos 16 e 17)
HardwareSerial sensorSerial(2);

// Declarando o display conectado usando comunicacao I2C (SDA,SCL)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Definindo os topicos utilizados pela comunicacao MQTT
#define TOPICO_SUBSCRIBE_H2S  "topico_ativacao_componentes" //topico para ativação de possíveis componentes pelo aplicativo via mqtt.
#define TOPICO_PUBLISH_LEITURA_H2S   "topico_h2s_if25JF"  //topico para publicação da medição do sensor de metano.
#define ID_MQTT  "IF25JFH2S"     //id mqtt (para identificação de sessão)

// Dados para acesso a rede
const char* SSID = "t"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "9"; // Senha da rede WI-FI que deseja se conectar

// Definicoes para o broker MQTT
const char* BROKER_MQTT1 = "test.mosquitto.org"; // URL do broker MQTT que se deseja utilizar
int BROKER_PORT1 = 1883; // Porta do Broker MQTT
const char* BROKER_MQTT2 = "broker.hivemq.com"; // URL do broker MQTT que se deseja utilizar
int BROKER_PORT2 = 1883; // Porta do Broker MQTT

WiFiClient espClient1; // Cria o objeto espClient
PubSubClient MQTT1(espClient1); // Instancia o Cliente MQTT passando o objeto espClient
WiFiClient espClient2; // Cria o objeto espClient
PubSubClient MQTT2(espClient2); // Instancia o Cliente MQTT passando o objeto espClient

// Flags para monitorar os estados dos brokers
bool mqtt1Ativo = false;
bool mqtt2Ativo = false;

/* // #Parte referente ao Firebase
#define API_KEY "AIzaSyAOReNZ3P5PAdu8_w2v_1wfT1I_mggiIeA"
// #Parte referente ao Firebase
#define DATABASE_URL "sensor-h2s-e5b12-default-rtdb.firebaseio.com"
// #Parte referente ao Firebase
FirebaseData fbdo;
// #Parte referente ao Firebase
FirebaseAuth auth;
// #Parte referente ao Firebase
FirebaseConfig config;
// #Parte referente ao Firebase
bool firebaseAtivo = false; */

// Prototipos das funcoes:
void initWiFi(void);
void initMQTT(void);
void reconnectMQTT1(void);
void reconnectMQTT2(void);
void reconnectWiFi(void);
void liga_display(String medicao);
/* // #Parte referente ao Firebase
void initFirebase(void);
// #Parte referente ao Firebase
void enviaDadosFirebase(float concentracao);
 */

// Função inicializa WI-FI
void initWiFi(void) 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    
    reconnectWiFi();
}

// Função: inicializa parâmetros de conexão MQTT(endereço do broker, porta e seta função de callback)
void initMQTT(void) {
    MQTT1.setServer(BROKER_MQTT1, BROKER_PORT1); //Mosquitto
    MQTT2.setServer(BROKER_MQTT2, BROKER_PORT2); //HiveMQ
    Serial.println("Configuração dos MQTTs completa.");
}

/* // #Parte referente ao Firebase
void initFirebase(void) {
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    Firebase.begin(&config, &auth);
    if (Firebase.ready()) {
        firebaseAtivo = true;
        Serial.println("Firebase inicializado com sucesso.");
    } else {
        firebaseAtivo = false;
        Serial.println("Falha ao inicializar o Firebase.");
    }
} */

void reconnectMQTT1(void) 
{
    if (!MQTT1.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT1);
        if (MQTT1.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao Mosquitto!");
            MQTT1.subscribe(TOPICO_SUBSCRIBE_H2S);
            mqtt1Ativo = true; 
        } 
        else
        {
            Serial.println("Falha conectando ao Mosquitto. Haverá nova tentativa depois.");
            mqtt1Ativo = false;
        }
    }
}

void reconnectMQTT2(void) 
{
    if (!MQTT2.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT2);
        if (MQTT2.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao HiveMQ!");
            MQTT2.subscribe(TOPICO_SUBSCRIBE_H2S);
            mqtt2Ativo = true; 
        } 
        else
        {
            Serial.println("Falha conectando ao HiveMQ. Haverá nova tentativa depois.");
            mqtt2Ativo = false;
        }
    }
}

void reconnectWiFi(void) 
{
    if (WiFi.status() == WL_CONNECTED)
        return;
          
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
      
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void liga_display(String medicao) {
    String concat = medicao + " ppm";
    display.setFont(&FreeMono9pt7b);
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(0,20);             
    display.println("Concetracao:");
    display.setCursor(0,60);   
    display.println(concat);    
    display.display();
    delay(1000); 
}

// #Parte referente ao Firebase
/* void enviaDadosFirebase(float concentracao) {
    if (!firebaseAtivo) return;

    if (Firebase.RTDB.setFloat(&fbdo, "/sensores/h2s/concentracao", concentracao)) {
        Serial.println("Dados enviados ao Firebase.");
    } else {
        Serial.println("Erro ao enviar dados ao Firebase: " + fbdo.errorReason());
    }
} */

unsigned long getUnixTimestamp() {
    timeClient.update(); // Atualiza a hora via NTP
    return timeClient.getEpochTime(); // Retorna em segundos
}

// Função para validar o checksum do pacote
bool validaChecksum(byte *data) {
    byte checksum = 0;
    for (int i = 1; i < 8; i++) { // Soma de bytes 1 a 7
        checksum += data[i];
    }
    checksum = ~checksum + 1; // Complemento de 2
    return (checksum == data[8]); // Verifica se o checksum bate
}

// Função que gera e envia as metricas
void handleMetrics() {
    if (lastconcentration == 0.0) {
        server.send(503, "text/plain", "No data available yet."); // Serviço não disponível
        return;
    }
    String metrics = "";
    metrics += "# HELP h2s_concentration H2S concentration in ppm\n";
    metrics += "# TYPE h2s_concentration gauge\n";
    metrics += "h2s_concentration " + String(lastconcentration) + "\n";
    server.send(200, "text/plain", metrics); // Envia as métricas
}

void setup() {
    Serial.begin(9600);

    // Inicia a comunicação serial com o sensor
    sensorSerial.begin(9600, SERIAL_8N1, 16, 17);

    // Aguarda o tempo de aquecimento do sensor, mude para 180000 se estiver frio.
    Serial.println("Aguardando aquecimento do sensor...");
    delay(1800);

    // Inicializa o display OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println("Falha ao inicializar o display SSD1306");
        for (;;);
    }

    // Inicializa Wi-Fi
    initWiFi();

    // Inicializa o cliente MQTT
    initMQTT();

 /*    // #Parte referente ao Firebase
    initFirebase(); */

    // Inicializa o cliente NTP
    timeClient.begin();

    Serial.println("Configuração completa!");

    // Adiciona servidor http e inicia ele
    server.on("/metrics", handleMetrics);
    server.begin();
    Serial.println("Servidor HTTP iniciado na porta 80.");
}

void loop() {
    // Verifica e reconecta Wi-Fi e MQTTs
    reconnectWiFi();     // Garante que o Wi-Fi está conectado
    reconnectMQTT1();    // Tenta reconectar ao Mosquitto
    reconnectMQTT2();    // Tenta reconectar ao HiveMQ

    // Leitura do sensor
    while (sensorSerial.available() >= 9) {
        byte response[9];
        while (sensorSerial.available() > 0 && sensorSerial.peek() != 0xFF) {
            sensorSerial.read();
        }
        if (sensorSerial.available() >= 9) {
            sensorSerial.readBytes(response, 9);
            if (response[0] == 0xFF && response[1] == 0x17 && validaChecksum(response)) {
                lastconcentration = (response[4] * 256 + response[5]) / 100.0;
                Serial.print("Concentração de H2S: ");
                Serial.print(lastconcentration);
                Serial.println(" ppm");

                // Publica nos brokers disponíveis
                if (mqtt1Ativo) {
                    char msg[10];
                    dtostrf(lastconcentration, 4, 2, msg);
                    MQTT1.publish(TOPICO_PUBLISH_LEITURA_H2S, msg);
                    Serial.println("Dados enviados ao Mosquitto.");
                }
                if (mqtt2Ativo) {
                    char msg[10];
                    dtostrf(lastconcentration, 4, 2, msg);
                    MQTT2.publish(TOPICO_PUBLISH_LEITURA_H2S, msg);
                    Serial.println("Dados enviados ao HiveMQ.");
                }

                // #Parte referente ao Firebase
//                enviaDadosFirebase(lastconcentration);
//            } else {
//                Serial.println("Pacote inválido ou corrompido. Ignorando...");
           }
        }
    }

    // Servidor HTTP para Prometheus
    server.handleClient();

    // Mantém os loops MQTT ativos
    if (mqtt1Ativo) {
        MQTT1.loop();
    }
    if (mqtt2Ativo) {
        MQTT2.loop();
    }

    delay(6000); // Atraso para reduzir o loop
}
