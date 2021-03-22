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
volatile uint8_t cc1101_packet_available;

//--------------------------[class constructors]-----------------------------
//init CC1100 constructor
CC1101 RF;

//--------------------------[Sensor input]----------------------------------
int sensorValue = 0;  // variable to store the value coming from the sensor
int sensor_threshold = 100;
// most launchpads have a red LED
#define LED RED_LED
#define LED1 P2_3

void rx_init(uint8_t address);

//---------------------------------[SETUP]-----------------------------------
void setup()
{
  // init serial Port for debugging
  Serial.begin(9600);
  Serial.println();

  uint8_t add_ = 0x01;
  rx_init(add_);
}

//---------------------------------[LOOP]-----------------------------------
void loop()
{
  /*----------------------------------------------
    - Here we are expecting a 7-byte message.
    - Byte[6:3] contain a 32-bit timestamp.
    ------------------------------------------------*/

  //if valid package is received
  if (cc1101_packet_available == TRUE)
  {
    sensorValue = (uint16_t)(Rx_fifo[3] << 8) +
                  Rx_fifo[4];

    Serial.print(F("TX_data: ")); Serial.print(sensorValue);
    Serial.print("Sender:"); Serial.println(sender);
    Serial.print("next freq:");Serial.println(Rx_fifo[5]);
    Serial.print("rssi:"); Serial.println(Rx_fifo[6]);
    Serial.print("lqi:"); Serial.println(Rx_fifo[7]);

    cc1101_packet_available = FALSE;
  }

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
//--------------------------[end loop]----------------------------

void rx_init(uint8_t address)
{
  // init CC1101 RF-module and get My_address from EEPROM
  RF.begin(My_addr);                   //inits RF module with main default settings

  RF.sidle();                          //set to ILDE first

  RF.set_mode(0x01);                   //set modulation mode 1 = GFSK_1_2_kb; 2 = GFSK_38_4_kb; 3 = GFSK_100_kb; 4 = MSK_250_kb; 5 = MSK_500_kb; 6 = OOK_4_8_kb
  RF.set_ISM(0x02);                    //set ISM Band 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
  RF.set_channel(0x01);                //set channel
  RF.set_output_power_level(10);        //set PA level in dbm
  RF.set_myaddr(address);                 //set my own address
  My_addr = address;

  RF.spi_write_register(IOCFG2, 0x06);

  RF.show_register_settings();         //shows current CC1101 register values
  RF.receive();                        //set to RECEIVE mode

  Serial.println(F("CC1101 RX Demo for MSP430"));   //welcome message
}
