//****************************************************************************//
// Torquetuner - firmware                                                     //
// SAT/Metalab                                                                //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Albert Niyonsenga and Christian Frisson (2022)                             //
//****************************************************************************//

/* Created using the Puara template: https://github.com/Puara/puara-module-template 
 * The template contains a fully commented version for the commonly used commands 
 */
//*****************************************************************************//
// INCLUDES Section

//#define SPARKFUN_ESP32_THING_PLUS 1
unsigned int firmware_version = 20221020;
// Define visual feedback
#define VISUAL_FEEDBACK
// Define libmapper
#define LIBMAPPER

#include "Arduino.h"
#include "variants.h"

// #include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <cmath>
#include <mapper.h>
//#include "Filter.h"
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <TaskScheduler.h>
// #include <freertos/FreeRTOS.h>

#ifndef SPARKFUN_ESP32_THING_PLUS
#include <TinyPICO.h>
#endif

#include "haptics.h"

// For disabling power saving
#include "esp_wifi.h"

// Include Puara's module manager
// If using Arduino.h, include it before including puara.h
#include "puara.h"

const int SEL_PIN = 0;

#ifdef TSTICKJOINT
const int SDA_PIN = 26;
const int SCL_PIN = 25;
#else
#ifdef SPARKFUN_ESP32_THING_PLUS
const int SDA_PIN = 23;
const int SCL_PIN = 22;
#else
const int SDA_PIN = 21;
const int SCL_PIN = 22;
#endif
#endif
//**************************INITIALISE PUARA + MCU Libraries*********************//
// Initialise Puara
Puara puara;

#ifndef SPARKFUN_ESP32_THING_PLUS
// Initialise the TinyPICO library
TinyPICO tp = TinyPICO();
#endif

//**************************INITIALISE LCD***************************************//
// LCD properties
#ifdef VISUAL_FEEDBACK
  Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
  #define OFF 0x0
  #define RED 0x1
  #define YELLOW 0x3
  #define GREEN 0x2
  #define TEAL 0x6
  #define BLUE 0x4
  #define VIOLET 0x5
  #define WHITE 0x7 
  int gui_state = 0;
  int max_gui_state = 3; //0 = haptic mode, 1 = angle, 2 = velocity, 3 = torque
  int old_gui_state = 0;


// Initial States
int STATE = 4;
int OLD_VALUE = 9999;
int OLD_STATE = 4;
int MAX_MOTOR_STATES = 8;


void print_state(int cur_state) {
  lcd.setCursor(0,1);
  if (cur_state == 0) {
    lcd.print("CLICK");
  } 
  else if (cur_state == 1) {
    lcd.print("MAGNET");
  }
  else if (cur_state == 2) {
    lcd.print("WALL");
  }
  else if (cur_state == 3) {
    lcd.print("INERTIA");
  }  
  else if (cur_state == 4) {
    lcd.print("LINEAR SPRING");
  }
  else if (cur_state == 5) {
    lcd.print("EXP SPRING");
  }
  else if (cur_state == 6) {
    lcd.print("FREE");
  }
  else if (cur_state == 7) {
    lcd.print("SPIN");
  }
  else if (cur_state == 8) {
    lcd.print("SERIAL LISTEN");
  }
  else {
    lcd.print("Unknown State");
  }
}

int CHANGE_STATE(int cur_state, int max_state, int inc_flag) {
  int new_state = 0;
  if (inc_flag) {
    new_state = cur_state + 1;
  } else{
    new_state = cur_state - 1;
  }
  if (new_state > max_state) {
    new_state = 0;
  }
  if (new_state < 0) {
    new_state = max_state;
  }
  printf("New State %d\n",new_state);
  return new_state;
}

const uint32_t DEBOUNCE_TIME = 10000; // 10 ms
bool update_btn(const int pin) {
  static bool last_val;
  static bool has_changed;
  static int32_t last_change;
  // Read button pin
  int32_t now = esp_timer_get_time();
  bool val =  !digitalRead(pin);
  if (val != last_val) {
    last_val = val;
    last_change = now;
    has_changed = true;
  }

  // Debounce button and trigger on release
  if (has_changed && (now - last_change) > DEBOUNCE_TIME  && !val) {
    has_changed = false;
    return true;
  } else {
    return false;
  }

}

