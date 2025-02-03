# ESP32-Wardriving-Deauther

ESP32-Wardriving-Deauther is a WiFi security testing tool for the ESP32 platform, allowing for WiFi wardriving, deauthentication attacks, and capturing network traffic in a PCAP file format for analysis with tools like Wireshark. **This tool is for educational and authorized testing purposes only.**

## Features

- **WiFi Scanning**: Detects and lists nearby WiFi networks.
- **Deauthentication Attacks**: Disconnects devices from their WiFi networks by sending deauthentication frames.
- **Wardriving**: Logs WiFi networks while moving, useful for mapping WiFi coverage.
- **PCAP File Generation**: Captures network traffic and stores it in PCAP format, which can be analyzed using Wireshark.

## Disclaimer

**This tool is for educational purposes only.** Unauthorized use of this tool against networks without explicit permission is **illegal** and **unethical**. The developers are not responsible for any misuse or damage caused by this tool.

## Requirements

- **ESP32 development board**
- **ESP-IDF (Espressif IoT Development Framework)**
- **Wireshark** (for analyzing PCAP files)

## Installation

1. **Clone the Repository**:
    ```bash
    git clone https://github.com/lraton/ESP32-Wardriving-Deauther.git
    cd ESP32-Wardriving-Deauther
    ```

2. **Set Up ESP-IDF** (if not installed):
    - Follow the [official ESP-IDF setup guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/).

3. **Build and Flash the Firmware**:
    ```bash
    idf.py set-target esp32
    idf.py build
    idf.py flash
    ```

4. **Monitor the Serial Output**:
    ```bash
    idf.py monitor
    ```

## Usage

1. **Power On the ESP32**: After flashing the firmware, power on the ESP32 board.
2. **Connect to the ESP32 WiFi**:
    - **SSID**: `prosciutto wifi`
    - **Password**: `prosciutto`
3. **Access the Web Interface**:
    - Open a browser and go to `http://192.168.4.1/`.
    - You'll see a list of available WiFi networks, along with options to attack and capture data.

### Attack Actions

- **Attack**: Click on the "Attack" link next to a network's SSID to perform a deauthentication attack on that network.
- **DoS Attack**: Click on the "DoS Attack" link next to a network's SSID to perform a Denial of Service (DoS) attack on that network.

### Download Captured Data (PCAP)

To download the captured network data in PCAP format, click the **"Download All"** link. The capture file will be saved as `capture.pcap` and can be opened with [Wireshark](https://www.wireshark.org/) for further analysis.

- **Download Capture**: `http://192.168.4.1/capture.pcap`

### PCAP Analysis with Wireshark

1. Open **Wireshark**.
2. Click **File** > **Open**.
3. Select the downloaded `capture.pcap` file.
4. Analyze the captured packets, such as deauthentication frames and network traffic.

## Legal and Ethical Considerations

- **Use this tool responsibly** and only on networks you own or have explicit permission to test.
- Unauthorized deauthentication attacks are **illegal** and violate laws in many countries.
- **Ensure compliance** with all applicable laws and regulations in your jurisdiction.

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE).

## Acknowledgements

This project is inspired by the [ESP8266 Deauther](https://github.com/spacehuhn/esp8266_deauther) and [ESP32-Deauther](https://github.com/tesa-klebeband/ESP32-Deauther) projects.
