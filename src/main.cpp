#include <Arduino.h>
#include <WiFi.h>
#include <lilka.h>
#include "openai_client.h"
#include "ui_handler.h"
#include "serial_handler.h"
#include "settings.h"
#include "config_server.h"

// Global objects
OpenAIClient* openaiClient;
UIHandler* ui;
SerialHandler* serialHandler;
Settings* settings;

// Mode selection
enum Mode {
    MODE_NONE,
    MODE_CHAT,
    MODE_SERIAL,
    MODE_SETTINGS
};

Mode selectedMode = MODE_NONE;

// Function declarations
void connectWiFi();
void handleUserInput();
void runSerialMode();
void runSettingsMode();
void runWebConfigMode();
void runQuickSettingsMode();
void editOpenAIModel();

void setup() {
    Serial.begin(115200);
    lilka::begin();
    delay(1000);
    
    Serial.println("Lilka AI");
    Serial.println("=============================");
    
    // Display welcome message
    lilka::Canvas canvas;
    canvas.fillScreen(0);
    
    int16_t x1, y1;
    uint16_t w, h;
    
    canvas.setTextSize(3);
    canvas.setTextColor(canvas.color565(150, 200, 255));
    canvas.getTextBounds("Лілка AI", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, canvas.height() / 2 + h / 2);
    canvas.print("Лілка AI");
    
    lilka::display.drawCanvas(&canvas);
    delay(2000);
    
    // Initialize settings
    settings = new Settings();
    settings->begin();
    
    // Initialize UI
    ui = new UIHandler();
    
    // Main menu loop
    bool menuActive = true;
    while (menuActive) {
        // Create mode selection menu
        lilka::Menu menu("Лілка AI");
        menu.addItem("Chat mode", NULL, lilka::colors::Arylide_yellow);
        menu.addItem("Serial mode", NULL, lilka::colors::Arylide_yellow);
        menu.addItem("Settings", NULL, lilka::colors::Orange);
        
        // Run menu
        while (!menu.isFinished()) {
            menu.update();
            lilka::Canvas canvas;
            menu.draw(&canvas);
            lilka::display.drawCanvas(&canvas);
        }
        
        // Get selected mode
        int16_t selection = menu.getCursor();
        if (selection == 2) {
            // Settings submenu
            runSettingsMode();
            continue;
        }
        
        // Check if settings are configured before connecting
        if (!settings->isConfigured() || settings->getWifiSSID().isEmpty() || 
            settings->getWifiPassword().isEmpty() || settings->getOpenAIKey().isEmpty()) {
            lilka::Alert alert("Settings Required", "Please configure WiFi and OpenAI credentials in Settings menu.");
            alert.draw(&lilka::display);
            while (!alert.isFinished()) {
                alert.update();
            }
            // Return to menu instead of restarting
            continue;
        }
        
        // Connect to WiFi using stored credentials
        connectWiFi();
        
        // Initialize OpenAI client with stored key
        String apiKey = settings->getOpenAIKey();
        String model = settings->getOpenAIModel();
        openaiClient = new OpenAIClient(apiKey, model);
        
        // Handle mode selection
        if (selection == 0) {
            selectedMode = MODE_CHAT;
            menuActive = false;
        } else if (selection == 1) {
            selectedMode = MODE_SERIAL;
            runSerialMode();
            menuActive = false;
        }
    }
}

void loop() {
    if (selectedMode == MODE_CHAT) {
        handleUserInput();
    } else if (selectedMode == MODE_SERIAL) {
        serialHandler->loop();
    }
    delay(10);
}

