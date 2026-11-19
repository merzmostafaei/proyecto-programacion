// Receptor LoRa (LilyGO T3-S3)
// Solo hace caso a los mensajes del NODO09.
// Trama del emisor: NODO09;MSG;001
// Comprueba la secuencia, cuenta los mensajes perdidos y lo muestra.

#include <SPI.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin de alimentacion para perifericos/OLED en LILYGO T3-S3
#define PIN_POWER_ON 15

// Pantalla OLED SSD1306
#define OLED_SDA 18
#define OLED_SCL 17
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Pines LoRa SX1262 para LILYGO T3-S3
#define LORA_SCK 5
#define LORA_MISO 3
#define LORA_MOSI 6
#define LORA_CS 7
#define LORA_DIO1 33
#define LORA_RST 8
#define LORA_BUSY 34

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

// Contadores
long ultimo = -1;     // numero del ultimo mensaje (-1 = todavia ninguno)
long recibidos = 0;   // mensajes validos recibidos
long perdidos = 0;    // mensajes perdidos detectados

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Activar alimentacion de la OLED
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
  delay(100);

  // Pantalla
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Radio a 868 MHz (igual que el emisor)
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI);
  int estado = radio.begin(868.0);

  display.clearDisplay();
  display.setCursor(0, 0);
  if (estado == RADIOLIB_ERR_NONE) {
    display.println("RECEPTOR NODO09");
    display.println("Esperando...");
    Serial.println("Receptor listo a 868 MHz");
  } else {
    display.println("Error de LoRa!");
    Serial.print("Fallo radio, codigo: ");
    Serial.println(estado);
  }
  display.display();
}

void loop() {
  String mensaje;

  // Esperar un mensaje
  if (radio.receive(mensaje) != RADIOLIB_ERR_NONE) {
    return;
  }

  // Ver todo lo que llega (util para comprobar que la radio funciona)
  Serial.println("Llega: " + mensaje);

  // Filtro: si no es del NODO09, ignorarlo
  if (!mensaje.startsWith("NODO09;")) {
    return;
  }

  // El numero va despues del ultimo ';'  (NODO09;MSG;001 -> 1)
  long numero = mensaje.substring(mensaje.lastIndexOf(';') + 1).toInt();

  recibidos++;
  
  // Calidad de la señal del ultimo mensaje
  float rssi = radio.getRSSI();   // potencia de la señal (dBm)
  float snr = radio.getSNR();     // relacion señal/ruido (dB)
  Serial.println("RSSI: " + String(rssi, 1) + " dBm  SNR: " + String(snr, 1) + " dB");
  Serial.println("Mensaje recibido: " + String(numero));

  // Comprobar la secuencia
  if (ultimo == -1) {
    Serial.println("Primer mensaje");
  } else if (numero == ultimo + 1) {
    Serial.println("Secuencia correcta");
  } else if (numero > ultimo + 1) {
    long faltan = numero - ultimo - 1;
    perdidos = perdidos + faltan;
    Serial.println("Mensajes perdidos: " + String(faltan));
  } else {
    Serial.println("El emisor se ha reiniciado");
  }
  ultimo = numero;

  // Porcentaje de perdidos (hacen falta al menos 2 mensajes)
  String porcentaje = "---";
  if (recibidos >= 2) {
    float p = 100.0 * perdidos / (recibidos + perdidos);
    porcentaje = String(p, 1) + "%";
  }

  // Contadores por el monitor serie
  Serial.println("Recibidos: " + String(recibidos) +
                 "  Perdidos: " + String(perdidos) +
                 "  Porcentaje: " + porcentaje);
  Serial.println();

  // Pantalla: mensaje y estado de la comunicacion
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("RECEPTOR NODO09");
  display.println("-----------------");
  display.println("Mensaje N: " + String(numero));
  
  // Calidad de la señal del ultimo mensaje
  float rssi = radio.getRSSI();   // potencia de la señal (dBm)
  float snr = radio.getSNR();     // relacion señal/ruido (dB)
  Serial.println("RSSI: " + String(rssi, 1) + " dBm  SNR: " + String(snr, 1) + " dB");
}
