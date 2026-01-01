#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <Arduino.h>
#include "openai_client.h"

class SerialHandler {
public:
    SerialHandler(OpenAIClient* openaiClient);
    void begin();
    void loop();
    
private:
    OpenAIClient* openaiClient;
    String inputBuffer;
    
    void displayWelcome();
    void displayPrompt();
    void handleSerialInput();
    void processCommand(const String& command);
    void processMessage(const String& message);
};

#endif // SERIAL_HANDLER_H
