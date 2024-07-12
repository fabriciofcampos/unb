//		*************************************
//              ******     ECG VERSÃO v5       ******
//		*************************************

// ALUNOS: ALACIDE E FABRICIO.
// DISCIPLINA TESD 24-1/PROF. ADSON.

// BIBLIOTECAS ARDUINO
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimerOne.h>

// CONSTANTES.
#define LARGURA_DISPLAY 128
#define ALTURA_DISPLAY 64
#define RESET_DISPLAY  -1
#define ADDRESS_DISPLAY 0x3C  	// Endereço I2C do display (comumente 0x3C ou 0x3D).
// #define osciladorPino8 8	// Define o pino de saída para o oscilador.


// CONSTANTES / VARIAVEIS.

// **** OSCILADOR ONDAS QUADRADAS DE 60Hz ****
const int osciladorPino8 = 8; // Pino de saída para a onda quadrada
const unsigned long meioPeriodo = 1; // meioPeriodo = meio período em microsegundos (1 / (2 * 60 Hz) = 8333.33 us)
unsigned long anteriorMicros = 0; // Armazena o tempo da última mudança de estado
bool estadoPino = LOW; // Estado inicial do pino


// **** FILTRO DIGITAL "NOTCH" DE 60 Hz **** (Versao Prof. Adson)
const int sampleRate = 120;
const float a0 = 1.0;
const float a1 = 1.8;
const float b1 = -0.81;
int x[2] = {0, 0}; // Entrada
int y[2] = {0, 0}; // Saída
int amostrafiltrada;
int amostraatual;

// BUFFER CIRCULAR OTIMIZADO PARA ACESSO DIRETO.
volatile int bufferAmostragens[LARGURA_DISPLAY];
volatile int indiceBuffer = 0;
// Fator de zoom.
float fatorZoom = 1.3;
float Vbat;

//CONFIGURACAO DO DISPLAY
Adafruit_SSD1306 display(LARGURA_DISPLAY, ALTURA_DISPLAY, &Wire, RESET_DISPLAY);


// ++++ FUNÇÃO DE INTERRUPÇÃO ACIONADA PELO TIMER ++++
void amostrasAnalog_INT() {
	// EXECUTADA VIA INTERRUPÇÃO A CADA 8333 us = 8,333ms (1/120 Hz) // CONFIGURADO NO VOID SETUP

	// **** FILTRO DIGITAL "NOTCH" DE 60 Hz ****
	int amostraAtual = 0;
	int amostraFiltrada = 0;
	int amostraSuja = analogRead(A0); // Faz leituras do sinal presente na entrada A0 (ADC).
	// Atualiza o buffer de entrada
	x[1] = x[0]; // x[0] e x[1] são as entradas atuais e anteriores, respectivamente.
	x[0] = amostraSuja;
	// Aplica o filtro
	y[0] = (a0 * x[0] + a1 * x[1] - b1 * y[1]);
	// Atualiza o buffer de saída
	y[1] = y[0]; // y[0] e y[1] são as saídas atuais e anteriores, respectivamente.
	amostraFiltrada = y[0];
  amostrafiltrada = amostraFiltrada;

	// **** ACIONAMENTO OU NÃO DO FILTRO "NOTCH"   **** 
	// Lê o estado do pino 2
	int estadoPino = digitalRead(2); 
	// Verifica se o pino 2 está em HIGH (5V)
	if (estadoPino == HIGH) {    // SE ABERTO (5V/HIGH), LIGA FILTRO "NOTCH".
	amostraAtual = amostraFiltrada/14;
	}
	// Verifica se o pino 2 está em LOW (0V)
	else if (estadoPino == LOW) { // SE ATERRADO (0V/LOW), DESLIGA FILTRO "NOTCH".
	amostraAtual = amostraSuja;
	}

  amostraatual = amostraAtual;
	// **** ALIMENTAÇÃO DO BUFFER CÍCLICO COM AS AMOSTRAS FILTRADAS ****
	// Ajusta os valores de tensão de 0V(0) e 5V(1023) para o tamanho do display 0-63.
	int amostraPlotavel = map(amostraAtual, 300, 700, 0, 63) * fatorZoom; 
	// Garante que o valor não ultrapasse a altura (64 pixels) do display. 
	amostraPlotavel = constrain(amostraPlotavel, 20, ALTURA_DISPLAY - 1); 
	// Atualiza o buffer circular
	bufferAmostragens[indiceBuffer] = amostraPlotavel; 
	indiceBuffer = (indiceBuffer + 1) % LARGURA_DISPLAY; // LARGURA_DISPLAY = 128 
	// % é o operador de MODULO e retorna o resto da divisão. Ex.: 7/128 = 7 (resto), 128/128 = 0 (resto).

	// Toggle na porta digital 7 (pino não-PWM).
	digitalWrite(7, !digitalRead(7));
  
}