bool update_btn_lcd(uint8_t buttonPressed){
  static bool last_val;
  static bool has_changed;
  static int32_t last_change;
  // Read button pin
  int32_t now = esp_timer_get_time();
  bool val =  !buttonPressed;
  if (val != last_val) {
    last_val = val;
    last_change = now;
    has_changed = true;
  }

  // Debounce button and trigger on release
  if (has_changed && (now - last_change) > DEBOUNCE_TIME  && !val) {
    has_changed = false;
    return true;
  } else {
    return false;
  }

}
#endif
//*************************SET UP TORQUETUNER************************************//
// I2C variables
const uint8_t I2C_BUF_SIZE = 10;
const uint8_t CHECKSUMSIZE = 2;
uint8_t tx_data[I2C_BUF_SIZE+CHECKSUMSIZE];
uint8_t rx_data[I2C_BUF_SIZE+CHECKSUMSIZE];
uint16_t checksum_rx = 0;
uint16_t checksum_tx = 0;

// Initialize TorqueTuner
TorqueTuner knob;

// State flags
int connected = 0;
bool is_playing = true;

//========TORQUETUNER FUNCTIONS========
uint16_t calcsum(uint8_t buf[], uint8_t length) {
  uint16_t val = 0;
  for (int k = 0; k < length; k++) {
    val += buf[k];
  }
  return val;
}

int read_param(float * param, uint8_t*  data, uint8_t length) {
  memcpy(param, data, length);
  if ( std::isnan(*param)) {
    return 1;
  } else {
    return 0;
  }
}

int receiveI2C(TorqueTuner * knob_) {
  Wire.requestFrom(8, I2C_BUF_SIZE + CHECKSUMSIZE);
  uint8_t k = 0;
  while (Wire.available()) {
    rx_data[k] = Wire.read();
    k++;
  }
  if (k != I2C_BUF_SIZE + CHECKSUMSIZE) { // check if all data is recieved
    //printf("Error in recieved data. Bytes missing :  %i\n", I2C_BUF_SIZE + CHECKSUMSIZE - k);
    return 1;
  }
  else {
    memcpy(&checksum_rx, rx_data + I2C_BUF_SIZE, 2); // read checksum
    if (checksum_rx != calcsum(rx_data, I2C_BUF_SIZE)) { // error in recieved data
      return 2;
    }
    else { // Succesfull recieve
      #ifdef MECHADUINO
      memcpy(&knob_->angle, rx_data, 2);
      #endif
      #ifdef MOTEUS
      memcpy(&knob_->angle, rx_data + 1, 2);
      #endif
      memcpy(&knob_->velocity, rx_data + 4, 4);
      //printf("Angle %d Velocity %f\n",knob_->angle,knob_->velocity );
      return 0; //Return 0 if no error has occured
    }
  }
}

void sendI2C(TorqueTuner * knob_) {
  Wire.beginTransmission(8); // transmit to device #8
  memcpy(tx_data, &knob_->torque, 2);
  memcpy(tx_data + 2, &knob_->target_velocity, 4);
  memcpy(tx_data + 6, &knob_->active_mode->pid_mode, 1);
  checksum_tx = calcsum(tx_data, I2C_BUF_SIZE);
  memcpy(tx_data + I2C_BUF_SIZE , &checksum_tx, 2);
  //printf("Torque %d Angle %d Velocity %f Target %f Mode %c\n",knob_->torque,knob_->angle,knob_->velocity,knob_->target_velocity,knob_->active_mode->pid_mode);
  int n = Wire.write(tx_data, I2C_BUF_SIZE + CHECKSUMSIZE);
  Wire.endTransmission();    // stop transmitting
}
//*************************SET UP LIBMAPPER**************************************//
#ifdef LIBMAPPER
// Libmapper variables
mpr_sig in_sig_scale;
mpr_sig in_sig_stretch;
mpr_sig in_sig_mode;
mpr_sig in_sig_target_velocity;
mpr_sig in_sig_offset;
mpr_sig in_sig_damping;

