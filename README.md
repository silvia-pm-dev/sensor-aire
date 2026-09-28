# ESP32 Environmental & Air Quality Monitor 

Estació de monitorització ambiental basada en microcontrolador ESP32 dissenyada per a l'adquisició, calibratge i registre de paràmetres de qualitat de l'aire interior en temps real.

El sistema integra múltiples busos de comunicació per capturar de forma sincronitzada concentracions de CO₂, partícules en suspensió (PM), compostos orgànics volàtils (VOCs) i condicions termo-higromètriques.

---

## Maquinari i sensors

- **Controlador principal:** Placa de desenvolupament **ESP32** (dual-core, suport Wi-Fi / Bluetooth).
- **Sensor de CO₂:** **SenseAir S8**
  - Tecnologia NDIR (infraroig no dispersiu) d'alta precisió.
  - Comunicació sèrie (UART) per a la lectura directa de concentració en ppm.
- **Sensor ambiental multivariable:** **Bosch BME680**
  - Temperatura (°C), Humitat relativa (%), Pressió atmosfèrica (hPa).
  - Sensor de gas per a detecció de COVs (Compostos Orgànics Volàtils) i càlcul de l'índex de qualitat de l'aire (IAQ).
  - Comunicació mitjançant bus **I2C**.
- **Sensor de pols / partícules:** Sensor làser de partícules en suspensió (mesura de concentració PM2.5 i PM10).
- **Connexions i encapsulat:** Connexions mitjançant cablejat Dupont d'alta densitat sobre protoboard / caixa de protecció aïllada.

---

## Arquitectura del programari

- **Llenguatge:** C / C++ (Framework Arduino / ESP32 Core).
- **Entorn de desenvolupament:** Arduino IDE.
- **Protocols de comunicació utilitzats:**
  - **I2C:** Interfície de lectura per al bus del BME680.
  - **UART / HardwareSerial / SoftwareSerial:** Transmissió bidireccional per comandaments de sondeig cap al sensor SenseAir S8 i recepció de trames del sensor de partícules.
  - **USB-Serial:** Telemetria i monitorització de dades estructurades pel Serial Monitor.

---

## Funcionament

1. **Inicialització (`setup`):**
   - Arrencada dels ports sèrie de depuració i dels perifèrics de comunicació.
   - Escaneig i inicialització del bus I2C per al BME680.
   - Comprovació de disponibilitat i escalfament (*warm-up*) del sensor NDIR SenseAir S8.
2. **Cicle d'adquisició (`loop`):**
   - Sondeig periòdic no bloquejant (mitjançant temporització per `millis()`).
   - Lectura i parsat de les trames binàries/hexadecimals de concentració de CO₂ i PM.
   - Compensació tèrmica i de pressió de les lectures de gas del BME680.
3. **Formatat i sortida:**
   - Estandardització de les lectures en mètriques ambientals consolidades (ppm, µg/m³, hPa, °C, %).
   - Emissió de registres periòdics estructurats per a supervisió i diagnòstic.

---
