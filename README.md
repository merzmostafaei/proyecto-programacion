# proyecto-programacion

Comunicación LoRa punto a punto entre dos placas **LilyGO T3-S3 (SX1262)** a 868 MHz, con control de secuencia y detección de mensajes perdidos.

Autor: Mohammadreza Mostafaei

## Descripción del proyecto

El proyecto está formado por dos programas:

- **Emisor** (`Emisor/Emisor.ino`): envía cada 2 segundos una trama con el identificador del nodo y un número de secuencia, por ejemplo `NODO09;MSG;001`.
- **Receptor** (`Receptor/Receptor.ino`): recibe las tramas, acepta solo las del `NODO09`, comprueba que la secuencia es correcta y cuenta los mensajes perdidos.

La finalidad es comprobar la fiabilidad de un enlace LoRa: cuántos mensajes llegan, cuántos se pierden y con qué calidad de señal.

## Tecnologías utilizadas

- **Lenguaje:** C++ (Arduino)
- **Entorno de desarrollo:** Arduino IDE / arduino-cli y Sublime Text
- **Hardware:** 2 × LilyGO T3-S3 (ESP32-S3 + transceptor LoRa SX1262 + pantalla OLED SSD1306)
- **Control de versiones:** Git, GitHub y GitHub Desktop (versión para Linux, en Ubuntu)

## Funcionamiento

**Emisor**
1. Inicia la pantalla OLED y la radio LoRa a 868 MHz.
2. Construye la trama `NODO09;MSG;NNN`, con el número de mensaje con tres cifras.
3. La transmite, muestra el resultado en el monitor serie y en la pantalla, incrementa el contador y espera 2 segundos.

**Receptor**
1. Espera un paquete LoRa.
2. Descarta los mensajes que no empiezan por `NODO09;`.
3. Extrae el número de secuencia y lo compara con el anterior:
   - número = anterior + 1 → secuencia correcta
   - número > anterior + 1 → se suman los mensajes que faltan como perdidos
   - número < anterior → el emisor se ha reiniciado
4. Calcula el porcentaje de pérdida: `perdidos / (recibidos + perdidos) × 100`.
5. Lee la calidad de la señal del último paquete: **RSSI** (potencia, en dBm) y **SNR** (relación señal/ruido, en dB).
6. Muestra en la pantalla OLED y en el monitor serie: número de mensaje, recibidos, perdidos, porcentaje de pérdida, RSSI y SNR.

## Requisitos

- Arduino IDE con el soporte para placas **ESP32** (Espressif) instalado.
- Bibliotecas:
  - [RadioLib](https://github.com/jgromes/RadioLib)
  - [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
  - [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
  - `SPI` y `Wire` (incluidas en el núcleo de Arduino)
- Dos placas LilyGO T3-S3 con antena conectada.
- Monitor serie a **115200 baudios**.

**Uso:** cargar `Emisor.ino` en una placa y `Receptor.ino` en la otra, y alimentar las dos.

## Pruebas realizadas

**Prueba previa del receptor (14/10/2026)** — monitor serie con `arduino-cli monitor -p /dev/ttyACM0 -c baudrate=115200`:

- La radio se inicializó correctamente (código `0`, «LoRa iniciado correctamente»).
- Se recibieron tramas de otros nodos del aula (`NODO06;LUZ;...`, `NODO003;LUMINOSIDAD;...`, `Nodo07,...`). Esto demostró que en la misma frecuencia transmiten varios nodos y motivó el **filtro por `NODO09`** del receptor actual.
- Apareció un error de recepción `-7` (en RadioLib, paquete con CRC incorrecto), que el programa descarta.

**Pruebas pendientes con la versión actual (emisor + receptor NODO09):**

| Prueba | Resultado esperado |
|---|---|
| Envío y recepción de mensajes | El receptor muestra «Secuencia correcta» y aumentan los recibidos |
| Filtro de nodo | Las tramas de otros nodos no se cuentan |
| Pérdida de mensajes (alejar las placas) | Aumentan los perdidos y el porcentaje de pérdida |
| Reinicio del emisor | El receptor muestra «El emisor se ha reiniciado» |
| RSSI y SNR | Los valores aparecen en la pantalla y empeoran con la distancia |

## Historial de versiones

| Commit | Cambio |
|---|---|
| Initial commit | Creación del repositorio con README |
| Añadir código fuente del proyecto | Código del emisor y del receptor |
| Mostrar RSSI y SNR en la pantalla del receptor | Mejora: calidad de la señal en el receptor |
| Change rssi,snr | Segundo intento de la mejora (contenía errores) |
| Corregir la visualización de RSSI y SNR en el receptor | Corrección del código de la mejora |
| Documentar el proyecto | Este README |

## Fuentes de información

- Documentación de RadioLib: https://jgromes.github.io/RadioLib/
- Repositorio de LilyGO (pines de la T3-S3): https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series
- Biblioteca Adafruit SSD1306: https://github.com/adafruit/Adafruit_SSD1306
- Documentación de Arduino: https://docs.arduino.cc/
- Documentación de GitHub Desktop: https://docs.github.com/es/desktop
- GitHub Desktop para Linux (shiftkey/desktop): https://github.com/shiftkey/desktop
- Material de clase del módulo
