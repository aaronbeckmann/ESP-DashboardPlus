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
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE);
    
    // ========================================
    // STAT CARDS - Display sensor values
    // ========================================
    
    StatCard* tempCard = dashboard.addStatCard("temp", "Temperature", String(temperature, 1), "°C");
    tempCard->setVariant(CardVariant::PRIMARY);
    tempCard->setTrend("up", "+0.5°C");
    
    StatCard* humidCard = dashboard.addStatCard("humidity", "Humidity", String(humidity, 0), "%");
    humidCard->setVariant(CardVariant::INFO);
    
    // ========================================
    // GAUGE CARD - Circular gauge display
    // ========================================
    
    GaugeCard* cpuGauge = dashboard.addGaugeCard("cpu", "CPU Usage", 0, 100, "%");
    cpuGauge->setValue(cpuUsage);
    cpuGauge->setThresholds(60, 85);
    
    // ========================================
    // CHART CARDS - Multiple chart types
    // ========================================
    
    // Line chart (default)
    ChartCard* tempChart = dashboard.addChartCard("temp-chart", "Temperature History", ChartType::LINE, 20);
    tempChart->setVariant(CardVariant::PRIMARY);
    
    // Area chart
    ChartCard* humidChart = dashboard.addChartCard("humid-chart", "Humidity History", ChartType::AREA, 20);
    humidChart->setVariant(CardVariant::INFO);
    
    // Bar chart
    ChartCard* barChart = dashboard.addChartCard("bar-chart", "Daily Usage", ChartType::BAR, 10);
    barChart->setVariant(CardVariant::SUCCESS);
    
    // ========================================
    // STATUS CARD - Icon + status message
    // ========================================
    
    StatusCard* wifiStatus = dashboard.addStatusCard("wifi-status", "WiFi Status", StatusIcon::WIFI);
    wifiStatus->setStatus(StatusIcon::WIFI, CardVariant::SUCCESS, "Connected", WiFi.localIP().toString());
    
    StatusCard* sysStatus = dashboard.addStatusCard("sys-status", "System Status", StatusIcon::CHECK);
    sysStatus->setStatus(StatusIcon::CHECK, CardVariant::SUCCESS, "All Systems Operational", "Uptime: 0h 0m");
    
    // ========================================
    // TOGGLE CARD - On/Off switch
    // ========================================
    
    ToggleCard* ledToggle = dashboard.addToggleCard("led-toggle", "LED Control", "Onboard LED", ledState);
    ledToggle->onChange = [](bool value) {
        ledState = value;
        digitalWrite(LED_PIN, value ? HIGH : LOW);
        Serial.printf("LED: %s\n", value ? "ON" : "OFF");
    };
    
    // ========================================
    // SLIDER CARD - Range slider
    // ========================================
    
    SliderCard* brightnessSlider = dashboard.addSliderCard("brightness", "LED Brightness", 0, 100, 5, "%");
    brightnessSlider->setValue(ledBrightness);
    brightnessSlider->onChange = [](int value) {
        ledBrightness = value;
        Serial.printf("Brightness: %d%%\n", value);
    };
    
    // ========================================
    // COLOR PICKER CARD
    // ========================================
    
    ColorPickerCard* colorPicker = dashboard.addColorPickerCard("led-color", "LED Color", ledColor);
    colorPicker->onChange = [](const String& color) {
        ledColor = color;
        Serial.printf("LED Color: %s\n", color.c_str());
    };
    
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
    
    DateCard* alarmTime = dashboard.addDateCard("alarm", "Alarm Time", true);  // Include time
    alarmTime->setCallback([](const String& value) {
        Serial.printf("Alarm set for: %s\n", value.c_str());
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
    
    // ========================================
    // OTA UPLOAD CARD - Firmware updates
    // ========================================
    
    OTACard* otaCard = dashboard.addOTACard("ota", "Firmware Update (OTA)", 4);
    otaCard->onProgress = [](size_t current, size_t total) {
        Serial.printf("OTA Progress: %d%%\n", (int)((current * 100) / total));
    };
    otaCard->onComplete = [](bool success) {
        Serial.printf("OTA %s\n", success ? "Success!" : "Failed!");
    };
    
    // ========================================
    // CONSOLE LOG CARD - Debug/Info/Warning/Error
    // ========================================
    
    ConsoleCard* consoleCard = dashboard.addConsoleCard("console", "System Console", 50);
    consoleCard->onClear = []() {
        Serial.println("Console cleared by user");
    };
    
    // Log some initial messages
    dashboard.logInfo("console", "ESP-DashboardPlus initialized successfully");
    dashboard.logDebug("console", "WebSocket server started on port 80");
    
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
        dashboard.updateChartCard("temp-chart", temperature);
        dashboard.updateChartCard("humid-chart", humidity);
        dashboard.updateChartCard("bar-chart", random(20, 80));
        
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
                dashboard.logDebug("console", "Sensor data collected: T=" + String(temperature, 1) + "C");
                break;
            case 1:
                dashboard.logInfo("console", "System heartbeat #" + String(logCounter));
                break;
            case 2:
                if (cpuUsage > 70) {
                    dashboard.logWarning("console", "High CPU usage detected: " + String(cpuUsage) + "%");
                } else {
                    dashboard.logInfo("console", "CPU usage normal: " + String(cpuUsage) + "%");
                }
                break;
            case 3:
                if (temperature > 30) {
                    dashboard.logError("console", "Temperature threshold exceeded: " + String(temperature, 1) + "C");
                } else {
                    dashboard.logDebug("console", "Temperature within limits");
                }
                break;
        }
    }
    
    delay(10);
}
