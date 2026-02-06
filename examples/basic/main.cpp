/**
 * ESP Dashboard Library - Complete Example
 * 
 * This example demonstrates all card types and features
 * of the ESP Dashboard library.
 * 
 * Build with PlatformIO:
 *   pio run -t upload
 * 
 * The html_to_header_pio.py script will automatically convert
 * dashboard.html to dashboard_html.h before building.
 * 
 * For more examples, see:
 *   https://aaronbeckmann.github.io/ESP-DashboardPlus/examples
 */

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ESPDashboardPlus.h>
#include <dashboard_html.h>  // Auto-generated from extras/dashboard.html

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Create AsyncWebServer on port 80
AsyncWebServer server(80);

// Create Dashboard instance
ESPDashboardPlus dashboard("My ESP32 Device");

// Simulated sensor values
float temperature = 23.5;
float humidity = 45.0;
int cpuUsage = 30;
bool ledState = false;
int ledBrightness = 75;
String ledColor = "#00D4AA";
String wifiMode = "sta";
String selectedTimezone = "";

// Pin definitions
const int LED_PIN = 2;

void setup() {
    Serial.begin(115200);
    
    // Initialize LED
    pinMode(LED_PIN, OUTPUT);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
    
    // Initialize dashboard with PROGMEM HTML
    // Parameters: server, htmlData, htmlSize, enableOTA, enableConsole
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE, true, true);
    
    // Set dashboard title and subtitle (displayed in browser)
    dashboard.setTitle("My ESP32 Device", "Home Automation Hub");
    
    // Set firmware version info (displayed in OTA tab)
    dashboard.setVersionInfo("1.0.0", "Never");
    
    // Set global command handler (for Console tab input)
    dashboard.onCommand([](const String& command) {
        Serial.printf("Console command received: %s\n", command.c_str());
        
        // Handle commands from the Console tab
        if (command == "help") {
            dashboard.logInfo("Available commands: help, status, reboot, version");
        } else if (command == "status") {
            dashboard.logInfo("System OK - Temp: " + String(temperature, 1) + "C, CPU: " + String(cpuUsage) + "%");
        } else if (command == "version") {
            dashboard.logInfo("ESP-DashboardPlus v1.0.0");
        } else if (command == "reboot") {
            dashboard.logWarning("Rebooting in 3 seconds...");
            delay(3000);
            ESP.restart();
        } else {
            dashboard.logWarning("Unknown command: " + command);
        }
    });
    
    // ========================================
    // CARD GROUPS - Organize cards into sections
    // ========================================
    
    // Create groups for organizing cards
    dashboard.addGroup("sensors", "Sensor Readings");
    dashboard.addGroup("charts", "Live Data");
    dashboard.addGroup("controls", "Device Controls");
    dashboard.addGroup("config", "Configuration");
    dashboard.addGroup("actions", "System Actions");
    
    // ========================================
    // STAT CARDS - Display sensor values
    // Cards are ordered by weight within groups (lower = first)
    // ========================================
    
    StatCard* tempCard = dashboard.addStatCard("temp", "Temperature", String(temperature, 1), "°C");
    tempCard->setVariant(CardVariant::PRIMARY);
    tempCard->setTrend("up", "+0.5°C");
    tempCard->setWeight(10);  // Appears first in sensors group
    dashboard.addCardToGroup("sensors", "temp");
    
    StatCard* humidCard = dashboard.addStatCard("humidity", "Humidity", String(humidity, 0), "%");
    humidCard->setVariant(CardVariant::INFO);
    humidCard->setWeight(20);  // Appears second in sensors group
    dashboard.addCardToGroup("sensors", "humidity");
    
    // ========================================
    // GAUGE CARD - Circular gauge display
    // ========================================
    
    GaugeCard* cpuGauge = dashboard.addGaugeCard("cpu", "CPU Usage", 0, 100, "%");
    cpuGauge->setValue(cpuUsage);
    cpuGauge->setThresholds(60, 85);
    cpuGauge->setWeight(30);  // Appears third in sensors group
    dashboard.addCardToGroup("sensors", "cpu");
    
    // ========================================
    // CHART CARDS - Multiple chart types
    // Cards can span multiple grid cells using setSize(x, y)
    // ========================================
    
    // Multi-series line chart (Temperature + Humidity on same chart)
    // This chart spans 2 columns for a wider view
    ChartCard* multiChart = dashboard.addChartCard("multi-chart", "Temp & Humidity", ChartType::LINE, 20);
    int tempSeriesIdx = multiChart->addSeries("Temperature", "primary");
    int humidSeriesIdx = multiChart->addSeries("Humidity", "info");
    multiChart->setWeight(10);  // First chart
    multiChart->setSize(2, 1);  // Span 2 columns, 1 row
    dashboard.addCardToGroup("charts", "multi-chart");
    
    // Single-series area chart (backwards compatible)
    ChartCard* cpuChart = dashboard.addChartCard("cpu-chart", "CPU History", ChartType::AREA, 20);
    cpuChart->setVariant(CardVariant::WARNING);
    cpuChart->setWeight(20);  // Second chart
    dashboard.addCardToGroup("charts", "cpu-chart");
    
    // Multi-series bar chart - spans 2x2 for larger display
    ChartCard* barChart = dashboard.addChartCard("bar-chart", "Daily Usage", ChartType::BAR, 10);
    int readSeriesIdx = barChart->addSeries("Reads", "success");
    int writeSeriesIdx = barChart->addSeries("Writes", "warning");
    barChart->setWeight(30);  // Third chart
    barChart->setSize(2, 2);  // Span 2 columns, 2 rows for larger display
    dashboard.addCardToGroup("charts", "bar-chart");
    
    // ========================================
    // STATUS CARD - Icon + status message
    // ========================================
    
    StatusCard* wifiStatus = dashboard.addStatusCard("wifi-status", "WiFi Status", StatusIcon::WIFI);
    wifiStatus->setStatus(StatusIcon::WIFI, CardVariant::SUCCESS, "Connected", WiFi.localIP().toString());
    wifiStatus->setWeight(40);  // After sensors
    dashboard.addCardToGroup("sensors", "wifi-status");
    
    StatusCard* sysStatus = dashboard.addStatusCard("sys-status", "System Status", StatusIcon::CHECK);
    sysStatus->setStatus(StatusIcon::CHECK, CardVariant::SUCCESS, "All Systems Operational", "Uptime: 0h 0m");
    sysStatus->setWeight(50);  // Last in sensors group
    dashboard.addCardToGroup("sensors", "sys-status");
    
    // ========================================
    // TOGGLE CARD - On/Off switch
    // ========================================
    
    ToggleCard* ledToggle = dashboard.addToggleCard("led-toggle", "LED Control", "Onboard LED", ledState);
    ledToggle->onChange = [](bool value) {
        ledState = value;
        digitalWrite(LED_PIN, value ? HIGH : LOW);
        Serial.printf("LED: %s\n", value ? "ON" : "OFF");
    };
    dashboard.addCardToGroup("controls", "led-toggle");
    
    // ========================================
    // SLIDER CARD - Range slider
    // ========================================
    
    SliderCard* brightnessSlider = dashboard.addSliderCard("brightness", "LED Brightness", 0, 100, 5, "%");
    brightnessSlider->setValue(ledBrightness);
    brightnessSlider->onChange = [](int value) {
        ledBrightness = value;
        Serial.printf("Brightness: %d%%\n", value);
    };
    dashboard.addCardToGroup("controls", "brightness");
    
    // ========================================
    // COLOR PICKER CARD
    // ========================================
    
    ColorPickerCard* colorPicker = dashboard.addColorPickerCard("led-color", "LED Color", ledColor);
    colorPicker->onChange = [](const String& color) {
        ledColor = color;
        Serial.printf("LED Color: %s\n", color.c_str());
    };
    dashboard.addCardToGroup("controls", "led-color");
    
    // ========================================
    // INPUT CARDS - Text and number input
    // ========================================
    
    // Text input
    InputCard* ssidInput = dashboard.addInputCard("wifi-ssid", "WiFi SSID", "Enter network name...");
    ssidInput->onSubmit = [](const String& value) {
        Serial.printf("New SSID: %s\n", value.c_str());
    };
    
    // Number input
    InputCard* intervalInput = dashboard.addInputCard("update-interval", "Update Interval", "1000");
    intervalInput->setNumberInput(100, 60000, 100, "ms");
    intervalInput->onSubmit = [](const String& value) {
        Serial.printf("Update interval: %s ms\n", value.c_str());
    };
    
    // ========================================
    // DROPDOWN CARD - Select menu
    // ========================================
    
    DropdownCardImpl* modeDropdown = dashboard.addDropdownCard("wifi-mode", "WiFi Mode", "Select mode...");
    modeDropdown->addOption("sta", "Station (Client)");
    modeDropdown->addOption("ap", "Access Point");
    modeDropdown->addOption("apsta", "AP + Station");
    modeDropdown->setValue(wifiMode);
    modeDropdown->onChange = [](const String& value) {
        wifiMode = value;
        Serial.printf("WiFi Mode: %s\n", value.c_str());
    };
    
    // ========================================
    // DATE CARD - Date/DateTime picker
    // ========================================
    
    DateCard* scheduleDate = dashboard.addDateCard("schedule", "Schedule Date", false);
    scheduleDate->setCallback([](const String& value) {
        Serial.printf("Selected date: %s\n", value.c_str());
    });
    
    DateCard* alarmTime = dashboard.addDateCard("alarm", "Alarm DateTime", true);  // Include time
    alarmTime->setCallback([](const String& value) {
        Serial.printf("Alarm set for: %s\n", value.c_str());
    });
    
    // ========================================
    // TIME CARD - Time picker (HH:MM or HH:MM:SS)
    // ========================================
    
    TimeCard* wakeTime = dashboard.addTimeCard("wake-time", "Wake Time", false);
    wakeTime->setCallback([](const String& value) {
        Serial.printf("Wake time set: %s\n", value.c_str());
    });
    
    TimeCard* preciseTime = dashboard.addTimeCard("precise-time", "Precise Time", true);  // Include seconds
    preciseTime->setCallback([](const String& value) {
        Serial.printf("Precise time: %s\n", value.c_str());
    });
    
    // ========================================
    // TIMEZONE CARD - Browser timezone detection
    // ========================================
    
    TimezoneCard* tzCard = dashboard.addTimezoneCard("timezone", "Browser Timezone", "Detect Timezone");
    tzCard->setCallback([](const String& tz, int offset, const String& offsetStr) {
        selectedTimezone = tz;
        Serial.printf("Timezone: %s (offset: %d min, %s)\n", tz.c_str(), offset, offsetStr.c_str());
    });
    
    // ========================================
    // LOCATION CARD - Browser geolocation
    // ========================================
    
    LocationCard* locCard = dashboard.addLocationCard("location", "Device Location", "Get Current Location");
    locCard->setCallback([](float lat, float lon) {
        Serial.printf("Location received: %.6f, %.6f\n", lat, lon);
    });
    
    // ========================================
    // BUTTON CARDS - Simple action buttons
    // ========================================
    
    ButtonCardImpl* saveBtn = dashboard.addButtonCard("save", "Settings", "Save Configuration", []() {
        Serial.println("Save button clicked!");
    });
    saveBtn->setVariant(CardVariant::PRIMARY);
    
    // ========================================
    // LINK CARDS - URL redirect buttons
    // ========================================
    
    LinkCard* docsLink = dashboard.addLinkCard("docs", "Documentation", "View Docs", "https://github.com/aaronbeckmann/ESP-DashboardPlus/docs");
    docsLink->setVariant(CardVariant::INFO);
    
    LinkCard* githubLink = dashboard.addLinkCard("github", "Source Code", "GitHub", "https://github.com/aaronbeckmann/ESP-DashboardPlus");
    githubLink->setTarget("_blank");
    
    // ========================================
    // ACTION BUTTONS - With confirmation popup
    // ========================================
    
    ActionButton* restartBtn = dashboard.addActionButton(
        "restart", 
        "Device Control", 
        "Restart Device",
        "Restart Device?",
        "The device will restart and temporarily lose connection.",
        []() {
            Serial.println("Restarting device...");
            delay(1000);
            ESP.restart();
        }
    );
    restartBtn->setVariant(CardVariant::WARNING);
    
    ActionButton* resetBtn = dashboard.addActionButton(
        "factory-reset",
        "Danger Zone",
        "Factory Reset",
        "Factory Reset?",
        "This will erase all settings and restore factory defaults. This action cannot be undone.",
        []() {
            Serial.println("Factory reset initiated!");
            delay(1000);
            ESP.restart();
        }
    );
    resetBtn->setVariant(CardVariant::DANGER);
    
    // Log some initial messages
    dashboard.logInfo("ESP-DashboardPlus initialized successfully");
    dashboard.logDebug("WebSocket server started on port 80");
    
    // Also log to the global Console tab (no card required)
    dashboard.logInfo("Dashboard started - OTA and Console tabs enabled");
    
    // Start server
    server.begin();
    Serial.println("ESP-DashboardPlus ready!");
    Serial.printf("Open http://%s in your browser\n", WiFi.localIP().toString().c_str());
}

