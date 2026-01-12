---
layout: default
title: Examples
nav_order: 5
---

# Examples

Practical examples showing ESP DashboardPlus in action.

---

## Basic Example

A minimal example showing core functionality.

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

AsyncWebServer server(80);
ESPDashboardPlus dashboard("Basic Example");

void setup() {
    Serial.begin(115200);
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE);
    
    // Temperature display
    dashboard.addStatCard("temp", "Temperature", "25.0", "°C");
    
    // LED toggle
    ToggleCard* led = dashboard.addToggleCard("led", "LED", "Onboard LED", false);
    led->onChange = [](bool state) {
        digitalWrite(LED_BUILTIN, state);
    };
    
    server.begin();
}

void loop() {
    dashboard.loop();
    
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();
        float temp = 20.0 + random(0, 100) / 10.0;
        dashboard.updateStatCard("temp", String(temp, 1));
    }
}
```

---

## Sensor Monitoring with Multi-Series Charts

Monitor multiple sensors with multi-line charts, card groups, and custom ordering.

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

AsyncWebServer server(80);
ESPDashboardPlus dashboard("Sensor Monitor");

float temperature = 25.0;
float humidity = 50.0;
int cpuUsage = 30;

void setup() {
    Serial.begin(115200);
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE, true, true);
    
    // === SENSORS GROUP (weight 1 = shown first) ===
    StatCard* tempCard = dashboard.addStatCard("temp", "Temperature", "25.0", "°C");
    tempCard->setVariant(CardVariant::PRIMARY);
    tempCard->setGroup("Sensors");
    tempCard->setWeight(1);
    
    StatCard* humidCard = dashboard.addStatCard("humid", "Humidity", "50", "%");
    humidCard->setVariant(CardVariant::INFO);
    humidCard->setGroup("Sensors");
    humidCard->setWeight(2);
    
    GaugeCard* cpuGauge = dashboard.addGaugeCard("cpu", "CPU Usage", 0, 100, "%");
    cpuGauge->setThresholds(60, 85);
    cpuGauge->setGroup("Sensors");
    cpuGauge->setWeight(3);
    
    // === CHARTS GROUP (weight 2 = shown second) ===
    // Multi-series chart with temperature AND humidity
    ChartCard* envChart = dashboard.addChartCard("env-chart", "Environment", ChartType::LINE, 30);
    envChart->setGroup("Charts");
    envChart->setWeight(1);
    envChart->setSize(2, 1);  // Span 2 columns
    envChart->addSeries("temp", "Temperature", "#FF6B6B");
    envChart->addSeries("humid", "Humidity", "#4ECDC4");
    
    // Single-series CPU chart
    ChartCard* cpuChart = dashboard.addChartCard("cpu-chart", "CPU History", ChartType::AREA, 30);
    cpuChart->setGroup("Charts");
    cpuChart->setWeight(2);
    
    // === STATUS GROUP ===
    StatusCard* status = dashboard.addStatusCard("status", "System Status", StatusIcon::CHECK);
    status->setStatus(StatusIcon::CHECK, CardVariant::SUCCESS, "All Systems OK", "Last update: now");
    status->setGroup("Status");
    
    dashboard.logInfo("Sensor monitoring started");
    server.begin();
}

void loop() {
    dashboard.loop();
    
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();
        
        temperature += random(-5, 6) / 10.0;
        temperature = constrain(temperature, 15, 35);
        humidity += random(-2, 3);
        humidity = constrain(humidity, 30, 70);
        cpuUsage += random(-5, 6);
        cpuUsage = constrain(cpuUsage, 10, 95);
        
        dashboard.updateStatCard("temp", String(temperature, 1));
        dashboard.updateStatCard("humid", String(humidity, 0));
        dashboard.updateGaugeCard("cpu", cpuUsage);
        
        // Update multi-series chart
        dashboard.updateChartCardSeries("env-chart", "temp", temperature);
        dashboard.updateChartCardSeries("env-chart", "humid", humidity);
        
        // Update single-series chart
        dashboard.updateChartCard("cpu-chart", cpuUsage);
        
        dashboard.logDebug("T=" + String(temperature, 1) + "°C H=" + String(humidity, 0) + "%");
    }
}
```

