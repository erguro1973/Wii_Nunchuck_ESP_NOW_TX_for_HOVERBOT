/*
 * MinimalNunchuk.h
 * Una "mini-librería" de Nunchuk diseñada para:
 * 1. Evitar conflictos de hardware con Serial en la XIAO ESP32-C6.
 * 2. NO llamar a Wire.begin() internamente.
 * 3. Leer todos los datos: Joystick, Botones y Acelerómetro.
 */

#pragma once // Evita que el archivo se incluya varias veces
#include <Arduino.h>
#include <Wire.h>

#define NUNCHUK_ADDRESS 0x52

class MinimalNunchuk {
public:
    // Variables públicas para acceder a los datos
    int joyX;
    int joyY;
    bool buttonC;
    bool buttonZ;
    int accelX;
    int accelY;
    int accelZ;

    MinimalNunchuk() {
        // Constructor vacío
    }

    // Inicializa el Nunchuk (envía el handshake)
    bool begin() {
        // Enviar el "handshake" de inicialización
        Wire.beginTransmission(NUNCHUK_ADDRESS);
        Wire.write(0xF0);
        Wire.write(0x55);
        if (Wire.endTransmission() != 0) return false; // Falla si no hay ACK

        delay(10);

        Wire.beginTransmission(NUNCHUK_ADDRESS);
        Wire.write(0xFB);
        Wire.write(0x00);
        if (Wire.endTransmission() != 0) return false; // Falla si no hay ACK

        delay(10);
        return true; // Éxito
    }

    // Solicita y lee los 6 bytes de datos del Nunchuk
    bool update() {
        uint8_t data[6]; // Buffer para los datos

        // 1. Solicitar los datos
        Wire.beginTransmission(NUNCHUK_ADDRESS);
        Wire.write(0x00);
        if (Wire.endTransmission() != 0) {
            return false; // Falla si no hay ACK
        }

        delayMicroseconds(100); // Pequeña pausa

        // 2. Leer los 6 bytes
        if (Wire.requestFrom(NUNCHUK_ADDRESS, 6) != 6) {
            return false; // Falla si no se reciben 6 bytes
        }

        for (int i = 0; i < 6; i++) {
            data[i] = Wire.read();
        }

        // 3. Decodificar los datos
        joyX = data[0];
        joyY = data[1];
        accelX = (int)(data[2] << 2) | ((data[5] >> 2) & 0x03);
        accelY = (int)(data[3] << 2) | ((data[5] >> 4) & 0x03);
        accelZ = (int)(data[4] << 2) | ((data[5] >> 6) & 0x03);
        buttonZ = !((data[5] >> 0) & 0x01);
        buttonC = !((data[5] >> 1) & 0x01);

        return true; // Éxito
    }
};
