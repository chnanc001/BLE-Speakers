# Bluetooth Speakers Using ESP32
This project involved making a bluetooth speaker using a ESP32 microcontroller ([Freenove ESP](https://github.com/Freenove/Freenove_ESP32_WROVER_Board/tree/main)) and [capacitive touch switch](https://www.amazon.ca/dp/B09VPK9N7F?ref=ppx_yo2ov_dt_b_fed_asin_title) for input and [LCD screen](https://www.amazon.ca/dp/B0B76Z83Y4?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1) as the display.


## Install
- clone repo
- connect capacitive touch switches (or other input switches), LCD screen, speakers and amplifier circuits according to schematic
- upload sketch to ESP32 board
- connect phone to speaker via bluetooth (speaker is called 'Stereo Typical')
- play music from phone (volume control from phone only)

## Schematic
The LCD uses I2C communication, on the ESP32 pins 21 (SDA) and 22 (SCL) are default for I2C.  

The three (previous, pause/play, next) capacitive touch switches are simple inputs.

<img src="https://github.com/chnanc001/BLE-Speakers/blob/main/Images/LCDSwitchCircuit.PNG" width="900">


## Amplifier Circuit for Stereo Sound
The two [Class D amplifiers](https://www.aliexpress.com/item/1005005383121121.html?spm=a2g0o.order_detail.order_detail_item.2.428bf19ckJln9C) were connected as follows to achieve stereo sound.

<img src="https://github.com/chnanc001/BLE-Speakers/blob/main/Images/AmplifierCircuit.PNG" width="500">


## Code
I used the Arduino IDE for this project.

The following libraries are required for this sketch:
- [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)
- [ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP)

### LCD Screen
In this project, I used the LCD screen to display the song title and artist only while the song is playing.  Otherwise, the word 'Paused' is displayed.

First, the I2C address of the LCD screen is required.  The I2C_Scanner sketch can be used for this (credit to [here](https://randomnerdtutorials.com/esp32-esp8266-i2c-lcd-arduino-ide/)).
Then the LCD can be declared:
```
int lcd_columns = 16;
int lcd_rows = 2;
LiquidCrystal_I2C lcd(0x27, lcd_columns, lcd_rows);
```

I wanted to display the artist and song title, thus this metadata was required from the conneted device.  I only wanted to artist and song title information, thus only checked for those ids (`ESP_AVRC_MD_ATTR_ARTIST` and `ESP_AVRC_MD_ATTR_TITLE`) and saved them into the variable `title_artist`.  The following callback then is as follows:
```
// callback that gets metadata about song
void avrc_metadata_callback(uint8_t id, const uint8_t *text) {
  Serial.printf("==> AVRC metadata rsp: attribute id 0x%x, %s\n", id, text);
  if (id == ESP_AVRC_MD_ATTR_TITLE){
    // add song title info
    title_artist = String((char*)text);
  }
  if (id == ESP_AVRC_MD_ATTR_ARTIST){
    // add artist info
    title_artist = title_artist + " by " + String((char*)text);
  }
  // save song info into global variable
  saveSongInfo(title_artist);
}
```
With `saveSongInfo()` as:
```
void saveSongInfo(String text){
  song_info = text;
  for (int i=0; i < lcd_columns; i++) {
    song_info = " " + song_info;  
  } 
  song_info = song_info + " "; 
}
```

More often than not, the artist and song title would be too long to display in 16 characters (the limit of the LCD screen), so I wanted to have the text scroll.  To achieve this, the following function was implemented to scroll through the text:
```
void printPlayLCD(String text){
  lcd.clear();  // clear screen
  lcd.setCursor(0, 0);  // move cursor to first row
  lcd.print(text);
  lcd.setCursor(0, 1);  // move cursor to second row
  // scrolling text
  message = song_info.substring(pos, pos + lcd_columns);
  if (count < 1){  // delay so display isnt changed every loop iteration
    count++;
  }
  else{
    if (pos < song_info.length()){
      pos++;
    }
    else {
      pos = 0;
    }
    count = 0;
  }
  lcd.print(message);
}
```
In my main loop, I also reset `pos` to `0` every time either the next or previous button is pressed so that the text will start scrolling from the very left of the LCD screen (instead of starting from somewhere in the middle, wherever the cursor was when the button was pressed).  E.g.:
```
  // if next button is pressed
  if (next.pressed()){
    a2dp_sink.next();
    pos = 0;  // so that song info starts scrolling from beginning
  }
```


### Capacitive Touch Switches
The capacitive touch switches were treated as simple inputs.  As they are buttons, I found debouncing was required.  To make declaration and usage easier I created a Button class.
```
class Button{
  private:
    byte pin;
  public:
    int state = 0;
    int previous_state = 0;
    uint32_t last_press_time;
    int debounce_delay = 50;

    Button(byte pin){
      this->pin = pin;
      init();
    }

    void init(){
      pinMode(pin, INPUT);
    }

    int pressed(){
      int pressed = 0;
      int button_reading = digitalRead(pin);

      // debounce button
      if (button_reading != previous_state){
        last_press_time = millis();
      }

      if ((millis() - last_press_time) > debounce_delay){
        if (button_reading != state){
          state = button_reading;

          if (state == 1){
            pressed = 1;
          }
        }
      }

      previous_state = button_reading;
      return pressed;
    }
};
```
As can be seem, the pins are initialised to inputs pins in this class itself and the `pressed()` function encapsulates the debouncing. 

(For this project, I did not include input controls for volume on the speaker - this perhaps will be added in a future project).

### Amplifier-Speaker
Make sure that the pins for the ampmlifier circuit are correctly declared in the `setup()`:
```
  cfg.pin_bck = 27;
  cfg.pin_ws = 26;
  cfg.pin_data = 25;
```

## Structure
I wanted my bluetooth speaker to look like a jukebox, thus laser cut the enclosure out of mdf.  The three capacitive touch switches were simply glued to the back of their respective symbols.  (The text on the LCD screen is mid-scroll).

<img src="https://github.com/chnanc001/BLE-Speakers/blob/main/Images/Final.png" width="300">