void connectWiFi() {
    // Get credentials from settings
    String wifiSSID = settings->getWifiSSID();
    String wifiPassword = settings->getWifiPassword();
    
    // Display connecting status
    lilka::Canvas canvas;
    canvas.fillScreen(0);
    
    int16_t x1, y1;
    uint16_t w, h;
    
    canvas.setTextSize(2);
    canvas.setTextColor(canvas.color565(100, 200, 255));
    canvas.getTextBounds("WiFi", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 60);
    canvas.print("WiFi");
    
    canvas.setTextSize(1);
    canvas.setTextColor(canvas.color565(200, 200, 200));
    canvas.getTextBounds("Connecting to...", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 100);
    canvas.print("Connecting to...");
    
    canvas.setTextColor(canvas.color565(255, 255, 255));
    canvas.getTextBounds(wifiSSID.c_str(), 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 120);
    canvas.print(wifiSSID);
    
    lilka::display.drawCanvas(&canvas);
    
    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        attempts++;
    }
    
    // Display result
    canvas.fillScreen(0);
    
    if (WiFi.status() == WL_CONNECTED) {
        // Success screen
        canvas.setTextSize(2);
        canvas.setTextColor(canvas.color565(100, 255, 100));
        canvas.getTextBounds("Connected!", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 60);
        canvas.print("Connected!");
        
        canvas.setTextSize(1);
        canvas.setTextColor(canvas.color565(200, 200, 200));
        canvas.getTextBounds("Network:", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 100);
        canvas.print("Network:");
        
        canvas.setTextColor(canvas.color565(255, 255, 255));
        canvas.getTextBounds(wifiSSID.c_str(), 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 120);
        canvas.print(wifiSSID);
        
        canvas.setTextColor(canvas.color565(200, 200, 200));
        canvas.getTextBounds("IP Address:", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 150);
        canvas.print("IP Address:");
        
        String ipAddr = WiFi.localIP().toString();
        canvas.setTextColor(canvas.color565(255, 255, 100));
        canvas.getTextBounds(ipAddr.c_str(), 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 170);
        canvas.print(ipAddr);
        
        lilka::display.drawCanvas(&canvas);
        delay(2000);
    } else {
        // Failure screen
        canvas.setTextSize(2);
        canvas.setTextColor(canvas.color565(255, 100, 100));
        canvas.getTextBounds("Failed!", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 80);
        canvas.print("Failed!");
        
        canvas.setTextSize(1);
        canvas.setTextColor(canvas.color565(200, 200, 200));
        canvas.getTextBounds("Check credentials", 0, 0, &x1, &y1, &w, &h);
        canvas.setCursor((canvas.width() - w) / 2, 130);
        canvas.print("Check credentials");
        
        lilka::display.drawCanvas(&canvas);
        delay(3000);
    }
}

void handleUserInput() {
    String userMessage;
    
    // Get input from user
    if (ui->getInput(userMessage)) {
        Serial.println("User message: " + userMessage);
        
        // Show waiting screen
        ui->showWaitingScreen();
        
        // Send to OpenAI
        String response;
        bool success = openaiClient->sendMessage(userMessage, response);
        
        if (success) {
            Serial.println("Response: " + response);
            ui->displayResponse(response);
        } else {
            Serial.println("Error: " + response);
            ui->showError(response);
        }
    }
}

void runSerialMode() {
    // Display serial mode info on screen
    lilka::Canvas canvas;
    canvas.fillScreen(0);
    
    canvas.setTextSize(2);
    canvas.setTextColor(canvas.color565(100, 255, 100));
    
    int16_t x1, y1;
    uint16_t w, h;
    
    // Title
    canvas.getTextBounds("Serial Mode", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 60);
    canvas.print("Serial Mode");
    
    // Info
    canvas.setTextSize(1);
    canvas.setTextColor(canvas.color565(255, 255, 255));
    
    canvas.getTextBounds("Active", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 90);
    canvas.print("Active");
    
    canvas.getTextBounds("RX: GPIO 44", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 120);
    canvas.print("RX: GPIO 44");
    
    canvas.getTextBounds("TX: GPIO 43", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 140);
    canvas.print("TX: GPIO 43");
    
    canvas.getTextBounds("Baud: 9600", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 160);
    canvas.print("Baud: 9600");
    
    canvas.setTextColor(canvas.color565(150, 150, 150));
    canvas.getTextBounds("Use terminal to chat", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 190);
    canvas.print("Use terminal to chat");
    
    lilka::display.drawCanvas(&canvas);
    
    // Initialize serial handler with shared OpenAI client
    serialHandler = new SerialHandler(openaiClient);
    serialHandler->begin();
}

