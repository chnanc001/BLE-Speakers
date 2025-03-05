# Bluetooth Speakers Using ESP32
This project involved making a bluetooth speaker using a ESP32 microcontroller ([Freenove ESP](https://github.com/Freenove/Freenove_ESP32_WROVER_Board/tree/main)) and [capacitive touch switch](https://www.amazon.ca/dp/B09VPK9N7F?ref=ppx_yo2ov_dt_b_fed_asin_title) for input and [LCD screen](https://www.amazon.ca/dp/B0B76Z83Y4?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1) as the display.




## Install
- clone repo
- connect capacitive touch switches (or other input switches), LCD screen, speakers and amplifier circuits according to schematic
- upload sketch to ESP32 board
- connect phone to speaker via bluetooth (speaker is called 'Stereo Typical')
- play music from phone (volume control from phone only)

## Schematic
LCD uses I2C communication, on the ESP32 pins 21 (SDA) and 22 (SCL) are default for I2C.





## Stereo Sound
The two [Class D amplifiers](https://www.aliexpress.com/item/1005005383121121.html?spm=a2g0o.order_detail.order_detail_item.2.428bf19ckJln9C) were connected to achieve stereo sound.

<img src="https://github.com/chnanc001/BLE-Speakers/blob/main/Images/AmplifierCircuit.PNG" width="500">


## Code
### Scrolling Text
