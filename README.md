# Lilka AI

OpenAI integration for Lilka ESP32 development board, providing interactive AI conversations with multiple interface modes and web-based configuration.

## Features

- ✨ OpenAI GPT integration with conversation history
- 🎮 Two interaction modes: On-device chat and RS-232 serial terminal
- 📱 Web-based configuration portal (no code changes needed)
- 💾 Persistent settings stored in non-volatile memory
- 🔧 Quick settings for model switching
- 💬 Real-time AI responses with scrollable display
- 🔋 Optimized for ESP32 performance

## Hardware Requirements

- Lilka ESP32 dev board (v2)
- WiFi network access
- OpenAI API key
- (Optional) RS-232 device for serial mode

## Quick Start

### First Time Setup

1. **Configure WiFi in Keira OS**

   - This firmware is designed to be loaded from Keira OS
   - WiFi credentials must be configured in Keira first
   - The app will automatically connect to WiFi during boot

2. **Configure OpenAI API Key**

   - Load the firmware from Keira OS
   - WiFi will connect automatically on startup
   - From main menu, select **Settings** → **Web Settings**
   - Open browser on same WiFi network and navigate to the displayed IP address
   - Enter your OpenAI API key and preferred model
   - Click "Save Settings"
   - Board will restart automatically

3. **Start Using**

   - Select **Chat mode** for on-device interaction
   - Or select **Serial mode** for RS-232 terminal communication

## Usage

### Main Menu Navigation

- **UP/DOWN**: Navigate menu items
- **A Button**: Select menu item
- **B Button**: Go back (in submenus)

### Main Menu Options

1. **Chat mode**: Interactive chat using on-screen keyboard
2. **Serial mode**: Terminal-based chat via RS-232
3. **Settings**: Configuration options

### Chat Mode

1. Select **Chat mode** from main menu
2. Use on-screen keyboard to type your message
3. Press **A** to send message to OpenAI
4. Wait for response (shows "Thinking..." screen)
5. Read response (use **UP/DOWN** to scroll if needed)
6. Press **A** to continue chatting
7. Press **SELECT/START** button to return to main menu

### Serial Mode

Connect RS-232 device to Lilka:
- **TX**: GPIO 43
- **RX**: GPIO 44
- **Baud Rate**: 9600

Use any serial terminal to send messages and receive AI responses.

### Settings Menu

**Settings (Web Config)**:
- Opens web server on existing WiFi connection
- Configure: OpenAI API Key, AI Model
- Access via browser on same network as Lilka
- WiFi is configured in Keira OS (not in this app)
- Requires device restart after saving

**Quick settings**:
- **OpenAI Model**: Change AI model without full configuration
  - Default: gpt-4.1-nano
  - Other options: gpt-4o-mini, gpt-4o, gpt-5-mini, etc.
- Changes take effect immediately

## Project Structure

```
lilka-ai/
├── include/
│   ├── config.h              # Constants and screen dimensions
│   ├── openai_client.h       # OpenAI API client
│   ├── ui_handler.h          # UI management
│   ├── serial_handler.h      # RS-232 serial commun (OpenAI only)
│   ├── config_server.h       # Web configuration server
│   └── wifi_config.h         # Keira WiFi credential loader
├── src/
│   ├── main.cpp              # Main application and menu logic
│   ├── openai_client.cpp     # OpenAI API implementation
│   ├── ui_handler.cpp        # UI screens and dialogs
│   ├── serial_handler.cpp    # Serial mode implementation
│   ├── settings.cpp          # Settings persistence (OpenAI only)
│   ├── config_server.cpp     # Web server for configuration
│   └── wifi_config.cpp       # Keira WiFi NVS reader
├── platformio.ini            # PlatformIO configuration
└── README.md o.ini            # PlatformIO configuration
└── README.md                # This file
```

## Configuration

### WiFi Configuration

WiFi credentials are managed by Keira OS. This firmware:
- Reads WiFi settings from Keira's NVS storage during boot
- Automatically connects to WiFi on startup
- Shows error alert if WiFi is not configured or connection fails
- You must configure WiFi in Keira before using this app

### OpenAI Configuration

OpenAI settings are configured through the web interface:

- **OpenAI API Key**: Get from [OpenAI Platform](https://platform.openai.com/api-keys)
- **AI Model**: OpenAI model to use (e.g., gpt-4.1-nano, gpt-4o-mini, gpt-4o)

To access web config:
1. Ensure Lilka is connected to WiFi (during boot)
2. Navigate to Settings → Web Settings from main menu
3. Note the IP address shown on screen
4. Open browser on same network and enter the IP address

### Get OpenAI API Key

1. Visit [OpenAI Platform](https://platform.openai.com/)
2. Sign in or create an account
3. Navigate to [API Keys](https://platform.openai.com/api-keys)
4. Create a new secret key
5. Copy and paste into web configuration

### Advanced Configuration

Edit `include/config.h` for advanced options:

- `SCREEN_WIDTH` / `SCREEN_HEIGHT`: Display dimensions
- `TOP_MARGIN` / `MARGIN`: UI spacing
- `CONNECTION_TIMEOUT`: HTTP timeout (default: 10000ms)

## Button Reference

| Button | Main Menu | Chat Mode | Serial Mode | Menus |
|--------|-----------|-----------|-------------|-------|
| UP | Navigate up | Scroll up | - | Navigate up |
| DOWN | Navigate down | Scroll down | - | Navigate down |
| A | Select item | Continue/Send | - | Select item |
| B | - | - | - | Go back |
| START | - | Return to main | - | - |

## Features Details

### Conversation History

Chat mode maintains conversation context, allowing multi-turn conversations with the AI.

### Scrollable Responses

Long AI responses can be scrolled using UP/DOWN buttons to read all content.

### Error Handling

- Network errors display informative alerts
- Invalid model names show OpenAI error messages
- WiFi connection failures provide feedback

### Persistent Settings

All settings are stored in ESP32 non-volatile storage and persist across reboots.

## Troubleshooting
**Configure WiFi in Keira OS first** - this app reads credentials from Keira's storage
- Verify WiFi is working in Keira OS before loading this firmware
- Check 2.4GHz network (ESP32 doesn't support 5GHz)
- Ensure WiFi signal strength is adequate
- If no credentials found, restart in Keira and configure WiFi therenfig
- Check 2.4GHz network (ESP32 doesn't support 5GHz)
- Ensure WiFi signal strength is adequate
- Check router allows new device connections

### OpenAI API Errors

- Verify API key is correct and active
- Check OpenAI account has available credits
- Confirm model name is valid (use Quick settings to change)
- Ensure internet connectivity after WiFi connection

### Web Configuration Issues

- Ensure your device is on the same WiFi network as Lilka
- Verify WiFi connection succeeded during boot
- Use the exact IP address shown on Lilka's screen
- Try different browser if page doesn't load
- Check firewall/router settings allow local network access
- Press B to cancel web config mode
- Wait for full restart after saving settings

### Serial Mode Issues

- Verify correct GPIO pins (TX: 43, RX: 44)
- Check baud rate is set to 9600
- Ensure proper RS-232 voltage levels
- Test serial connection before running

### Build Errors

- Update PlatformIO: `pio upgrade`
- Clean build: `pio run --target clean`
- Update platform: `pio pkg update`
- Check Lilka SDK is properly installed

## Dependencies

- **Lilka SDK**: ESP32 board support and UI components
- **ArduinoJson**: JSON parsing and serialization (v7+)
- **WiFi**: ESP32 WiFi library (built-in)
- **HTTPClient**: HTTP communication (built-in)
- **WebServer**: Web configuration server (built-in)
- **Preferences**: NVS storage (built-in)

## Technical Specifications

- **Platform**: ESP32 (Lilka v2)
- **WiFi**: 2.4GHz 802.11 b/g/n
- **Serial**: RS-232, 9600 baud, 8N1
- **Display**: Full screen with custom UI elements
- **Storage**: NVS (Non-Volatile Storage) for settings
- **HTTP**: TLS/SSL support for OpenAI API

## License

This project is provided as-is for educational and development purposes.

## Resources

- [Lilka SDK Documentation](https://docs.lilka.dev/projects/sdk/uk/latest/)
- [OpenAI API Documentation](https://platform.openai.com/docs/)
- [OpenAI Models](https://platform.openai.com/docs/models)
- [PlatformIO Documentation](https://docs.platformio.org/)

## Contributing

This is an educational project. Feel free to fork and modify for your needs.

## Credits

Developed for Lilka ESP32 dev board by Андерсон & friends.
