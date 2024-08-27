# Motorcycle Security System with RFID and Servo Lock

## 🏍️ Project Overview

This project implements an advanced security system for motorcycles using Arduino, RFID technology, and a servo-controlled lock mechanism. It provides a robust, user-friendly way to secure a motorcycle against unauthorized use or theft.

[Photo]

## 🌟 Features

- **RFID Authentication**: Secure access using pre-authorized RFID tags
- **Servo-Controlled Lock**: Physical locking mechanism for enhanced security
- **Obstacle Detection**: Prevents damage by detecting obstacles during locking
- **Visual and Audible Feedback**: LED and buzzer indicators for clear system status
- **Tamper Alert**: Triggers alarm on unauthorized access attempts
- **Lock Disable Feature**: Prevents accidental locking during ride
- **External Alarm Integration**: Compatible with existing alarm systems
- **Fail-Safe Design**: Self-checks and corrects ambiguous lock states

## 🛠️ Hardware Requirements

- Arduino Nano V3 Board
- MFRC522 RFID Reader
- Servo Motor
- 2 LEDs (Red and Green)
- Buzzer
- IR Sensor
- 2 Limit Switches
- Toggle Switch (for Lock Disable)
- Relay Module (for external alarm)
- RFID Tags/Cards
- Jumper Wires
- Breadboard or 
- Custom PCB coming

## 📋 Pin Configuration

| Component      | Arduino Pin |
|----------------|-------------|
| RFID RST       | 9           |
| RFID SDA (SS)  | 10          |
| RFID MOSI      | 11          |
| RFID MISO      | 12          |
| RFID SCK       | 13          |
| Servo Signal   | 3           |
| Red LED        | 2           |
| Green LED      | 8           |
| Buzzer         | A5          |
| IR Sensor      | 6           |
| Lock Extended  | 4           |
| Lock Retracted | 5           |
| Lock Disable   | 7           |
| External Alarm | A4          |

## 🚀 Setup and Installation

1. Clone this repository:
   ```
   git clone https://github.com/yourusername/motorcycle-security-system.git
   ```
2. Open the Arduino IDE and load the `motorcycle_security_system.ino` file.
3. Install the following libraries through the Arduino Library Manager:
   - MFRC522
   - Servo
4. Connect the hardware components according to the pin configuration.
5. Upload the code to your Arduino board.

## 🔧 Usage

1. Power ON the system.
2. Present an authorized RFID tag to lock/unlock the motorcycle.
3. The system will provide visual and audible feedback for each operation.
4. Use the Lock Disable switch when riding to prevent accidental locking.

## 📚 Documentation

For more detailed information about the system's functionality and code structure, please refer to the repository(https://github.com/s-a-usman/Lock).

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📞 Support

If you encounter any problems or have any questions, please open an issue in this repository.

## 🙏 Acknowledgements

- Thanks to all contributors who have helped to enhance this system.
- Special thanks to the Arduino and RFID communities for their invaluable resources.

---

Made with ❤️ for motorcycle enthusiasts and security-conscious riders everywhere.