mpr_sig out_sig_angle;
mpr_sig out_sig_velocity;
mpr_sig out_sig_trigger;
mpr_sig out_sig_speed;
mpr_sig out_sig_quant_angle;
mpr_sig out_sig_acceleration;
mpr_dev dev;

int pressure = 0;
int sel = 0;
int sel_min = 0;
int sel_max = 0;

// System variables
int err = 0;
int err_count = 0;
uint32_t last_time = 0;
uint32_t last_time_libmapper_poll = 0;
uint32_t last_time_libmapper_update = 0;
uint32_t last_time_errprint = 0;
uint32_t last_time_maintenance = 0;
uint32_t last_time_gui = 0;
uint32_t now = 0;

//========Setup Libmapper Signals========
void in_sig_scale_callback(mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type, const void* value, mpr_time time) {
  knob.scale =  *((float*)value);
}

void in_sig_stretch_callback(mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type, const void* value, mpr_time time) {
  knob.stretch =  *((float*)value);
}

void in_sig_offset_callback(mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type, const void* value, mpr_time time) {
  knob.active_mode->offset = (*((float*)value));
  //printf("Offset: %f", knob.active_mode->offset);
}

void in_sig_mode_callback(mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type, const void* value, mpr_time time) {
  knob.set_mode(*((int32_t*)value));
}

void in_sig_target_velocity_callback(mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type, const void* value, mpr_time time) {
  knob.target_velocity = (*((float*)value));
  //printf("Target Velocity: %f", knob.target_velocity);
}

void in_sig_damping_callback(mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type, const void* value, mpr_time time) {
  knob.active_mode->damping = (*((float*)value));
}


void init_mpr_sigs() {
  dev = mpr_dev_new(puara.get_dmi_name().c_str(), 0);

  // Init libmapper inputs
  float scale_min = -230;
  float scale_max = 230;
  in_sig_scale = mpr_sig_new(dev, MPR_DIR_IN, "Scale", 1, MPR_FLT, "Ncm", &scale_min, &scale_max, 0, in_sig_scale_callback, MPR_SIG_UPDATE);

  float angle_scale_min = 0;
  float angle_scale_max = 30;
  in_sig_stretch = mpr_sig_new(dev, MPR_DIR_IN, "Stretch", 1, MPR_FLT, "ratio", &angle_scale_min, &angle_scale_max, 0, in_sig_stretch_callback, MPR_SIG_UPDATE);

  int mode_min = 0;
  int mode_max = knob.num_modes - 1;
  sel_max = mode_max;
  in_sig_mode = mpr_sig_new(dev, MPR_DIR_IN, "Mode", 1, MPR_INT32, "mode", &mode_min, &mode_max, 0, in_sig_mode_callback, MPR_SIG_UPDATE);

  float vel_min = -700;
  float vel_max = 700;
  in_sig_target_velocity = mpr_sig_new(dev, MPR_DIR_IN, "TargetVelocity", 1, MPR_FLT, "Rpm", &vel_min, &vel_max, 0, in_sig_target_velocity_callback, MPR_SIG_UPDATE);

  float offset_min = -1800;
  float offset_max = 1800;
  in_sig_offset = mpr_sig_new(dev, MPR_DIR_IN, "Offset", 1, MPR_FLT, "degrees", &offset_min, &offset_max, 0, in_sig_offset_callback, MPR_SIG_UPDATE);

  float damping_min = -1;
  float damping_max = 1;
  in_sig_damping = mpr_sig_new(dev, MPR_DIR_IN, "Damping", 1, MPR_FLT, "ratio", &offset_min, &offset_max, 0, in_sig_damping_callback, MPR_SIG_UPDATE);

  // Init libmapper outputs
  int angle_min = 0;
  int angle_max = 3600;
  out_sig_angle = mpr_sig_new(dev, MPR_DIR_OUT, "Angle", 1, MPR_INT32, 0, &angle_min, &angle_max, 0, 0, 0);

  out_sig_velocity = mpr_sig_new(dev, MPR_DIR_OUT, "Velocity", 1, MPR_FLT, 0, &vel_min, &vel_max, 0, 0, 0);

  int trig_min = 0;
  int trig_max = 1;
  out_sig_trigger = mpr_sig_new(dev, MPR_DIR_OUT, "Trigger", 1, MPR_INT32, 0, &trig_min, &trig_max, 0, 0, 0);

  float speed_min = 0;
  float speed_max = vel_max;
  out_sig_speed = mpr_sig_new(dev, MPR_DIR_OUT, "Speed", 1, MPR_FLT, 0, &speed_min, &speed_max, 0, 0, 0);

  out_sig_quant_angle = mpr_sig_new(dev, MPR_DIR_OUT, "QuantAngle", 1, MPR_INT32, 0, &angle_min, &angle_max, 0, 0, 0);

  float acc_min = -100;
  float acc_max = 100;
  out_sig_acceleration = mpr_sig_new(dev, MPR_DIR_OUT, "Acceleration", 1, MPR_FLT, 0, &acc_min, &acc_max, 0, 0, 0);

  mpr_dev_poll(dev, 50);

}
#endif
//*************************SET UP OSC********************************************//
// Setup OSC signals
/*
 * Creating liblo addresses for sending direct OSC messages.
 * Those will be populated with IP address and port provided
 * by the puara module manager.
 */
