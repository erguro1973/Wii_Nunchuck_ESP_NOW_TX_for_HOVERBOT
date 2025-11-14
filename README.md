# 4x4 Nunchuk ESP-NOW Transmitter for HOVERBOT
### "Lessons Learned" Version for XIAO ESP32-C6

This project turns a Wii Nunchuk controller into a 4x4 low-latency controller for a custom vehicle (like a hoverboard) that uses **two separate ESP-NOW receivers** (one for the front axle, one for the rear).

It is
specifically designed for the **Seeed Studio XIAO ESP32-C6** and solves a critical hardware conflict between the default I2C and USB Serial ports.

## 🚀 Key Features

* **Dual Axle Control:** Sends identical control data to two receiver MAC addresses simultaneously.
* **"Freewheel" Logic:** When the **C Button** is held, the transmitter sends "centered" data (joystick and accelerometer at rest) **only to Receiver 2 (rear axle)**. This allows the rear axle to freewheel for tight turns while the front axle maintains control.
* **Custom I2C Pins:** Uses `D4 (GPIO 22)` and `D5 (GPIO 23)` for I2C, avoiding the "heat" / USB Serial conflict on the C6's default pins.
* **Minimal Library:** Uses a custom `MinimalNunchuk.h` library that **does not** call `Wire.begin()` automatically, allowing for manual pin re-mapping.
* **Selectable Antenna:** Includes logic to select between the XIAO C6's internal (PCB) or external (U.FL) antenna.

---

## 🛠️ Hardware Components

* **Controller:** Seeed Studio XIAO ESP32-C6.
* **Input:** A Wii Nunchuk (original or compatible).
* **Connector:** Nunchuk adapter (or wires soldered directly).
* **Critical Component (I2C):** 2x **4.7kΩ** Pull-Up Resistors.
* **Power:** 3.7V LiPo Battery + 220uF-1000uF Capacitor (Recommended, see previous READMEs).

---

## 🔌 Wiring

Unlike other boards, the XIAO C6's I2C pins require **external pull-up resistors** to function reliably.

| Nunchuk (Wire) | XIAO Pin (Function) | Physical Pin |
| :--- | :--- | :--- |
| **GND** (White) | `GND` | `GND` |
| **+3.3V** (Red) | `3V3` | `3V3` |
| **SDA** (Green) | `GPIO 23` | `D5` |
| **SCL** (Yellow)| `GPIO 22` | `D4` |

**Mandatory Pull-Up Resistor Wiring:**
* Connect one **4.7kΩ** resistor between `D5 (SDA)` and `3V3`.
* Connect one **4.7kΩ** resistor between `D4 (SCL)` and `3V3`.

---

## ⚙️ Software Setup

### 1. Environment (Arduino IDE)

Ensure you have the Espressif ESP32 board manager installed and select "Seeed Studio XIAO ESP32C6" as your board.

### 2. Libraries

This project requires a custom Nunchuk library (`MinimalNunchuk.h`) to be added to your project folder.

* `WiFi.h` (Included with ESP32)
* `esp_now.h` (Included with ESP32)
* `Wire.h` (Included)
* `MinimalNunchuk.h` (Must be added manually to your project!)

### 3. Transmitter Configuration

Before compiling, you must:

1.  **Update MAC Addresses:** Replace the values in `receiver_mac_1` and `receiver_mac_2` with the MAC addresses of your two receiver boards.
2.  **Select Antenna:** In the code, decide whether to use the internal or external antenna. By default, `USE_EXTERNAL_ANTENNA` is **uncommented** (using the external U.FL antenna). Comment out this line to use the internal PCB antenna.

---

## 🔧 Troubleshooting

* **"Nunchuk not detected":** You did not install the **4.7kΩ pull-up resistors** on pins D4 and D5. They are mandatory.
* **TX reboots on battery:** (If using battery) You have not installed the 220uF-1000uF capacitor between 3.3V and GND.
* **Receiver gets nothing:** Verify the MAC addresses are correct and that all devices are on the same Wi-Fi channel (Channel 1 by default).


# Transmisor Nunchuk 4x4 con ESP-NOW para HOVERBOT (XIAO ESP32-C6)

Este proyecto convierte un mando Nunchuk de Wii en un controlador 4x4 de baja latencia para un vehículo personalizado (como un hoverboard) que utiliza **dos receptores ESP-NOW separados** (uno para el eje delantero y otro para el trasero).