---

## LED Controller

Control RGB LED with color picker, brightness slider, and toggle.

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

const int LED_R = 25;
const int LED_G = 26;
const int LED_B = 27;

AsyncWebServer server(80);
ESPDashboardPlus dashboard("LED Controller");

bool ledEnabled = false;
int brightness = 100;
String currentColor = "#00D4AA";

void updateLED() {
    if (!ledEnabled) {
        analogWrite(LED_R, 0);
        analogWrite(LED_G, 0);
        analogWrite(LED_B, 0);
        return;
    }
    
    long rgb = strtol(currentColor.substring(1).c_str(), NULL, 16);
    int r = ((rgb >> 16) & 0xFF) * brightness / 100;
    int g = ((rgb >> 8) & 0xFF) * brightness / 100;
    int b = (rgb & 0xFF) * brightness / 100;
    
    analogWrite(LED_R, r);
    analogWrite(LED_G, g);
    analogWrite(LED_B, b);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE);
    
    // Power toggle
    ToggleCard* power = dashboard.addToggleCard("power", "LED Power", "Enable LED", false);
    power->onChange = [](bool state) {
        ledEnabled = state;
        updateLED();
    };
    
    // Color picker
    ColorPickerCard* color = dashboard.addColorPickerCard("color", "LED Color", "#00D4AA");
    color->onChange = [](const String& c) {
        currentColor = c;
        updateLED();
    };
    
    // Brightness slider
    SliderCard* bright = dashboard.addSliderCard("brightness", "Brightness", 0, 100, 5, "%");
    bright->setValue(100);
    bright->onChange = [](int value) {
        brightness = value;
        updateLED();
    };
    
    server.begin();
}

void loop() {
    dashboard.loop();
}
```

---

## WiFi Configuration

Configure WiFi settings via the dashboard.

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

Preferences prefs;
AsyncWebServer server(80);
ESPDashboardPlus dashboard("WiFi Config");

String savedSSID = "";
String savedPassword = "";

void setup() {
    Serial.begin(115200);
    
    // Load saved credentials
    prefs.begin("wifi", true);
    savedSSID = prefs.getString("ssid", "");
    savedPassword = prefs.getString("pass", "");
    prefs.end();
    
    // Try saved credentials or start AP
    if (savedSSID.length() > 0) {
        WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            attempts++;
        }
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.softAP("ESP32-Setup", "password123");
    }
    
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE);
    
    // Status
    StatusCard* status = dashboard.addStatusCard("status", "WiFi Status", StatusIcon::WIFI);
    if (WiFi.status() == WL_CONNECTED) {
        status->setStatus(StatusIcon::WIFI, CardVariant::SUCCESS, "Connected", WiFi.localIP().toString());
    } else {
        status->setStatus(StatusIcon::WIFI, CardVariant::WARNING, "AP Mode", "192.168.4.1");
    }
    
    // SSID input
    InputCard* ssidInput = dashboard.addInputCard("ssid", "WiFi SSID", savedSSID);
    ssidInput->setValue(savedSSID);
    
    // Password input
    InputCard* passInput = dashboard.addInputCard("password", "WiFi Password", "");
    passInput->inputType = "password";
    
    // Save button
    dashboard.addButtonCard("save", "Settings", "Save & Restart", []() {
        prefs.begin("wifi", false);
        // Get values from cards
        InputCard* ssid = static_cast<InputCard*>(dashboard.getCard("ssid"));
        InputCard* pass = static_cast<InputCard*>(dashboard.getCard("password"));
        prefs.putString("ssid", ssid->value);
        prefs.putString("pass", pass->value);
        prefs.end();
        
        delay(1000);
        ESP.restart();
    });
    
    server.begin();
}

void loop() {
    dashboard.loop();
}
```

