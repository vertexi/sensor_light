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
// most launchpads have a red LED
#define LED RED_LED
#define LED1 P2_3
uint8_t ack_reties = 5;
unsigned long prev_fail_time = 0;
uint8_t fail_freq = 1;
int fail_time = 5000;

void rx_init(uint8_t address);
void pin_init();
void send_target(int sensorValue, uint8_t current_freq, int8_t rssi_dbm, 
    uint8_t lqi, uint8_t Rx_addr);

//---------------------------------[SETUP]-----------------------------------
void setup()
{
  // init serial Port for debugging
  Serial.begin(9600);
  Serial.println();

  uint8_t add_ = 0x03;
  rx_init(add_);
  pin_init();
  
  prev_fail_time = millis();// init the failure time
}

//---------------------------------[LOOP]-----------------------------------

void loop()
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

  pktlen = 0x05;
  //if valid package is received
  if (cc1101_packet_available == TRUE)
  {
    sensorValue = ((uint16_t)(Rx_fifo[3] & 0x0f) << 8 ) +
                  Rx_fifo[4];

    uint8_t rx_add_t = 0x01;
    send_target(Rx_fifo[3], Rx_fifo[4], rssi_dbm, lqi, rx_add_t);

    if (Rx_fifo[3] < 0x50)
    {
      RF.set_ISM((uint8_t)(Rx_fifo[3] >> 4));
      RF.receive();                        //set to RECEIVE mode
      Serial.print("next freq:");Serial.println((uint8_t)(Rx_fifo[3] >> 4));
    } else
    {
      RF.set_ISM(0x01);
      RF.receive();                        //set to RECEIVE mode
      Serial.print("next freq:");Serial.println(1);
    }
    Serial.print(F("TX_data: ")); Serial.print(sensorValue);
    Serial.print("Sender:"); Serial.println(sender);
    Serial.print("rssi:"); Serial.println(rssi_dbm);
    Serial.print("lqi:"); Serial.println(lqi);

    cc1101_packet_available = FALSE;

    prev_fail_time = millis();
  } else
  {
    if ((millis() - prev_fail_time) > fail_time)
    {
      Serial.println("failing!!!");
      if (fail_freq == 0x05)
      {
        fail_freq = 0x01;
        RF.set_ISM(fail_freq);
        RF.receive();                        //set to RECEIVE mode
        Serial.print("try freq:");Serial.println(1);
      } else
      {
        RF.set_ISM(fail_freq++);
        RF.receive();                        //set to RECEIVE mode
        Serial.print("try freq:");Serial.println(fail_freq-1);
      }
      prev_fail_time = millis();
    }
  }
}
//--------------------------[end loop]----------------------------

void pin_init()
{
  // initialize the digital pin as an output.
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
}

void rx_init(uint8_t address)
{
  // init CC1101 RF-module and get My_address from EEPROM
  RF.begin(My_addr);             // inits RF module with main default settings

  RF.sidle();                    // set to ILDE first

  RF.set_mode(0x01);             // set modulation mode 1 = GFSK_1_2_kb; 2 =
                                 // GFSK_38_4_kb; 3 = GFSK_100_kb; 4 =
                                 // MSK_250_kb; 5 = MSK_500_kb; 6 = OOK_4_8_kb
  RF.set_ISM(0x01);              // set ISM Band 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
  RF.set_channel(0x01);          // set channel
  RF.set_output_power_level(10); // set PA level in dbm
  RF.set_myaddr(address);        // set my own address
  My_addr = address;

  RF.spi_write_register(IOCFG2, 0x06);

  RF.show_register_settings();         //shows current CC1101 register values
  RF.receive();                        //set to RECEIVE mode

  Serial.println(F("CC1101 RX Demo for MSP430"));   //welcome message
}

void send_target(uint8_t tx1, uint8_t tx2, int8_t rssi_dbm, 
    uint8_t lqi, uint8_t Rx_addr)
{
  uint8_t pktlen = 0x07;                                 // complete Pktlen for ACK packet
  uint8_t tx_buffer[0x07];                               // tx buffer array init

  tx_buffer[3] = tx1      ; tx_buffer[4] = tx2 ;         // fill buffer with ACK Payload
  tx_buffer[5] = rssi_dbm ; tx_buffer[6] = lqi ;         // fill buffer with ACK Payload

  RF.set_ISM(0x01);                                      // set ISM Band 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
  RF.receive();                                          // set to RECEIVE mode

  RF.send_packet(My_addr, Rx_addr, tx_buffer, pktlen, ack_reties);

  Serial.println(F("data send to target!"));
}