//++++ CONFIGURAÇÕES DE INICIALIZAÇÃO ++++
void setup() {

	// **** OSCILADOR ONDAS QUADRADAS DE 60Hz ****
	pinMode(osciladorPino8, OUTPUT); // Configura o pino 8 (não PWM) como saída.
	digitalWrite(osciladorPino8, estadoPino); // Define o estado inicial do pino


	/* **** CONFIG. DE AREF DO ADC ****
	// Configura ref. externa de tensão p/ o ADC (pino AREF = teto (fundo de escala) de tensão do ADC). 
	// ADC ARDUINO UNO SMD REV3 = 2^10, logo conta de 0 a 1023 (0V a AREF V). Não indica OVERLOAD, logo ultima contagem "1023" já pode ser usado como tal.
		analogReference (EXTERNAL); // Injetado em AREF: 1002 mV  (Sem código/configuracao/injeção (DEFAULT) => AREF = VCC = 3300 ou 5000 mV).
	// Se desejar usar AREF interna, ela = 1100 mV e setar p/: analogReference (INTERNAL);
	// OBS: "Buffer AmpOp TL064" com 1100mV em IN+ só libera até 1002mV em sua saída. Problema pode ser VCCs baixos: com +2,5V e -2,5V (5V).
	*/
	
	// INICIALIZA A COMUNICAÇÃO SERIAL PARA DEPURAÇÃO.
	Serial.begin(115200);

	// **** CONFIG. PINO ACIONAMENTO DO FILTRO "NOTCH""NOTCH" OU NÃO  **** 
	// Configura o pino 2 como entrada
	pinMode(2, INPUT);

	// **** CONFIG. DO DISPLAY ****
	// Inicializa o display com a comunicação I2C
	if(!display.begin(SSD1306_SWITCHCAPVCC, ADDRESS_DISPLAY)) {
	Serial.println(F("Falha ao inicializar o display SSD1306"));
	for(;;); // Trava em um loop infinito se ocorrer algum erro com o display (!display).
	}
	// Limpa o buffer do display
	display.clearDisplay();
	// TESTE DISPLAY: Preenche a tela com a cor branca (acende todos os pixels AZUIS E AMARELO)
	display.fillScreen(SSD1306_WHITE);
	// Atualiza o display com o conteúdo do buffer
	display.display();
	delay (1000); // Tempo do teste do display na INICIALIZAÇÃO antes da INTERRUPÇÃO CÍCLICA (10ms) entrar em operação.

	// CONFIGURA O PINO A0 COMO ENTRADA
	pinMode(A0, INPUT); 

	// CONFIGURA O PINO D7 COMO SAÍDA
	pinMode(7, OUTPUT); 
	// Inicializa D7 como LOW
	digitalWrite(7, LOW); 

	// INICIALIZA O TIMER1
	Timer1.initialize(8333); // Configura o timer para gerar INT a cada 8333 us = 8,333 ms (TAXA DE MOASTRAGEM = 120 Hz)
	Timer1.attachInterrupt(amostrasAnalog_INT); // Ativa a interrupção do timer
  
}

// ++++ LOOP DE EXECUÇÃO ++++
void loop() {

	// **** OSCILADOR ONDAS QUADRADAS DE 60Hz ****
	unsigned long currentMicros = micros(); // Lê o tempo atual em microsegundos
	// Verifica se é hora de alternar o estado do pino
	if (currentMicros - anteriorMicros >= meioPeriodo) {
	anteriorMicros = currentMicros; // Atualiza o tempo da última mudança de estado
	estadoPino = !estadoPino; // Alterna o estado do pino
	digitalWrite(osciladorPino8, estadoPino); // Define o novo estado do pino
	}

	// **** PLOTAGEM DO BUFFER CÍCLICO **** 
	
	// Variável para controle de tempo
	static unsigned long atualizaPlot8333us = micros();  // 8333us = 1/120 Hz => Taxa de amostragem = 120 Hz
	// Atualiza o display com o buffer de amostragens a cada 10ms desde a última tela. 
	if (  (micros() - atualizaPlot8333us) >= 8333) {
		atualizaPlot8333us = micros();
		display.clearDisplay(); // Limpa o display
		// Desenha os pontos/linhas (128 x 64 pixels) gravados
		// no buffer de Amostragens nos últimos 10ms:
		for (int i = 0; i < LARGURA_DISPLAY; i++) {    // LARGURA_DISPLAY = 128
			int indice = (indiceBuffer + i) % LARGURA_DISPLAY;
			int proxIndice = (indiceBuffer + i + 1) % LARGURA_DISPLAY;
			if (i < LARGURA_DISPLAY - 1) {
				display.drawLine(i, bufferAmostragens[indice], i + 1, bufferAmostragens[proxIndice], SSD1306_WHITE);
			} else {
				display.drawPixel(i, bufferAmostragens[indice], SSD1306_WHITE);
			}
		}
	display.setTextSize(2);
	display.setTextColor(WHITE);

	// **** LEITURA E PLOTAGEM TENSÃO DA BATERIA ****
	int valorADC = analogRead (A1);  // Lê sinal bateria em A1 (ADC) via divisor por aprox. 10.15 de tensão. Resistores: 56k e 6k2.
	Vbat = (Vbat + (valorADC * (5.0 / 1023.0) * 10.05) ) / 2; // Converte valor ADC p/ tensão (divisor = 10.5 porque teto = 1,002V). Calc. média p/ estabilizar leitura.    
	display.setCursor(0,0);
	display.println(amostraatual);
	display.setCursor(60,0);
	//display.println(Vbat/10);
	display.setCursor(105,0);
	display.println(amostrafiltrada/14);

	// Atualiza o display
	display.display();
	}
}