---

## OTA Firmware Update

Enable over-the-air firmware updates using the dedicated OTA tab.

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

AsyncWebServer server(80);
ESPDashboardPlus dashboard("OTA Example");

void setup() {
    Serial.begin(115200);
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    // Enable OTA and Console tabs
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE, true, true);
    
    // Set firmware version info (displayed in OTA tab)
    dashboard.setVersionInfo("1.0.0", "2024-01-15");
    
    // Firmware info card
    StatCard* version = dashboard.addStatCard("version", "Firmware Version", "1.0.0", "");
    
    // Log to Console tab
    dashboard.logInfo("Ready for firmware update");
    dashboard.logInfo("Use the OTA tab to upload new firmware");
    
    server.begin();
}

void loop() {
    dashboard.loop();
}
```

---

## Card Sizing Example

Create larger cards that span multiple grid columns or rows.

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

AsyncWebServer server(80);
ESPDashboardPlus dashboard("Sizing Demo");

void setup() {
    Serial.begin(115200);
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE);
    
    // Wide chart (2 columns)
    ChartCard* mainChart = dashboard.addChartCard("main", "Main Metrics", ChartType::LINE, 50);
    mainChart->setSize(2, 1);
    mainChart->addSeries("a", "Series A", "#FF6B6B");
    mainChart->addSeries("b", "Series B", "#4ECDC4");
    
    // Tall gauge (2 rows)
    GaugeCard* gauge = dashboard.addGaugeCard("cpu", "CPU", 0, 100, "%");
    gauge->setSizeY(2);
    
    // Large chart (2x2)
    ChartCard* bigChart = dashboard.addChartCard("history", "Full History", ChartType::AREA, 100);
    bigChart->setSize(2, 2);
    
    // Normal sized cards
    dashboard.addStatCard("temp", "Temperature", "25.0", "°C");
    dashboard.addStatCard("humid", "Humidity", "50", "%");
    
    server.begin();
}

void loop() {
    dashboard.loop();
}
```

---

## Time and Location Cards

Use TimeCard for time selection and LocationCard for GPS/browser geolocation.

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

AsyncWebServer server(80);
ESPDashboardPlus dashboard("Time & Location Demo");

void setup() {
    Serial.begin(115200);
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE);
    
    // Time picker (HH:MM)
    TimeCard* wakeTime = dashboard.addTimeCard("wake", "Wake Time", false);
    wakeTime->setCallback([](const String& value) {
        Serial.printf("Wake time: %s\n", value.c_str());
    });
    
    // Time picker with seconds (HH:MM:SS)
    TimeCard* preciseTime = dashboard.addTimeCard("precise", "Precise Time", true);
    preciseTime->setCallback([](const String& value) {
        Serial.printf("Precise time: %s\n", value.c_str());
    });
    
    // Location picker (uses browser geolocation)
    LocationCard* locCard = dashboard.addLocationCard("location", "Device Location", "Get My Location");
    locCard->setCallback([](float lat, float lon) {
        Serial.printf("Location: %.6f, %.6f\n", lat, lon);
        // You could save this to EEPROM or use for geofencing
    });
    
    // Status card showing location
    StatusCard* locStatus = dashboard.addStatusCard("loc-status", "Location Status", StatusIcon::INFO);
    locStatus->setStatus(StatusIcon::INFO, CardVariant::PRIMARY, "Awaiting Location", "Click button to detect");
    
    server.begin();
}

void loop() {
    dashboard.loop();
}
```

---

## Complete All-Cards Example

See the `examples/basic/main.cpp` in the library for a complete example demonstrating all 14 card types, multi-series charts, card groups, ordering, and sizing.
