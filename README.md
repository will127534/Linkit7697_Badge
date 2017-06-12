# Trophy Board

This is the source code for a trophy board project. 

The hardware is based on LinkIt 7697 HDK and the schematic can be found in `hardware` directory. It provides an LED Matrix for display, a directional pad and 4 buttons, in addition to USR button and RST button on the side of the board.

The software provides features including:

 * A cute boot animation - this is a trophy board after all.
 * Menus to select several functions by directional pad:
   * **Clock** application, which syncs time through NTP protocol over Wi-Fi network.
   * **Snake** the game.
   * **Tetris** the game.
   * **Wi-Fi config** app, which allows you to set Wi-Fi AP's SSID and password through COM port. To connect to COM port, [install CP2102N driver](https://docs.labs.mediatek.com/resource/linkit7697-arduino/en/connecting-linkit-7697-to-computer), connect the board to your computer via micro-USB cable, and open the COM port with baud rate **115200**.

See https://docs.labs.mediatek.com/trophy-board for detailed usage instructions.

## Build & Run

The program is in the `software/Badge` directory.

This program is an **Arduino sketch** for LinkIt 7697. To build this sketch, you need to:

 1. Follow the instructions in [LinkIt 7697 HDK Get Started](https://docs.labs.mediatek.com/resource/linkit7697-arduino/en/setup-arduino-ide-for-linkit-7697) to install the IDE and board support package.
 1. Install dependent libraries in `software/Library` by copy the folders such as `Adafruit_IS31FL3731` to Arduino IDE Library path, which is usually in `%USERPROFILE%/Documents/Arduino/Library` on Windows. You can open **File > Preferences** in IDE menu and check the **Sketchbook location** to know the exact library path.
 1. Open the sketch in `software/Badge/Badge.ino` and build it with Arduino IDE.

To run this sketch, simple use the upload function in Arduino IDE.
