Monitoramento de H2S com ESP32

Introdução

Este projeto utiliza um ESP32 para monitorar a concentração de H2S (Sulfeto de Hidrogênio) e enviar os dados para serviços MQTT, Firebase e um servidor HTTP. O sistema também exibe a leitura no display OLED e permite a coleta dos dados via Prometheus.

📑 Table of Contents

Features

Instalação

Configuração

Utilização

Tutorial

Roadmap

Contribuição

Licença

🚀 Features

Conexão com Wi-Fi e recuperação automática em caso de falha.

Publicação dos dados em dois brokers MQTT (Mosquitto e HiveMQ).

Opcional - Armazenamento das leituras no Firebase Realtime Database.

Exibição da concentração no display OLED.

Servidor HTTP para expor métricas no formato Prometheus.

Sincronização de horário via NTP.

🛠 Instalação

Requisitos

ESP32

Sensor de H2S (compatível com comunicação UART)

Opcional - Display OLED SSD1306

Ambiente de desenvolvimento Arduino IDE

Opcional - PlatformIO, se utilizar VSCode

Bibliotecas necessárias:

WiFi.h

PubSubClient.h

HardwareSerial.h

Wire.h

Adafruit_GFX.h

Adafruit_SSD1306.h

NTPClient.h

WiFiUdp.h

WebServer.h

Firebase_ESP_Client.h //opcional

⚙️ Configuração

Antes de compilar e carregar o código, configure:

O nome e senha do Wi-Fi:

const char* SSID = "SEU_WIFI";
const char* PASSWORD = "SUA_SENHA";

Os brokers MQTT e os tópicos de comunicação:

#define BROKER_MQTT1 "test.mosquitto.org"
#define BROKER_MQTT2 "broker.hivemq.com"
#define TOPICO_PUBLISH_LEITURA_H2S "topico_h2s_if25JF"

Configurar o Firebase:

#define API_KEY "SUA_API_KEY"
#define DATABASE_URL "SEU_DATABASE_URL"

📌 Utilização

Após a configuração e upload do código no ESP32:

O sensor fará a leitura da concentração de H2S.

Os dados serão publicados nos brokers MQTT configurados.

As medições serão armazenadas no Firebase. (opcional)

O display OLED exibirá a última leitura.

O servidor HTTP estará acessível em http://<IP_DO_ESP>/metrics para coleta via Prometheus.

📖 Tutorial

Conecte os dispositivos conforme o esquema de fiação.

Compile e envie o código para o ESP32.

Monitore as leituras via Serial Monitor.

Utilize um cliente MQTT para visualizar os dados publicados.

Acesse o Firebase para verificar os registros armazenados. (opcional)

Envie os dados para um servidor local (ex.: Prometheus) ou na núvem (ex.: google cloud, aws...).

Configure Prometheus para coletar as métricas HTTP.

Caso queira, utilize o app.js e o index.html para criar uma página com gráfico em tempo real do sensor, utilizando as ferramentas disponíveis do Github.

🔜 Roadmap

Criar um website para visualizar os dados em tempo real e histórico.


🤝 Contribuição

Contribuições são bem-vindas! Para colaborar:

Faça um fork do repositório.

Crie uma branch com sua feature (git checkout -b minha-feature).

Faça um commit das alterações (git commit -m 'Adiciona nova funcionalidade').

Envie para o repositório (git push origin minha-feature).

Abra um Pull Request.

📜 Licença

Este projeto é licenciado sob a MIT License. Veja o arquivo LICENSE para mais detalhes.

