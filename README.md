# 📸 Mini Digital Camera (ESP32-S3 Cam & ST7789)

A high-performance, ultra-compact DIY digital camera based on the **Seeed Studio XIAO ESP32-S3 Sense**. This project features real-time video streaming to a 240x240 px TFT LCD, high-resolution photo capture, and local storage on a microSD card.

## ✨ Key Features
*   **Real-time Live View:** Smooth video streaming on a 240x240 ST7789 display.
*   **Triple-Button Interface:** Dedicated hardware buttons for Shutter (Shoot), Next, and Previous navigation.
*   **Standalone Storage:** Save high-quality photos directly to the XIAO's built-in microSD slot.
*   **Color Correction:** Custom `Swap565` implementation for accurate color rendering on the LCD.

---

## 🏗️ Hardware Fabrication (PCB)

To achieve this ultra-compact form factor, a custom PCB was designed to integrate the XIAO module, display, and user interface into a single, pocket-sized board.

For the best results with SMD 0603 components and high-speed SPI signals, I highly recommend getting the boards manufactured by **[JLCPCB](https://jlcpcb.com)**.

**Why choose JLCPCB for this project?**
*   **High Precision:** Perfect for the small SMD 0603 footprints and tight traces used in this design.
*   **Fast Turnaround:** Professional prototype boards delivered in just a few days.
*   **Reliability:** Industrial-grade manufacturing ensures your SPI display and SD card run at maximum speed without data corruption.

> 💡 **One-Click Order:** You can find the full schematic and PCB layout on [OSHWlab](https://oshwlab.com/fazrialramdn_5854/mini-digital-camera-with-esp32). Simply click the **"Order at JLCPCB"** button to get these boards manufactured professionally.

---

## 🛠️ Bill of Materials (BOM)

### Core Components
*   **Microcontroller:** Seeed Studio XIAO ESP32-S3 Sense
*   **Display:** ST7789 LCD Module (240x240 px, 12-pin interface)
*   **Storage:** MicroSD Card (formatted to FAT32)
*   **Power:** 3.7V LiPo Battery

### Electronics (SMD 0603 / SOT-23)
*   3x Tactile Switches (HX-3x6x3.5)
*   1x MSK12C02 Slide Switch (Power Control)
*   1x S8050 (J3Y) Transistor (LCD Backlight switching)
*   Resistors: 100Ω (R1), 10kΩ (R2), 1kΩ (R3)
*   Capacitor: 10nF (C1)

---

## 🔧 Assembly & Hardware Hack

### 1. The "J3" Trace Cut (Critical!)
The XIAO ESP32-S3 Sense and the ST7789 display share pull-up resistors on the same lines. To prevent microSD initialization failure, you **MUST** perform this modification:
*   Locate the **J3 pad** on the XIAO expansion board (near the antenna connector).
*   Use a sharp hobby knife or cutter to carefully **cut the trace** between the two pads.

### 2. Soldering
1.  Solder the passive components (Resistors & Capacitors) first.
2.  Install the Transistor, Slide Switch, and Tactile Buttons.
3.  Carefully mount the ST7789 LCD and the XIAO S3 Sense module.

---

## 💻 Software Setup (Arduino IDE)

To compile this project, ensure you have the following libraries installed in your Arduino IDE:

### Required Libraries
| Library Name | Purpose | Installation Source |
| :--- | :--- | :--- |
| **Adafruit GFX** | Core graphics library for the display | Library Manager |
| **Adafruit ST7789** | Hardware-specific driver for the LCD | Library Manager |
| **TJpg_Decoder** | To render .jpg files from SD to Screen | Library Manager |
| **esp_camera.h** | Handling the OV2640 sensor | Built-in (ESP32 Board) |
| **FS.h & SD.h** | File system and MicroSD card support | Built-in (ESP32 Board) |
| **Arduino.h & SPI.h**| Core communication protocols | Built-in |

### Important IDE Settings
Go to **Tools** and set the following for the Seeed XIAO ESP32-S3:
*   **Board:** "Seeed XIAO ESP32S3"
*   **PSRAM:** "OPI PSRAM" (**CRITICAL:** The camera buffer will fail without this enabled)
*   **Flash Mode:** "QIO 80MHz"
*   **Flash Size:** "8MB"

---

## 🚀 How to Use
1.  **Power On:** Slide the power switch (SW4) to the ON position.
2.  **Live Stream:** The camera starts in Live Mode immediately, showing the stream on the LCD.
3.  **Capture:** Press **BTN_SHOOT** to save the current frame as a `.jpg` to the SD card.
4.  **Gallery:** Press **BTN_NEXT** to enter Gallery Mode and view saved photos.
5.  **Navigation:** Use **Next/Prev** to scroll through images. Press **BTN_SHOOT** to return to live camera mode.

---

Developed with ❤️ by [Project Pajri]([https://www.instructables.com/member/Project+Pajri/](https://www.instructables.com/Build-Your-Own-Mini-Digital-Camera-With-ESP32-S3-C/))