Está diseñado específicamente para la placa **Seeed Studio XIAO ESP32-C6** y soluciona un conflicto de hardware crítico entre el puerto I2C por defecto y el puerto Serie USB.

## 🚀 Características Clave

* **Control Dual de Ejes:** Envía datos de control idénticos a dos direcciones MAC de receptores simultáneamente.
* **Lógica "Freewheel" (Rueda Libre):** Al mantener presionado el **Botón C**, el transmisor envía datos "centrados" (joystick y acelerómetro en reposo) **solo al Receptor 2 (eje trasero)**, permitiendo que el eje trasero quede en "rueda libre" para giros cerrados, mientras el eje delantero mantiene el control.
* **Pines I2C Personalizados:** Utiliza `D4 (GPIO 22)` y `D5 (GPIO 23)` para I2C, evitando el conflicto de "calor" / USB Serial de los pines por defecto del C6.
* **Librería Mínima:** Utiliza una librería `MinimalNunchuk.h` personalizada que **no** inicializa `Wire.begin()` automáticamente, permitiendo reasignar los pines manualmente.
* **Antena Seleccionable:** Incluye la lógica para seleccionar entre la antena interna (PCB) o externa (U.FL) del XIAO C6.

---

## 🛠️ Componentes de Hardware

* **Controlador:** Seeed Studio XIAO ESP32-C6.
* **Entrada:** Un Nunchuk de Wii (original o compatible).
* **Conector:** Adaptador para Nunchuk (o cables soldados).
* **Componente Crítico (I2C):** 2x Resistencias Pull-Up de **4.7kΩ**.
* **Alimentación:** Batería LiPo de 3.7V + Condensador de 220uF-1000uF (Recomendado, ver `README` anterior).

---

## 🔌 Cableado

A diferencia de otras placas, los pines I2C del XIAO C6 requieren **resistencias pull-up externas** para funcionar de manera fiable.

| Nunchuk (Cable) | Pin XIAO (Función) | Pin Físico |
| :--- | :--- | :--- |
| **GND** (Blanco) | `GND` | `GND` |
| **+3.3V** (Rojo) | `3V3` | `3V3` |
| **SDA** (Verde) | `GPIO 23` | `D5` |
| **SCL** (Amarillo)| `GPIO 22` | `D4` |

**Cableado de las Resistencias Pull-Up (Obligatorio):**
* Conecta una resistencia de **4.7kΩ** entre el pin `D5 (SDA)` y `3V3`.
* Conecta una resistencia de **4.7kΩ** entre el pin `D4 (SCL)` y `3V3`.

---

## ⚙️ Configuración de Software

### 1. Entorno (Arduino IDE)

Asegúrate de tener instalado el gestor de placas de Espressif ESP32 y selecciona "Seeed Studio XIAO ESP32C6" como tu placa.

### 2. Librerías

Este proyecto requiere una librería de Nunchuk personalizada (`MinimalNunchuk.h`) que debes añadir a la carpeta de tu proyecto.

* `WiFi.h` (Incluida con ESP32)
* `esp_now.h` (Incluida con ESP32)
* `Wire.h` (Incluida)
* `MinimalNunchuk.h` (¡Debes añadirla manualmente a tu proyecto!)

### 3. Configuración del Transmisor

Antes de compilar, debes:

1.  **Actualizar las Direcciones MAC:** Reemplaza los valores de `receiver_mac_1` y `receiver_mac_2` con las direcciones MAC de tus dos placas receptoras.
2.  **Seleccionar Antena:** En el código, decide si quieres usar la antena interna o externa. Por defecto, `USE_EXTERNAL_ANTENNA` está **descomentado** (usa la antena externa U.FL). Comenta esa línea si quieres usar la antena interna de la PCB.

---

## 🔧 Solución de Problemas

* **"Nunchuk no detectado":** No has instalado las **resistencias pull-up de 4.7kΩ** en los pines D4 y D5. Son obligatorias.
* **El TX se reinicia con batería:** (Si usas batería) No has instalado el condensador de 220uF-1000uF entre 3.3V y GND.
* **El receptor no recibe nada:** Verifica las direcciones MAC y que ambos dispositivos estén en el mismo canal Wi-Fi (Canal 1 por defecto).
