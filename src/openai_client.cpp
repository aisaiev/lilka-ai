#include "openai_client.h"

OpenAIClient::OpenAIClient(const String& apiKey, const String& model) : _apiKey(apiKey), _model(model) {
    initializeHistory();
}

void OpenAIClient::initializeHistory() {
    _conversationHistory.clear();
    JsonArray messages = _conversationHistory["messages"].to<JsonArray>();
    
    // Add system message
    JsonObject systemMessage = messages.add<JsonObject>();
    systemMessage["role"] = "system";
    systemMessage["content"] = "You are a helpful AI assistant responding to a user via a retro terminal. Keep responses concise and formatted for a text-based terminal. Avoid markdown formatting.";
}

void OpenAIClient::clearHistory() {
    initializeHistory();
}

String OpenAIClient::buildRequestBody(const String& message) {
    JsonDocument doc;
    
    doc["model"] = _model;
    
    // Build messages array
    JsonArray messages = doc["messages"].to<JsonArray>();
    
    // Copy conversation history (includes system message)
    JsonArray historyMessages = _conversationHistory["messages"].as<JsonArray>();
    for (JsonVariant msg : historyMessages) {
        JsonObject copiedMsg = messages.add<JsonObject>();
        copiedMsg["role"] = msg["role"];
        copiedMsg["content"] = msg["content"];
    }
    
    // Add new user message
    JsonObject userMessage = messages.add<JsonObject>();
    userMessage["role"] = "user";
    userMessage["content"] = message;
    
    String output;
    serializeJson(doc, output);
    
    return output;
}

bool OpenAIClient::parseResponse(const String& jsonResponse, String& response) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonResponse);
    
    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        response = "Error: Failed to parse response";
        return false;
    }
    
    if (doc["error"].is<JsonObject>()) {
        response = "API Error: " + String(doc["error"]["message"].as<const char*>());
        return false;
    }
    
    if (doc["choices"].is<JsonArray>() && doc["choices"].size() > 0) {
        response = doc["choices"][0]["message"]["content"].as<String>();
        return true;
    }
    
    response = "Error: No response from API";
    return false;
}

bool OpenAIClient::sendMessage(const String& message, String& response) {
    if (WiFi.status() != WL_CONNECTED) {
        response = "Error: WiFi not connected";
        return false;
    }
    
    HTTPClient http;
    http.begin(OPENAI_API_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + _apiKey);
    http.setTimeout(CONNECTION_TIMEOUT);
    
    String requestBody = buildRequestBody(message);
    Serial.println("Sending request to OpenAI...");
    
    int httpCode = http.POST(requestBody);
    
    if (httpCode > 0) {
        Serial.printf("HTTP Response code: %d\n", httpCode);
        String payload = http.getString();
        
        if (httpCode == 200) {
            bool success = parseResponse(payload, response);
            http.end();
            
            if (success) {
                // Add user message and assistant response to history
                // Get the existing array (don't recreate it)
                JsonArray messages = _conversationHistory["messages"].as<JsonArray>();
                
                JsonObject userMsg = messages.add<JsonObject>();
                userMsg["role"] = "user";
                userMsg["content"] = message;
                
                JsonObject assistantMsg = messages.add<JsonObject>();
                assistantMsg["role"] = "assistant";
                assistantMsg["content"] = response;
            }
            
            return success;
        } else {
            Serial.println("Error response: " + payload);
            // Try to parse error message from JSON response
            parseResponse(payload, response);
            // parseResponse will set response to error message if it finds one
            // If it couldn't parse, response will be set to a parsing error message
            http.end();
            return false;
        }
    } else {
        Serial.printf("HTTP Request failed: %s\n", http.errorToString(httpCode).c_str());
        response = "Connection error";
        http.end();
        return false;
    }
}
