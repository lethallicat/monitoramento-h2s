// Configurações do Broker MQTT
const broker = "wss://test.mosquitto.org:8081"; // Endereço do broker (WebSocket)
const topic = "topico_sensor_h2s_if24JF";      // Tópico do sensor de H2S

// Conecta ao broker MQTT
const client = mqtt.connect(broker);

// Array para armazenar os dados de medições
let historicoMedicoes = [];
const maxPontos = 50; // Define o máximo de pontos no gráfico

// Configurações do gráfico
const ctx = document.getElementById('grafico').getContext('2d');
const chart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [], // Horários das medições
        datasets: [{
            label: 'Concentração de H2S (ppm)',
            data: [], // Valores das medições
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

// Atualiza o gráfico com os dados armazenados
function atualizarGrafico() {
    chart.data.labels = historicoMedicoes.map(item => item.horario); // Horários
    chart.data.datasets[0].data = historicoMedicoes.map(item => item.concentracao); // Concentrações
    chart.update();
}

// Conexão com o broker MQTT
client.on('connect', function () {
    console.log('Conectado ao broker MQTT');
    client.subscribe(topic); // Substitua pelo seu tópico correto
});

client.on('message', function (receivedTopic, message) {
    if (receivedTopic === topic) {
        // Processa a mensagem recebida
        const concentracao = parseFloat(message.toString());
        const horario = new Date().toLocaleTimeString(); // Horário atual

        // Atualiza os dados na página
        document.getElementById('dados').innerText = `Concentração: ${concentracao.toFixed(2)} ppm`;
        document.getElementById('hora').innerText = `Hora da última medição: ${horario}`;

        // Adiciona a medição ao histórico
        historicoMedicoes.push({ horario, concentracao });

        // Remove pontos mais antigos se exceder o limite
        if (historicoMedicoes.length > maxPontos) {
            historicoMedicoes.shift();
        }

        // Atualiza o gráfico
        atualizarGrafico();
    }
});
