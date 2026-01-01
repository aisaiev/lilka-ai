#ifndef OPENAI_CLIENT_H
#define OPENAI_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

// OpenAI API Configuration
#define OPENAI_API_URL "https://api.openai.com/v1/chat/completions"

class OpenAIClient {
public:
    OpenAIClient(const String& apiKey, const String& model);
    bool sendMessage(const String& message, String& response);
    void clearHistory();
    
private:
    String _apiKey;
    String _model;
    JsonDocument _conversationHistory;
    String buildRequestBody(const String& message);
    bool parseResponse(const String& jsonResponse, String& response);
    void initializeHistory();
};

#endif // OPENAI_CLIENT_H
