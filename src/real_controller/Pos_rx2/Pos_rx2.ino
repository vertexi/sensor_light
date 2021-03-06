/*-----------------------------------------------------------------------------
  '                     RX_DEMO
  '                     -------
  '  - install CC11101_MSP430.h and CC11101_MSP430.cpp as a library (create subdirectory "CC1101"
  '    in your existing sketchbook/libraries directory and copy CC1101_MSP430.* into the CC1101
  '    directory)
  '
  '-----------------------------------------------------------------------------*/

#include <CC1101_MSP430.h>

//---------------------------=[Global variables]=----------------------------
uint16_t rf_timecode = 0;
//--------------------------[Global CC1100 variables]------------------------
uint8_t Tx_fifo[FIFOBUFFER], Rx_fifo[FIFOBUFFER];
uint8_t My_addr, Tx_addr, Rx_addr, Pktlen, pktlen, Lqi, Rssi;
uint8_t rx_addr, sender, lqi;
int8_t rssi_dbm;
volatile uint8_t cc1101_packet_available = FALSE;

//--------------------------[class constructors]-----------------------------
//init CC1100 constructor
CC1101 RF;

//--------------------------[Sensor input]----------------------------------
int sensorValue = 0;  // variable to store the value coming from the sensor
int sensor_threshold = 100;
uint8_t ack_reties = 10;
unsigned long prev_fail_time = 0;
uint8_t fail_channel = 0;
int fail_time = 5000;

uint8_t channels[3] = {0x97, 0xf0, 0x01};
#define total_channel 3

void rx_init(uint8_t address);
void pin_init();
void send_target(int sensorValue, uint8_t current_freq, int8_t rssi_dbm, 
    uint8_t lqi, uint8_t Rx_addr);
void fail_tackle();
void packet_detector();
void get_data();
void send_to_target();

//---------------------------------[SETUP]-----------------------------------
void setup()
{
  // init serial Port for debugging
  Serial.begin(9600);
  Serial.println();

  uint8_t add_ = 0x04;
  rx_init(add_);
  pin_init();
  
  prev_fail_time = millis();// init the failure time
}

//---------------------------------[LOOP]-----------------------------------

void loop()
{
  packet_detector();

  //if valid package is received
  if (cc1101_packet_available == TRUE)
  {
    get_data(); //storage the sensor data and clear the fail_time
    send_to_target(); //send data to target throught 0x50 channel and back to
    // next channel prepare to communicate with the sensor
  } else
  {
    fail_tackle();//when long time don't get the message channel other channel
  }
}
//--------------------------[end loop]----------------------------

void get_data()
{
  sensorValue = (uint16_t)(Rx_fifo[3] << 8) +
    Rx_fifo[4];

  Serial.print("next channel:") ; Serial.println(Rx_fifo[5]) ;
  Serial.print(F("TX_data: "))  ; Serial.print(sensorValue)  ;
  Serial.print("Sender:")       ; Serial.println(sender)     ;
  Serial.print("rssi:")         ; Serial.println(rssi_dbm)   ;
  Serial.print("lqi:")          ; Serial.println(lqi)        ;

  cc1101_packet_available = FALSE;
  prev_fail_time = millis();
  fail_time = 5000;
}

void send_to_target()
{
  uint8_t rx_add_t = 0x01;
  send_target(Rx_fifo[3], Rx_fifo[4], Rx_fifo[5], rssi_dbm, lqi, rx_add_t);

  RF.set_channel(Rx_fifo[5]);
  RF.receive();                        //set to RECEIVE mode
}

void packet_detector()
{
  if (RF.packet_available() == TRUE)
  {
    if (RF.get_payload(Rx_fifo, pktlen, rx_addr, sender, rssi_dbm, lqi) == TRUE) //stores the payload data to Rx_fifo
    {
      cc1101_packet_available = TRUE;                                //set flag that a package is in RX buffer
      Serial.print(F("Received packet!"));
    }
    else
    {
      cc1101_packet_available = FALSE;                               //set flag that an package is corrupted
    }
  }
}


void fail_tackle()
{
  if ((millis() - prev_fail_time) > fail_time)
  {
    //Serial.println("failing!!!");
    fail_time = 100;
    fail_channel++;

    if (!(fail_channel < total_channel))
    {
      fail_channel = 0;
    }
    RF.set_channel(channels[fail_channel]);
    RF.receive();                        //set to RECEIVE mode
    delay(20);

    //Serial.print("try channel:");Serial.println(fail_channel);
    prev_fail_time = millis();
  }
}

void pin_init()
{
}

void rx_init(uint8_t address)
{
  // init CC1101 RF-module and get My_address from EEPROM
  RF.begin(My_addr);             // inits RF module with main default settings

  RF.sidle();                    // set to ILDE first

  RF.set_mode(0x01);             // set modulation mode 1 = GFSK_1_2_kb; 2 =
                                 // GFSK_38_4_kb; 3 = GFSK_100_kb; 4 =
                                 // MSK_250_kb; 5 = MSK_500_kb; 6 = OOK_4_8_kb
  RF.set_ISM(0x02);              // set ISM Band 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
  RF.set_channel(0x01);          // set channel
  RF.set_output_power_level(5); // set PA level in dbm
  RF.set_myaddr(address);        // set my own address
  My_addr = address;

  RF.spi_write_register(IOCFG2, 0x06);

  RF.show_register_settings();         //shows current CC1101 register values
  RF.receive();                        //set to RECEIVE mode

  Serial.println(F("CC1101 RX Demo for MSP430"));   //welcome message
}

void send_target(uint8_t sensor1, uint8_t sensor2, uint8_t next_channel, 
    int8_t rssi_dbm, uint8_t lqi, uint8_t Rx_addr)
{
  uint8_t pktlen = 0x08;                                 // complete Pktlen for ACK packet
  uint8_t tx_buffer[0x08];                               // tx buffer array init

  tx_buffer[3] = sensor1      ; tx_buffer[4] = sensor2 ;
  tx_buffer[5] = next_channel ;
  tx_buffer[6] = rssi_dbm     ; tx_buffer[7] = lqi     ;

  RF.set_channel(0x50);
  RF.receive();                                          // set to RECEIVE mode

  bool tx_status = FALSE;
  do
  {
    tx_status = RF.send_packet(My_addr, Rx_addr, tx_buffer, pktlen, ack_reties);
  }while(!tx_status);

  Serial.println(F("data send to target!"));
}
