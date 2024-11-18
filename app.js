// Configurações do Broker MQTT
const broker = "wss://test.mosquitto.org:8081"; // Endereço do broker (WebSocket)
const topic = "topico_sensor_h2s_if24JF";      // Tópico do sensor de H2S

// Conecta ao broker MQTT
const client = mqtt.connect(broker);

// Dados para o gráfico
let labels = []; // Horários
let data = [];   // Valores de concentração

// Array para armazenar os dados de medições
let historicoMedicoes = [];
const maxPontos = 50; // Define o máximo de pontos no gráfico

// Configurações do gráfico
const ctx = document.getElementById('grafico').getContext('2d');
const chart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Concentração de H2S (ppm)',
            data: [],
            borderColor: 'teal',
            borderWidth: 1,
            fill: false,
            pointRadius: 4,
            pointBackgroundColor: 'teal',
        }]
    },
    options: {
        responsive: true,
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
        }
    }
});

// Atualiza o gráfico com dados armazenados no array
function atualizarGrafico() {
    chart.data.labels = historicoMedicoes.map(item => item.horario);
    chart.data.datasets[0].data = historicoMedicoes.map(item => item.concentracao);
    chart.update();
}

// Configuração MQTT
const client = mqtt.connect('wss://test.mosquitto.org:8081'); // Broker público

client.on('connect', function () {
    console.log('Conectado ao broker MQTT');
    client.subscribe('topico_sensor_h2s_if24JF'); // Substituir pelo seu tópico
});

client.on('message', function (topic, message) {
    // Recebe a medição do sensor
    const concentracao = parseFloat(message.toString());
    const horario = new Date().toLocaleTimeString(); // Horário atual

    // Exibe na página
    document.getElementById('dados').innerText = `Concentração: ${concentracao.toFixed(2)} ppm`;
    document.getElementById('hora').innerText = `Hora da última medição: ${horario}`;

    // Armazena a medição no array
    historicoMedicoes.push({ horario, concentracao });

    // Limita o número de pontos no gráfico
    if (historicoMedicoes.length > maxPontos) {
        historicoMedicoes.shift(); // Remove o ponto mais antigo
    }

    // Atualiza o gráfico
    atualizarGrafico();
});