void loop() {
    // Process WebSocket events
    dashboard.loop();
    
    // Simulate sensor readings (replace with actual sensor code)
    static unsigned long lastUpdate = 0;
    static unsigned long lastLogUpdate = 0;
    static unsigned long startTime = millis();
    static int logCounter = 0;
    
    if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();
        
        // Simulate temperature fluctuation
        temperature += random(-10, 11) / 10.0;
        temperature = constrain(temperature, 15.0, 35.0);
        
        // Simulate humidity fluctuation
        humidity += random(-5, 6);
        humidity = constrain(humidity, 30.0, 70.0);
        
        // Simulate CPU usage
        cpuUsage += random(-10, 11);
        cpuUsage = constrain(cpuUsage, 10, 90);
        
        // Update dashboard cards
        dashboard.updateStatCard("temp", String(temperature, 1));
        dashboard.updateStatCard("humidity", String(humidity, 0));
        dashboard.updateGaugeCard("cpu", cpuUsage);
        
        // Update multi-series chart (temp & humidity on same chart)
        dashboard.updateChartCard("multi-chart", 0, temperature);   // Series 0: Temperature
        dashboard.updateChartCard("multi-chart", 1, humidity);      // Series 1: Humidity
        
        // Update single-series chart (backwards compatible)
        dashboard.updateChartCard("cpu-chart", cpuUsage);
        
        // Update multi-series bar chart
        dashboard.updateChartCard("bar-chart", 0, random(20, 60));  // Reads
        dashboard.updateChartCard("bar-chart", 1, random(10, 40));  // Writes
        
        // Update system status with uptime
        unsigned long uptime = (millis() - startTime) / 1000;
        int hours = uptime / 3600;
        int minutes = (uptime % 3600) / 60;
        String uptimeStr = "Uptime: " + String(hours) + "h " + String(minutes) + "m";
        dashboard.updateStatusCard("sys-status", StatusIcon::CHECK, CardVariant::SUCCESS, 
                                   "All Systems Operational", uptimeStr);
        
        Serial.printf("Temp: %.1f°C, Humidity: %.0f%%, CPU: %d%%\n", 
                      temperature, humidity, cpuUsage);
    }
    
    // Periodically log messages to console (every 5 seconds)
    if (millis() - lastLogUpdate > 5000) {
        lastLogUpdate = millis();
        logCounter++;
        
        // Cycle through different log levels
        switch (logCounter % 4) {
            case 0:
                dashboard.logDebug("Sensor data collected: T=" + String(temperature, 1) + "C");
                break;
            case 1:
                dashboard.logInfo("System heartbeat #" + String(logCounter));
                break;
            case 2:
                if (cpuUsage > 70) {
                    dashboard.logWarning("High CPU usage detected: " + String(cpuUsage) + "%");
                } else {
                    dashboard.logInfo("CPU usage normal: " + String(cpuUsage) + "%");
                }
                break;
            case 3:
                if (temperature > 30) {
                    dashboard.logError("Temperature threshold exceeded: " + String(temperature, 1) + "C");
                } else {
                    dashboard.logDebug("Temperature within limits");
                }
                break;
        }
    }
    
    delay(10);
}
