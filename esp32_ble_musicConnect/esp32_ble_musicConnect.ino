#include "AudioTools.h"
#include "BluetoothA2DPSink.h"
#include <LiquidCrystal_I2C.h>

// for bluetooth comms
I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);


void saveSongInfo(String text);  

// button object
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

// user music controls
// pause/play button
Button pause_play(32);
int pause_play_state = 0;
// next song button
Button next(33);
// previous song button
Button previous(35);

// set the LCD number of columns and rows
int lcd_columns = 16;
int lcd_rows = 2;
LiquidCrystal_I2C lcd(0x27, lcd_columns, lcd_rows);  // 0x27 address found using i2c scanner

String title_artist;
// store song information (title + artist)
String song_info;
// for printing message to lcd screen
String message;
int pos = 0;
int count = 0;

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

void setup() {

  // comms setup
  auto cfg = i2s.defaultConfig();
  cfg.pin_bck = 27;
  cfg.pin_ws = 26;
  cfg.pin_data = 25;
  i2s.begin(cfg);
  // setup bluetooth
  a2dp_sink.start("Stereo Typical");  // or Sound Decision
  // getting metadata setup
  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST);
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);

  Serial.begin(115200);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void loop() {
  
  // if pause-play button is pressed
  if(pause_play.pressed()){
    pause_play_state = !pause_play_state;
  }
  // change pause play state
  if (pause_play_state) {
    a2dp_sink.play();
    printPlayLCD("Now Playing");
    // printPlayLCD();
  } else {
    a2dp_sink.pause();
    printPlayLCD("Paused");
    // printPauseLCD();
  }

  // if next button is pressed
  if (next.pressed()){
    a2dp_sink.next();
    pos = 0;  // so that song info starts scrolling from beginning
  }

  // if previous button is pressed
  if (previous.pressed()){
    a2dp_sink.previous();
    pos = 0;  // so that song info starts scrolling from beginning
  }

}

///*** FUNCTIONS ***///
void saveSongInfo(String text){
  song_info = text;
  for (int i=0; i < lcd_columns; i++) {
    song_info = " " + song_info;  
  } 
  song_info = song_info + " "; 
}

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


