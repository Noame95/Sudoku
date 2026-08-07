#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "secrets.h"


struct WiFiConfig {
    const char* name;
    const char* password;
};

struct HttpConfig {
    const char* host;
    uint16_t port;
    String baseUrl;
};

unsigned long lastWifi = 0;
HTTPClient http;
WiFiClientSecure secureClient;
const int SIZE = 9;

struct HttpPaths {
    const char* generate;
    const char* solve;
    const char* easy;
    const char* medium;
    const char* hard;
    const char* controller;
    const char* keypad;
};

unsigned long lastJoystickMove = 0;
constexpr unsigned long JOYSTICK_COOLDOWN = 250;
struct JoystickConfig {
    const int xPin;
    const int yPin;
    const int buttonPin;
};

constexpr int KEYPAD_SIZE = 4;
struct NumberBoardConfig {
    const int rowPins[KEYPAD_SIZE];
    const int columnPins[KEYPAD_SIZE];
    const char keys[KEYPAD_SIZE][KEYPAD_SIZE];
};

WiFiConfig wifiConfig = {
    .name = WIFI_NAME,
    .password = WIFI_PASSWORD
};

HttpConfig httpConfig = {
    .host = BACKEND_HOST,
    .port = 443,
    .baseUrl = String("https://") + BACKEND_HOST
};

HttpPaths httpPaths = {
    .generate = "generate",
    .solve = "solve",
    .easy = "easy",
    .medium = "medium",
    .hard = "hard",
    .controller =  "controller",
    .keypad =  "keypad"
};

JoystickConfig joystickConfig = {
    .xPin = 34,
    .yPin = 35,
    .buttonPin = 32
};

NumberBoardConfig numberBoardConfig = {
    .rowPins = {26, 25, 27, 14},
    .columnPins = {13, 23, 22, 21},
    .keys = {
            {'1', '2', '3', 'A'},
            {'4', '5', '6', 'B'},
            {'7', '8', '9', 'C'},
            {'*', '0', '#', 'D'}
    }
};

void wifiConnected() {
    while (WiFiClass::status() != WL_CONNECTED) {
        const unsigned long currentTime = millis();
        if (currentTime - lastWifi >= 3000) {
            Serial.println("Could not connect to WiFi!");
            lastWifi = currentTime;
        }
    } Serial.println("Connected!");
}

String requestBoard(const char* action, const char* difficulty = "", const String& payload = "") {
    String url = httpConfig.baseUrl + "/" + action;
    if (String(difficulty) != "") {url += "/" + String(difficulty);}
    const bool initialized = http.begin(secureClient, url);
    if (String(payload) != "") {http.addHeader("Content-Type", "application/json");}

    if (!initialized) {
        Serial.println("HTTP FAILED!");
        return "";
    }
    int status;
    if (String(action) == httpPaths.generate) {status = http.POST("");}
    else if (String(action) == httpPaths.solve) {status = http.POST(payload);}
    else {
        Serial.println("INVALID HTTP ACTION!");
        http.end();
        return "";
    }
    Serial.println(status);
    String response = "";
    if (status == HTTP_CODE_OK) {response = http.getString();}
    http.end();
    return response;
}

String boardToJson(const int board[SIZE][SIZE]) {
    String json = "{\"board\":[";
    for (int i = 0; i < SIZE; i++) {
        json += "[";
        for (int j = 0; j < SIZE; j++) {
            json += String(board[i][j]);
            if (j < SIZE - 1) {json += ",";}
        } json += "]";
        if (i < SIZE - 1) {json += ",";}
    } json += "]}";
    return json;
}

const char* getJoystickDirection() {
    const unsigned long currentTime = millis();
    if (currentTime - lastJoystickMove < JOYSTICK_COOLDOWN) {return "";}

    const int xValue = analogRead(joystickConfig.xPin);
    const int yValue = analogRead(joystickConfig.yPin);
    const int pressed = digitalRead(joystickConfig.buttonPin);
    const char* direction = "";

    if (yValue <= 100) {direction = "left";}
    else if (yValue >= 4000) {direction = "right";}
    else if (xValue <= 100) {direction = "down";}
    else if (xValue >= 4000) {direction = "up";}
    else if (pressed == 0) {direction = "pressed";}
    if (direction[0] != '\0') {
        lastJoystickMove = currentTime;
    } return direction;
}

char readNumberBoard() {
    for (int row = 0; row < KEYPAD_SIZE; row++) {
        digitalWrite(numberBoardConfig.rowPins[row], LOW);
            for (int column = 0; column < KEYPAD_SIZE; column++) {
                if (digitalRead(numberBoardConfig.columnPins[column]) == LOW) {
                    const char key = numberBoardConfig.keys[row][column];
                    digitalWrite(numberBoardConfig.rowPins[row], HIGH);
                    return key;
                }
            }
        digitalWrite(numberBoardConfig.rowPins[row], HIGH);
    } return '\0';
}

bool sendDeviceRequest(const String& request, const char* endpoint) {
    const String url = httpConfig.baseUrl + "/" + endpoint + "/" + request;
    Serial.println(url);
    const bool initialized = http.begin(secureClient, url);
    if (!initialized) {
        Serial.println("HTTP initialization failed");
        return false;
    } const int status = http.POST("");
    Serial.print("HTTP status: ");
    if (status < 0) {
        Serial.print("HTTP error: ");
        Serial.println(HTTPClient::errorToString(status));
    } Serial.println(status);
    http.end();
    return status == HTTP_CODE_OK;
}


void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 READY");
    WiFi.begin(wifiConfig.name, wifiConfig.password);
    wifiConnected();
    secureClient.setInsecure();
    pinMode(joystickConfig.buttonPin, INPUT_PULLUP);
    for (int row = 0; row < KEYPAD_SIZE; row++) {
        pinMode(numberBoardConfig.rowPins[row], OUTPUT);
        digitalWrite(numberBoardConfig.rowPins[row], HIGH);
    }
    for (int column = 0; column < KEYPAD_SIZE; column++) {
        pinMode(numberBoardConfig.columnPins[column], INPUT_PULLUP);
    }
}

void loop() {
    const char* direction = getJoystickDirection();
    if (direction[0] != '\0') {
        Serial.println(direction);
        sendDeviceRequest(direction, httpPaths.controller);
    } const char key = readNumberBoard();
    if (key != '\0') {
        Serial.println(key);
        sendDeviceRequest(String(key), httpPaths.keypad);
    }
}