void runSettingsMode() {
    while (1) {
        // Create settings submenu
        lilka::Menu settingsMenu("Settings");
        settingsMenu.addItem("Settings", NULL, lilka::colors::Arylide_yellow);
        settingsMenu.addItem("Quick settings", NULL, lilka::colors::Arylide_yellow);
        settingsMenu.addItem("Back", NULL, lilka::colors::Dark_sea_green);
        
        // Run menu until selection
        while (!settingsMenu.isFinished()) {
            settingsMenu.update();
            lilka::Canvas canvas;
            settingsMenu.draw(&canvas);
            lilka::display.drawCanvas(&canvas);
        }
        
        // Handle selection
        int16_t cursor = settingsMenu.getCursor();
        if (cursor == 0) {
            // Web Config
            runWebConfigMode();
            ESP.restart();
        } else if (cursor == 1) {
            // Quick settings
            runQuickSettingsMode();
            // Continue loop to recreate and show Settings menu again
        } else if (cursor == 2) {
            // Back - return to main menu
            break;
        }
    }
}

void runWebConfigMode() {
    // Start WiFi AP
    ConfigServer configServer(settings);
    configServer.start();
    
    IPAddress IP = WiFi.softAPIP();
    
    // Display configuration screen
    lilka::Canvas canvas;
    canvas.fillScreen(0);
    
    int16_t x1, y1;
    uint16_t w, h;
    
    // Title
    canvas.setTextSize(2);
    canvas.setTextColor(canvas.color565(255, 150, 50));
    canvas.getTextBounds("Settings", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 40);
    canvas.print("Settings");
    
    // Instructions
    canvas.setTextSize(1);
    canvas.setTextColor(canvas.color565(200, 200, 200));
    canvas.getTextBounds("Connect to WiFi:", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 80);
    canvas.print("Connect to WiFi:");
    
    canvas.setTextColor(canvas.color565(100, 255, 100));
    canvas.getTextBounds("LilkaAI-AP", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 100);
    canvas.print("LilkaAI-AP");
    
    canvas.setTextColor(canvas.color565(200, 200, 200));
    canvas.getTextBounds("Password:", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 130);
    canvas.print("Password:");
    
    canvas.setTextColor(canvas.color565(100, 255, 100));
    canvas.getTextBounds("lilka2026", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 150);
    canvas.print("lilka2026");
    
    canvas.setTextColor(canvas.color565(200, 200, 200));
    canvas.getTextBounds("Open browser:", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 180);
    canvas.print("Open browser:");
    
    String ipText = "http://" + IP.toString();
    canvas.setTextColor(canvas.color565(255, 255, 100));
    canvas.getTextBounds(ipText.c_str(), 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 200);
    canvas.print(ipText);
    
    lilka::display.drawCanvas(&canvas);
    
    // Wait for configuration
    while (!configServer.isDone()) {
        configServer.loop();
        delay(10);
    }
    
    // Stop AP
    WiFi.softAPdisconnect(true);
}

void runQuickSettingsMode() {
    while (1) {
        // Create quick settings submenu
        lilka::Menu quickMenu("Quick settings");
        quickMenu.addItem("OpenAI Model", NULL, lilka::colors::Arylide_yellow);
        quickMenu.addItem("Back", NULL, lilka::colors::Dark_sea_green);
        
        // Run menu until selection
        while (!quickMenu.isFinished()) {
            quickMenu.update();
            lilka::Canvas canvas;
            quickMenu.draw(&canvas);
            lilka::display.drawCanvas(&canvas);
        }
        
        // Handle selection
        int16_t cursor = quickMenu.getCursor();
        if (cursor == 0) {
            // Edit OpenAI Model
            editOpenAIModel();
            // Continue loop to recreate and show Quick settings menu again
        } else if (cursor == 1) {
            // Back - return to settings menu
            break;
        }
    }
}

void editOpenAIModel() {
    // Get current model
    String currentModel = settings->getOpenAIModel();
    
    // Show input dialog
    lilka::InputDialog dialog("OpenAI Model");
    dialog.setValue(currentModel);
    
    while (!dialog.isFinished()) {
        dialog.update();
        lilka::Canvas canvas;
        dialog.draw(&canvas);
        lilka::display.drawCanvas(&canvas);
    }
    
    // Save if confirmed
    String newModel = dialog.getValue();
    if (!newModel.isEmpty()) {
        settings->setOpenAIModel(newModel);
        
        // Show confirmation
        lilka::Alert alert("Saved", "Model updated to: " + newModel);
        alert.draw(&lilka::display);
        while (!alert.isFinished()) {
            alert.update();
        }
    }
}
