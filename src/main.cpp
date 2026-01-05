#include <Arduino.h>
#include <WiFi.h>
#include <lilka.h>
#include "openai_client.h"
#include "ui_handler.h"
#include "serial_handler.h"
#include "settings.h"
#include "config_server.h"
#include "wifi_config.h"

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
bool wifiConnected = false;

// Function declarations
void handleUserInput();
void runSerialMode();
void runSettingsMode();
bool runWebConfigMode();
void runQuickSettingsMode();
void editOpenAIModel();

void setup() {
    lilka::begin();
    
    Serial.println("Lilka AI");
    Serial.println("=============================");
    
    // Display welcome message
    lilka::Canvas canvas;
    canvas.fillScreen(0);
    
    int16_t x1, y1;
    uint16_t w, h;
    
    canvas.setTextSize(3);
    canvas.setTextColor(canvas.color565(150, 200, 255));
    canvas.getTextBounds("Lilka AI", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, canvas.height() / 2 + h / 2);
    canvas.print("Lilka AI");
    
    lilka::display.drawCanvas(&canvas);
    delay(1000);
    
    // Initialize settings
    settings = new Settings();
    settings->begin();
    
    // Initialize UI
    ui = new UIHandler();
    
    // Connect to WiFi during boot
    String wifiSSID, wifiPassword;
    if (loadWiFiCredentials(wifiSSID, wifiPassword)) {
        Serial.printf("Found WiFi credentials for: %s\n", wifiSSID.c_str());
        wifiConnected = connectToWiFi(wifiSSID, wifiPassword);
    }
    
    // Main menu loop
    bool menuActive = true;
    while (menuActive) {
        // Create mode selection menu
        lilka::Menu menu("Lilka AI");
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
        
        // Check WiFi connection status
        if (!wifiConnected) {
            lilka::Alert alert("WiFi Error", "No WiFi connection.\n\nPlease configure WiFi in Keira and restart.\n\nPress A to return.");
            alert.draw(&lilka::display);
            while (!alert.isFinished()) {
                alert.update();
            }
            continue;
        }
        
        // Check if OpenAI API key is configured
        if (settings->getOpenAIKey().isEmpty()) {
            lilka::Alert alert("Settings Required", "Please configure OpenAI API Key in Settings menu.");
            alert.draw(&lilka::display);
            while (!alert.isFinished()) {
                alert.update();
            }
            continue;
        }
        
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
        settingsMenu.addItem("Web Settings", NULL, lilka::colors::Arylide_yellow);
        settingsMenu.addItem("Quick Settings", NULL, lilka::colors::Arylide_yellow);
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
            bool settingsSaved = runWebConfigMode();
            if (settingsSaved) {
                ESP.restart();
            }
            // If cancelled (B pressed), return to main menu
            break;
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

bool runWebConfigMode() {
    // Check if WiFi is connected
    if (!wifiConnected || WiFi.status() != WL_CONNECTED) {
        lilka::Alert alert("WiFi Error", "Not connected to WiFi.\n\nPlease configure WiFi in Keira and restart.\n\nPress A to return.");
        alert.draw(&lilka::display);
        while (!alert.isFinished()) {
            alert.update();
        }
        return false;
    }
    
    // Start web server on existing WiFi connection
    ConfigServer configServer(settings);
    configServer.start();
    
    IPAddress IP = WiFi.localIP();
    
    // Display configuration screen
    lilka::Canvas canvas;
    canvas.fillScreen(0);
    
    int16_t x1, y1;
    uint16_t w, h;
    
    // Title
    canvas.setTextSize(2);
    canvas.setTextColor(canvas.color565(255, 150, 50));
    canvas.getTextBounds("Web Settings", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 40);
    canvas.print("Web Settings");
    
    // Instructions
    canvas.setTextSize(1);
    canvas.setTextColor(canvas.color565(200, 200, 200));
    canvas.getTextBounds("Open browser on", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 90);
    canvas.print("Open browser on");
    
    canvas.getTextBounds("same WiFi network:", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 110);
    canvas.print("same WiFi network:");
    
    String ipText = "http://" + IP.toString();
    canvas.setTextColor(canvas.color565(255, 255, 100));
    canvas.getTextBounds(ipText.c_str(), 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 150);
    canvas.print(ipText);
    
    canvas.setTextSize(1);
    canvas.setTextColor(canvas.color565(150, 150, 150));
    canvas.getTextBounds("Press B to cancel", 0, 0, &x1, &y1, &w, &h);
    canvas.setCursor((canvas.width() - w) / 2, 200);
    canvas.print("Press B to cancel");
    
    lilka::display.drawCanvas(&canvas);
    
    // Wait for configuration or cancel button
    while (!configServer.isDone()) {
        configServer.loop();
        lilka::State state = lilka::controller.getState();
        if (state.b.justPressed) {
            return false; // Cancelled, return to main menu
        }
    }
    
    return true; // Settings saved, will restart
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
