
#include <SPI.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin de alimentación para periféricos/OLED en LILYGO T3-S3
#define PIN_POWER_ON 15

// Configuración I2C y Pantalla OLED SSD1306
#define OLED_SDA 18
#define OLED_SCL 17
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pines LoRa SX1262 para LILYGO T3-S3
#define LORA_SCK 5
#define LORA_MISO 3
#define LORA_MOSI 6
#define LORA_CS 7
#define LORA_DIO1 33
#define LORA_RST 8
#define LORA_BUSY 34

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

// Identificación del Nodo y Secuencia de Control
const String idNodo = "NODO09";
unsigned long numeroMensaje = 1;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 1. Activar alimentación física de la OLED
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
  delay(100);

  // 2. Inicializar Pantalla OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error al iniciar pantalla OLED");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // 3. Inicializar Bus SPI y Módulo LoRa a 868 MHz
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI);
  int estado = radio.begin(868.0);

  display.setCursor(0, 0);
  if (estado == RADIOLIB_ERR_NONE) {
    display.println("EMISOR NODO09");
    display.println("-----------------");
    display.println("LoRa 868 MHz: OK");
    Serial.println("Emisor NODO09 listo a 868 MHz");
  } else {
    display.println("Error de LoRa!");
    Serial.print("Fallo radio, codigo: "); Serial.println(estado);
  }
  display.display();
  delay(1500);
}

void loop() {
  // 1. Formatear el número de mensaje con ceros a la izquierda (ej. 001, 002, 003...)
  char numMsgStr[6];
  sprintf(numMsgStr, "%03lu", numeroMensaje);

  // 2. Construcción de la trama simplificada: NODO09;MSG;001
  String mensaje = idNodo + ";MSG;" + String(numMsgStr);

  // 3. Envío del paquete por radiofrecuencia
  int estadoTx = radio.transmit(mensaje.c_str());

  // 4. Salida por el Monitor Serie
  Serial.println("----------------------------------------");
  if (estadoTx == RADIOLIB_ERR_NONE) {
    Serial.print("Enviado correctamente: ");
    Serial.println(mensaje);
  } else {
    Serial.print("Error al enviar paquete, codigo: ");
    Serial.println(estadoTx);
  }

  // 5. Salida por la Pantalla OLED
  display.clearDisplay();
  display.setCursor(0, 0);

  if (estadoTx == RADIOLIB_ERR_NONE) {
    display.println("ESTADO: Enviado OK");
    display.println("-----------------");
    display.print("Nodo: "); display.println(idNodo);
    display.print("Mensaje N: "); display.println(numMsgStr);
    display.println();
    display.println("Transmitiendo...");
  } else {
    display.println("ESTADO: Error Tx");
    display.println("-----------------");
    display.print("Mensaje N: "); display.println(numMsgStr);
    display.println("Fallo al enviar!");
  }

  display.display();

  // Incrementar contador para el siguiente mensaje y esperar 2 segundos
  numeroMensaje++;
  delay(2000);
}
