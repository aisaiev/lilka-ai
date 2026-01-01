#include "serial_handler.h"
#include <WiFi.h>

SerialHandler::SerialHandler(OpenAIClient* openaiClient) : inputBuffer("") {
    this->openaiClient = openaiClient;
}

void SerialHandler::begin() {
    // Initialize Serial2 with RX=44, TX=43
    // Using 9600 baud for reliable RS-232 communication
    Serial2.begin(9600, SERIAL_8N1, 44, 43);
    delay(100);
    
    displayWelcome();
    displayPrompt();
}

void SerialHandler::displayWelcome() {
    Serial2.println();
    Serial2.println("==================================================");
    Serial2.println("Lilka-AI: Serial Console AI Chat");
    Serial2.println("==================================================");
    Serial2.println();
    Serial2.println("Type your messages and press Enter to chat.");
    Serial2.println("Commands:");
    Serial2.println("  /restart - Restart and clear history");
    Serial2.println("  /help    - Show this help");
    Serial2.println();
}

void SerialHandler::displayPrompt() {
    Serial2.println();
    Serial2.print("> ");
}

void SerialHandler::handleSerialInput() {
    while (Serial2.available() > 0) {
        char c = Serial2.read();
        
        if (c == '\n' || c == '\r') {
            if (inputBuffer.length() > 0) {
                String message = inputBuffer;
                message.trim();
                inputBuffer = "";
                
                Serial2.println(); // Echo newline
                
                if (message.length() > 0) {
                    if (message.startsWith("/")) {
                        processCommand(message);
                    } else {
                        processMessage(message);
                    }
                } else {
                    displayPrompt();
                }
            }
        } else if (c == '\b' || c == 127 || c == 8) { // Backspace (handle multiple codes)
            if (inputBuffer.length() > 0) {
                inputBuffer.remove(inputBuffer.length() - 1);
                Serial2.write('\b'); // Move cursor back
                Serial2.write(' ');  // Overwrite with space
                Serial2.write('\b'); // Move cursor back again
            }
        } else if (c >= 32 && c <= 126) { // Printable characters
            inputBuffer += c;
            Serial2.print(c); // Echo character
        }
    }
}

void SerialHandler::processCommand(const String& command) {
    String cmd = command;
    cmd.toLowerCase();
    
    if (cmd == "/restart") {
        openaiClient->clearHistory();
        // Clear screen (send ANSI clear screen code)
        Serial2.print("\033[2J\033[H");
        displayWelcome();
        Serial2.println("System ready! Start chatting...");
        displayPrompt();
        return;
    }
    
    if (cmd == "/help") {
        displayWelcome();
        displayPrompt();
        return;
    }
    
    Serial2.print("Unknown command: ");
    Serial2.println(command);
    displayPrompt();
}

void SerialHandler::processMessage(const String& message) {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial2.println("Error: WiFi not connected");
        displayPrompt();
        return;
    }
    
    Serial2.println();
    Serial2.println("Processing...");
    
    // Send to OpenAI
    String response;
    bool success = openaiClient->sendMessage(message, response);
    
    if (success) {
        Serial2.println();
        Serial2.println(response);
    } else {
        Serial2.println();
        Serial2.println("Failed to get response from OpenAI. Please try again.");
    }
    
    displayPrompt();
}

void SerialHandler::loop() {
    handleSerialInput();
}
