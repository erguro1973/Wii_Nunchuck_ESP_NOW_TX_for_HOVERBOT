// --- Transmisor 4x4 (Nunchuk) para 2x RX Hoverboard ---
// --- VERSIÓN "LECCIONES APRENDIDAS" PARA XIAO ESP32-C6 ---
// Placa: XIAO ESP32-C6
// I2C: Pines "mágicos" 22/23 (físicamente D4/D5) + PULL-UPS EXTERNAS
// Librería: "MinimalNunchuk.h" (nuestra) para evitar conflicto de calor.
// Antena: Lógica de pines 3/14, configurada para antena INTERNA.
// Lógica: Botón 'C' activa "Freewheel" en el Eje Trasero (RX2).

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Wire.h>
#include "MinimalNunchuk.h" // <-- Nuestra "mini-librería"

// --- Configuración I2C (Placa XIAO ESP32-C6 en pines D4/D5) ---
#define I2C_SDA_PIN 23 // Pin D5 (GPIO 23) cable verde
#define I2C_SCL_PIN 22 // Pin D4 (GPIO 22) cable amarillo

// --- Configuración de Antena (Específica de XIAO C6) ---
#define RF_SWITCH_ENABLE_PIN 3  // GPIO 3: Pin para habilitar el control del switch
#define ANTENNA_SELECT_PIN 14 // GPIO 14: Pin para controlar el RF switch
// Coméntala para usar la antena INTERNA (PCB)
#define USE_EXTERNAL_ANTENNA // <-- Comentado para usar antena INTERNA

// --- Canal ESP-NOW ---
#define ESP_NOW_CHANNEL 1

// --- Direcciones MAC de los Receptores (Hoverboards) ---
// ¡DEBES OBTENER ESTAS MACs DEL MONITOR SERIE DE TUS RECEPTORES!
uint8_t receiver_mac_1[] = {0x68, 0xfe, 0x71, 0x16, 0xaa, 0x88}; // <-- MAC del receptor 1
uint8_t receiver_mac_2[] = {0x68, 0xfe, 0x71, 0x16, 0x82, 0x38}; // <-- MAC del receptor 2

// --- Estructura de Datos (Igual que en el RX) ---
typedef struct {
  int x; // Joystick X (0-255)
  int y; // Joystick Y (0-255)
  bool c; // Botón C
  bool z; // Botón Z
  int accX; // Acelerómetro X
  int accY; // Acelerómetro Y
  int accZ; // Acelerómetro Z
} NunchukData;

NunchukData myData_1; // Paquete para el eje delantero
NunchukData myData_2; // Paquete para el eje trasero

// --- Objetos ---
MinimalNunchuk nunchuk; // <-- Objeto de nuestra nueva librería
esp_now_peer_info_t peerInfo_1;
esp_now_peer_info_t peerInfo_2;

// --- Callback de Envío (CORREGIDA) ---
// La nueva firma (según tu compilador) usa 'const wifi_tx_info_t*'
void OnDataSent(const wifi_tx_info_t *wifi_info, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("Error al enviar el paquete.");
  }
}
  
void setup() {
  Serial.begin(115200);
  delay(1000); 
  Serial.println("Iniciando Transmisor 4x4 (XIAO C6, I2C en D4/D5)...");
  Serial.println("Usando Mini-Librería Personalizada.");

  // --- Selección de Antena (Específica de XIAO C6) ---
  pinMode(RF_SWITCH_ENABLE_PIN, OUTPUT);
  digitalWrite(RF_SWITCH_ENABLE_PIN, LOW); 
  pinMode(ANTENNA_SELECT_PIN, OUTPUT);
#ifdef USE_EXTERNAL_ANTENNA
  digitalWrite(ANTENNA_SELECT_PIN, HIGH); 
  Serial.println("Usando antena EXTERNA.");
#else
  digitalWrite(ANTENNA_SELECT_PIN, LOW); 
  Serial.println("Usando antena INTERNA.");
#endif
  delay(10); 
  
  // --- Inicializa I2C en los pines que funcionan (22 y 23) ---
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); 
  
  // --- Inicializa Nunchuk (Nuestra librería NO llama a Wire.begin()) ---
  if (!nunchuk.begin()) {
    Serial.println("¡Error: Nunchuk no detectado en D4/D5!");
    Serial.println("(Asegúrate de tener las resistencias pull-up externas)");
  } else {
    Serial.println("Nunchuk inicializado.");
  }
  
  // Inicializa Wi-Fi y ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_now_register_send_cb(OnDataSent); // <-- Ahora coincide
  
  // --- Registra el Peer 1 ---
  memcpy(peerInfo_1.peer_addr, receiver_mac_1, 6);
  peerInfo_1.channel = ESP_NOW_CHANNEL;  
  peerInfo_1.encrypt = false;
  if (esp_now_add_peer(&peerInfo_1) != ESP_OK){
    Serial.println("Fallo al añadir el peer 1");
    return;
  }

  // --- Registra el Peer 2 ---
  memcpy(peerInfo_2.peer_addr, receiver_mac_2, 6);
  peerInfo_2.channel = ESP_NOW_CHANNEL;  
  peerInfo_2.encrypt = false;
  if (esp_now_add_peer(&peerInfo_2) != ESP_OK){
    Serial.println("Fallo al añadir el peer 2");
    return;
  }
  
  Serial.println("Transmisor Nunchuk 4x4 listo. Enviando a 2 MACs...");
}
  
void loop() {
  
  if (!nunchuk.update()) {
    Serial.println("Nunchuk desconectado!");
    delay(100);
    return; 
  }
    
  // --- Lógica de envío 4x4 (con Freewheel) ---
  
  // 1. Rellena el paquete para el Receptor 1 (Eje Delantero)
  myData_1.x = nunchuk.joyX;
  myData_1.y = nunchuk.joyY;
  myData_1.c = nunchuk.buttonC;
  myData_1.z = nunchuk.buttonZ;
  myData_1.accX = nunchuk.accelX;
  myData_1.accY = nunchuk.accelY;
  myData_1.accZ = nunchuk.accelZ;

  // 2. Rellena el paquete para el Receptor 2 (Eje Trasero)
  myData_2.c = nunchuk.buttonC;
  myData_2.z = nunchuk.buttonZ;
  
  // Lógica de "Freewheel"
  if (nunchuk.buttonC == true) {
    // BOTÓN C PRESIONADO: Poner el eje trasero en "freewheel"
    myData_2.x = 128; // Joystick X centrado
    myData_2.y = 128; // Joystick Y centrado
    myData_2.accX = 512; // Acelerómetro centrado (aprox)
    myData_2.accY = 512; // Acelerómetro centrado (aprox)
    myData_2.accZ = 512; // Acelerómetro centrado (aprox)
  } else {
    // BOTÓN C SUELTO: Control normal 4x4
    myData_2.x = nunchuk.joyX;
    myData_2.y = nunchuk.joyY;
    myData_2.accX = nunchuk.accelX;
    myData_2.accY = nunchuk.accelY;
    myData_2.accZ = nunchuk.accelZ;
  }
  
  // 3. Envía los dos paquetes (que ahora pueden ser diferentes)
  esp_now_send(receiver_mac_1, (uint8_t *) &myData_1, sizeof(myData_1));
  esp_now_send(receiver_mac_2, (uint8_t *) &myData_2, sizeof(myData_2));
  
  delay(20); 
}
