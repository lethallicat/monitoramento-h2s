// Configurações dos Brokers MQTT
const brokers = [
	"wss://test.mosquitto.org:8081", // Mosquitto (WebSocket)
	"wss://broker.hivemq.com:8000/mqtt" // HiveMQ (WebSocket)
];
const topic = "topico_h2s_if25JF"; // Tópico do sensor de H2S

// Determina qual broker usar
let brokerIndex = localStorage.getItem("brokerIndex") || 0;
brokerIndex = parseInt(brokerIndex, 10) % brokers.length;
const broker = brokers[brokerIndex];

// Conecta ao broker MQTT
console.log(`Tentando conectar ao Broker: ${broker}`);
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
			x: { title: { display: true, text: 'Horário' } },
			y: { title: { display: true, text: 'Concentração (ppm)' } }
		},
		responsive: true,
	}
});

// Conecta ao Broker
client.on('connect', () => {
	console.log(`Conectado ao Broker MQTT: ${broker}`);
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

	// Tenta mudar para o próximo broker
	brokerIndex = (brokerIndex + 1) % brokers.length;
	localStorage.setItem("brokerIndex", brokerIndex);

	// Força o reload da página para tentar conectar ao próximo broker
	alert(`Erro ao conectar ao broker: ${broker}. Tentando próximo broker...`);
	location.reload();
});

// Lógica para expandir e reduzir o gráfico ao clicar
const graficoContainer = document.getElementById('grafico-container');
graficoContainer.addEventListener('click', () => {
	graficoContainer.classList.toggle('fullscreen'); // Adiciona ou remove a classe 'fullscreen'
});
