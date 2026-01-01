#include "settings.h"

Settings::Settings() {}

void Settings::begin() {
    preferences.begin("lilka-ai", false);
}

String Settings::getWifiSSID() {
    if (!preferences.isKey("wifi_ssid")) {
        return "";
    }
    return preferences.getString("wifi_ssid", "");
}

String Settings::getWifiPassword() {
    if (!preferences.isKey("wifi_pass")) {
        return "";
    }
    return preferences.getString("wifi_pass", "");
}

String Settings::getOpenAIKey() {
    if (!preferences.isKey("openai_key")) {
        return "";
    }
    return preferences.getString("openai_key", "");
}

String Settings::getOpenAIModel() {
    if (!preferences.isKey("openai_model")) {
        return "gpt-4.1-nano";
    }
    return preferences.getString("openai_model", "gpt-4.1-nano");
}

bool Settings::isConfigured() {
    return preferences.getBool("configured", false);
}

void Settings::setWifiSSID(const String& ssid) {
    preferences.putString("wifi_ssid", ssid);
}

void Settings::setWifiPassword(const String& password) {
    preferences.putString("wifi_pass", password);
}

void Settings::setOpenAIKey(const String& key) {
    preferences.putString("openai_key", key);
}

void Settings::setOpenAIModel(const String& model) {
    preferences.putString("openai_model", model);
}

void Settings::setConfigured(bool configured) {
    preferences.putBool("configured", configured);
}

void Settings::clear() {
    preferences.clear();
    preferences.putBool("configured", false);
}