lo_address osc1;
lo_address osc2;
std::string baseNamespace = "/";
std::string oscNamespace;

// Declare a new liblo server and set an error callback
void error(int num, const char *msg, const char *path) {
    printf("Liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}
lo_server_thread osc_server;

/* 
 * Generic handler that catches any incoming messages and display them. 
 * Returning 1 means that the message has not been fully handled and the server 
 * should try other methods.
 * (based on https://github.com/radarsat1/liblo/blob/master/examples/example_server.c)
 */
int generic_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, lo_message data, void *user_data) {
    int i;

    printf("OSC message received; path: %s\n", path);
    for (i = 0; i < argc; i++) {
        printf("arg %d '%c' ", i, types[i]);
        lo_arg_pp((lo_type)types[i], argv[i]);
        printf("\n");
    }
    printf("\n");
    fflush(stdout);

    return 1;
}

//*********************TASK SCHEDULING*******************************************//
// Timing variables
const uint32_t LIBMAPPER_POLL_RATE = 10000 ; // us
const uint32_t LIBMAPPER_UPDATE_RATE = 10000 ; // us
const uint32_t OSC_UPDATE_RATE = 40000 ; // us
const uint32_t HAPTICS_UPDATE_RATE = 500 ; // 2 KHz
const uint32_t I2CUPDATE_FREQ = 3400000; // high speed mode;
const uint32_t GUI_RATE = 66000; //  15 FPS
const uint32_t INPUT_READ_RATE = 1000; // 1kHz

Scheduler runnerHaptics;
Scheduler runnerComms;
#ifdef LIBMAPPER
void pollLibmapper();
void updateLibmapper();
#endif
#ifdef VISUAL_FEEDBACK
void updateDisplay();
void readInputs();
#endif
void updateHaptics();
void reconnectWifi();
void updateOSC();
// Comms Tasks
#ifdef LIBMAPPER
Task libmapperPoll (LIBMAPPER_POLL_RATE, TASK_FOREVER, &pollLibmapper, &runnerComms,true);
Task libmapperUpdate (LIBMAPPER_UPDATE_RATE, TASK_FOREVER, &updateLibmapper, &runnerComms,true);
#endif
Task OSCupdate (OSC_UPDATE_RATE, TASK_FOREVER, &updateOSC, &runnerComms,true);

// Haptic Tasks
Task HapticUpdate (HAPTICS_UPDATE_RATE, TASK_FOREVER, &updateHaptics, &runnerHaptics,true);
#ifdef VISUAL_FEEDBACK
Task DisplayUpdate (GUI_RATE, TASK_FOREVER, &updateDisplay, &runnerHaptics,true);
Task InputRead (INPUT_READ_RATE,TASK_FOREVER,&readInputs, &runnerHaptics,true);
#endif
//==========Functions for task scheduler===========
#ifdef LIBMAPPER
void pollLibmapper() {
  mpr_dev_poll(dev, 0);
}

void updateLibmapper () {
    // Update libmapper outputs
  mpr_sig_set_value(out_sig_angle, 0, 1, MPR_INT32, &knob.angle_out);
  mpr_sig_set_value(out_sig_velocity, 0, 1, MPR_FLT, &knob.velocity);
  mpr_sig_set_value(out_sig_trigger, 0, 1, MPR_INT32, &knob.trigger);
  float speed = abs(knob.velocity);
  mpr_sig_set_value(out_sig_speed, 0, 1, MPR_FLT, &speed);
  mpr_sig_set_value(out_sig_quant_angle, 0, 1, MPR_INT32, &knob.angle_discrete);
  mpr_sig_set_value(out_sig_acceleration, 0, 1, MPR_FLT, &knob.acceleration);
  mpr_dev_update_maps(dev);
}
#endif

#ifdef VISUAL_FEEDBACK
void updateDisplay() {
  if ((gui_state == 0) && ((STATE != OLD_STATE) || (old_gui_state != gui_state))){
    if (old_gui_state != gui_state) {
      old_gui_state = 0;
    }
    if (STATE != OLD_STATE) {
      OLD_STATE = STATE;
    }
    
    lcd.clear();
    lcd.print("Haptic Effect");
    print_state(STATE); 

  }
}

void readInputs() {
  // Check buttons
  uint8_t buttons = lcd.readButtons();
  bool button_pressed = update_btn_lcd(buttons);
  if (button_pressed) {
    if ((buttons & BUTTON_SELECT) || (buttons & BUTTON_RIGHT)) {
      OLD_STATE = STATE;
      STATE = CHANGE_STATE(STATE,MAX_MOTOR_STATES,1);
      knob.set_mode(STATE);
    }
    if (buttons & BUTTON_LEFT) {
      OLD_STATE = STATE;
      STATE = CHANGE_STATE(STATE,MAX_MOTOR_STATES,0);
      knob.set_mode(STATE);
    }
  }
}
#endif

void updateHaptics() {
  // Recieve Angle and velocity from servo
    err = receiveI2C(&knob);
    if (err) {
      //printf("i2c error \n");
    }
    else {
      // Update torque if valid angle measure is recieved.
      if (is_playing) {
        knob.update();
      } else { 
        // OBS: Consider not updating? assign last last value instead? //
        knob.torque = 0;
        knob.target_velocity = 0;
      }
      sendI2C(&knob);
    }
}

void updateOSC() {
  // Sending continuous OSC messages
    if (puara.IP1_ready()) {
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/angle_out");
          lo_send(osc1, oscNamespace.c_str(), "i", knob.angle_out);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/velocity");
          lo_send(osc1, oscNamespace.c_str(), "f", knob.velocity);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/trigger");
          lo_send(osc1, oscNamespace.c_str(), "i", knob.trigger);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/speed");
          float speed = abs(knob.velocity);
          lo_send(osc1, oscNamespace.c_str(), "f", speed);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/angle_discrete");
          lo_send(osc1, oscNamespace.c_str(), "i", knob.angle_discrete);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/accl");
          lo_send(osc1, oscNamespace.c_str(), "f", knob.acceleration);
    }
    if (puara.IP2_ready()) {
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/angle_out");
          lo_send(osc1, oscNamespace.c_str(), "i", knob.angle_out);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/velocity");
          lo_send(osc1, oscNamespace.c_str(), "f", knob.velocity);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/trigger");
          lo_send(osc1, oscNamespace.c_str(), "i", knob.trigger);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/speed");
          float speed = abs(knob.velocity);
          lo_send(osc1, oscNamespace.c_str(), "f", speed);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/angle_discrete");
          lo_send(osc1, oscNamespace.c_str(), "i", knob.angle_discrete);
          oscNamespace.replace(oscNamespace.begin()+baseNamespace.size(),oscNamespace.end(), "instrument/accl");
          lo_send(osc1, oscNamespace.c_str(), "f", knob.acceleration);
    }
}

// Set up multithreading
#define HAPTIC_CPU 0
#define COMMS_CPU 1

// ===== rtos task handles =========================
TaskHandle_t tHaptics;
TaskHandle_t tMappings;

// Mappings
void tHapticTasks(void* parameters)  {
  for(;;){
    runnerHaptics.execute();
  }
}

void tMappingTasks(void* parameters) {
  for(;;){
    runnerComms.execute();
  }
}

void createCoreTasks() {
  xTaskCreatePinnedToCore(
    tHapticTasks,
    "haptics",
    8096,
    NULL,
    2,
    &tHaptics,
    HAPTIC_CPU);

  xTaskCreatePinnedToCore(
    tMappingTasks,   /* Task function. */
    "comms",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    2,           /* priority of the task */
    &tMappings,  /* task handle */
    COMMS_CPU);
}




//*************************************SETUP*************************************//
void setup() {
  #ifdef VISUAL_FEEDBACK
  // Setup LCD
  lcd.begin(16,2);
  lcd.print("Booting up");
  #endif

  // // Start serial
  // Serial.begin(115200);

  // Start Puara
  puara.start();
  puara.set_version(firmware_version);
  baseNamespace.append(puara.get_dmi_name());
  baseNamespace.append("/");
  oscNamespace = baseNamespace;

  // Populating liblo addresses and server port
  std::cout << "    Initializing Liblo server/client... ";
  osc1 = lo_address_new(puara.getIP1().c_str(), puara.getPORT1Str().c_str());
  osc2 = lo_address_new(puara.getIP2().c_str(), puara.getPORT2Str().c_str());
  osc_server = lo_server_thread_new(puara.getLocalPORTStr().c_str(), error);

  // Add method that will match any path and args and start server
  lo_server_thread_add_method(osc_server, NULL, NULL, generic_handler, NULL);
  lo_server_thread_start(osc_server);
  std::cout << "done" << std::endl;


  #ifdef LIBMAPPER
  std::cout << "    Initializing Libmapper device/signals... ";
  init_mpr_sigs();
  std::cout << "done" << std::endl;
  #endif

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(I2CUPDATE_FREQ); // Fast mode plus

  // Make a reading for initilization
  int err = 1;
  #ifdef VISUAL_FEEDBACK
  lcd.clear();
  lcd.print("Waiting for I2C");
  #endif
  while (err) {
    err = receiveI2C(&knob);
    #ifdef VISUAL_FEEDBACK
    lcd.setCursor(0, 1);
    lcd.print(millis()/1000);
    #endif
  }
  knob.set_mode(TorqueTuner::LINSPRING);

  // Show current haptic effect
  #ifdef VISUAL_FEEDBACK
  gui_state = 0; 
  old_gui_state = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Haptic Effect");
  print_state(STATE);
  #endif

  pinMode(SEL_PIN, INPUT);

  // Print Stuff to Serial
  // Using Serial.print and delay to prevent interruptions
  delay(500);
  Serial.println(); 
  Serial.println(puara.get_dmi_name().c_str());
  Serial.println("Edu Meneses\nMetalab - Société des Arts Technologiques (SAT)\nIDMIL - CIRMMT - McGill University");
  Serial.print("Firmware version: "); Serial.println(firmware_version); Serial.println("\n");
  
  // Create tasks
  createCoreTasks();

}

void loop() {
  //runnerHaptics.execute();
  //runnerComms.execute();
}