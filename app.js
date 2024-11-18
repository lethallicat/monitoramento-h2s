// Configurações do Broker MQTT
const broker = "wss://test.mosquitto.org:8081"; // Endereço do broker (WebSocket)
const topic = "topico_sensor_h2s_if24JF";      // Tópico do sensor de H2S

// Conecta ao broker MQTT
const client = mqtt.connect(broker);

// Dados para o gráfico
let labels = []; // Horários
let data = [];   // Valores de concentração

// Configuração do gráfico
const ctx = document.getElementById('grafico').getContext('2d');
const chart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: labels,
        datasets: [{
            label: 'Concentração de H2S (ppm)',
            data: data,
            borderColor: 'rgba(75, 192, 192, 1)',
            backgroundColor: 'rgba(75, 192, 192, 0.2)',
            tension: 0.4,
        }]
    },
    options: {
        scales: {
            x: {
                title: {
                    display: true,
                    text: 'Horário'
                }
            },
            y: {
                title: {
                    display: true,
                    text: 'Concentração (ppm)'
                },
                beginAtZero: true
            }
        },
        responsive: true,
    }
});

// Conecta ao Broker
client.on('connect', () => {
    console.log("Conectado ao Broker MQTT!");
    client.subscribe(topic, (err) => {
        if (!err) {
            console.log(`Inscrito no tópico: ${topic}`);
        } else {
            console.error("Erro ao se inscrever no tópico:", err);
        }
    });
});

// Recebe dados do MQTT
client.on('message', (receivedTopic, message) => {
    // Processa apenas mensagens do tópico esperado
    if (receivedTopic === topic) {
        const concentracao = message.toString();
        const agora = new Date();
        const horaAtual = agora.toLocaleTimeString();

        // Atualiza a exibição na página
        document.getElementById('dados').textContent = `Concentração: ${concentracao} ppm`;
        document.getElementById('hora').textContent = `Hora da última medição: ${horaAtual}`;

        // Adiciona os dados no gráfico
        labels.push(horaAtual);
        data.push(parseFloat(concentracao));

        // Limita o histórico no gráfico (exemplo: 25 últimas medições)
        if (labels.length > 25) {
            labels.shift();
            data.shift();
        }

        // Atualiza o gráfico
        chart.update();
    }
});

// Tratamento de erros
client.on('error', (err) => {
    console.error("Erro de conexão:", err);
});
