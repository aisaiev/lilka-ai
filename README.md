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

1. **Build and Upload**

```bash
# Build and upload using PlatformIO
pio run --target upload
```

2. **Configure via Web Interface**

   - Power on Lilka board
   - From main menu, select **Settings** → **Settings**
   - Lilka will create WiFi access point "LilkaAI-AP"
   - Connect your device to this WiFi (password: `lilka2026`)
   - Open browser and navigate to `http://192.168.4.1`
   - Enter your WiFi credentials and OpenAI API key
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
- Opens WiFi access point for web-based configuration
- Configure: WiFi SSID, WiFi Password, OpenAI API Key, AI Model
- Requires device restart after saving

**Quick settings**:
- **OpenAI Model**: Change AI model without full configuration
  - Default: gpt-4.1-nano
  - Other options: gpt-4.1, gpt-4o-mini, gpt-5-mini, etc.
- Changes take effect immediately

## Project Structure

```
lilka-ai/
├── include/
│   ├── config.h              # Constants and screen dimensions
│   ├── openai_client.h       # OpenAI API client
│   ├── ui_handler.h          # UI management
│   ├── serial_handler.h      # RS-232 serial communication
│   ├── settings.h            # NVS settings storage
│   └── config_server.h       # Web configuration server
├── src/
│   ├── main.cpp              # Main application and menu logic
│   ├── openai_client.cpp     # OpenAI API implementation
│   ├── ui_handler.cpp        # UI screens and dialogs
│   ├── serial_handler.cpp    # Serial mode implementation
│   ├── settings.cpp          # Settings persistence
│   └── config_server.cpp     # Web server for configuration
├── platformio.ini            # PlatformIO configuration
└── README.md                # This file
```

## Configuration

All configuration is done through the web interface. No code changes required!

### Web Configuration Fields

- **WiFi SSID**: Your WiFi network name
- **WiFi Password**: Your WiFi password
- **OpenAI API Key**: Get from [OpenAI Platform](https://platform.openai.com/api-keys)
- **AI Model**: OpenAI model to use (e.g., gpt-4o-mini, gpt-4o)

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

### WiFi Connection Issues

- Verify correct SSID and password in web config
- Check 2.4GHz network (ESP32 doesn't support 5GHz)
- Ensure WiFi signal strength is adequate
- Check router allows new device connections

### OpenAI API Errors

- Verify API key is correct and active
- Check OpenAI account has available credits
- Confirm model name is valid (use Quick settings to change)
- Ensure internet connectivity after WiFi connection

### Web Configuration Issues

- If can't connect to LilkaAI-AP, restart Lilka
- Use exact password: `lilka2026`
- Try different browser if page doesn't load
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
