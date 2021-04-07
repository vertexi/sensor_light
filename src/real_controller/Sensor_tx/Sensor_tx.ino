/*-----------------------------------------------------------------------------
  '                     TX_DEMO
  '                     -------

  ' - install CC11101_MSP430.h and CC11101_MSP430.cpp as a library (create subdirectory "CC1101"
  '    in your existing sketchbook/libraries directory and copy CC1101_MSP430.* into the CC1101
  '    directory)
  '
  '-----------------------------------------------------------------------------*/
#include <CC1101_MSP430.h>

//--------------------------[Global CC1100 variables]------------------------
uint8_t Tx_fifo[FIFOBUFFER], Rx_fifo[FIFOBUFFER];
uint8_t My_addr, Tx_addr, Rx_addr, Pktlen, pktlen, Lqi, Rssi;
uint8_t rx_addr, sender, lqi;
int8_t rssi_dbm;


//--------------------------[class constructors]-----------------------------
//init CC1100 constructor
CC1101 RF;

//--------------------------[Sensor input]----------------------------------
// int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0; // variable to store the value coming from the sensor
int ack_reties = 5;  // the maximun times to try resend packet if don't recive the ack
int posValue = 0;    // the test position potentiometer
int posPin = A0;     // the potentiometer input pin

uint8_t channels[3] = {0x97, 0xf0, 0x01};
#define total_channel 3

void tx_init();
void send_data(uint8_t Pktlen, uint8_t Rx_addr, uint8_t channel, int sensorValue);
void multi_channel_send(uint8_t Pktlen, uint8_t Rx_addr, int sensorValue);

//---------------------------------[SETUP]-----------------------------------
void setup()
{
  // init serial Port for debugging
  Serial.begin(9600);
  Serial.println();

  uint8_t add_ = 0x05;
  tx_init(add_);

  pinMode(posPin, INPUT);
}
//---------------------------------[LOOP]-----------------------------------

int sensor_threshold = 100;
bool sensor_status = false;
bool senstat=TRUE;

void loop()
{
  delay(4000);
  // read the value from the sensor:
  // sensorValue = analogRead(sensorPin);
  // read the value from the sensor:
  posValue = analogRead(posPin);
  Serial.print("sensor:");Serial.println(posValue);

  Tx_fifo[3] = (uint8_t)(posValue >> 8);
  Tx_fifo[4] = (uint8_t)(posValue);
  //split 16-Bit sensor data to 2 byte array
  
  Pktlen = 0x06;                                               //set packet len to 0x13
  Rx_addr = 0x02;
  send_data(Pktlen, Rx_addr, 0x97, posValue);
  send_data(Pktlen, Rx_addr, 0x01, posValue);
  Rx_addr = 0x03;
  send_data(Pktlen, Rx_addr, 0x97, posValue);
  send_data(Pktlen, Rx_addr, 0x01, posValue);
  Rx_addr = 0x04;
  send_data(Pktlen, Rx_addr, 0x97, posValue);
  send_data(Pktlen, Rx_addr, 0x01, posValue);
  
/*
  if (sensorValue > sensor_threshold && (!sensor_status)) 
  {
    Tx_fifo[3] = (uint8_t)(sensorValue >> 8);                    //split 16-Bit sensor data to 2 byte array
    Tx_fifo[4] = (uint8_t)(sensorValue);
    RF.send_packet(My_addr, Rx_addr, Tx_fifo, Pktlen, ack_reties);   //sents package over air. ACK is received via GPIO polling
    sensor_status = !sensor_status;
  } else if (sensorValue <= sensor_threshold && sensor_status) 
  {
    Tx_fifo[3] = (uint8_t)(sensorValue >> 8);                    //split 16-Bit sensor data to 2 byte array
    Tx_fifo[4] = (uint8_t)(sensorValue);
    RF.send_packet(My_addr, Rx_addr, Tx_fifo, Pktlen, ack_reties);   //sents package over air. ACK is received via GPIO polling
    sensor_status = !sensor_status;
  }
*/
}
//--------------------------[end loop]----------------------------

void multi_channel_send(uint8_t Pktlen, uint8_t Rx_addr, int sensorValue)
{
  for (int i = 0; i < total_channel; i++)
  {
    send_data(Pktlen, Rx_addr, channels[i], sensorValue);
  }
}

void send_data(uint8_t Pktlen, uint8_t Rx_addr, uint8_t channel, int sensorValue)
{
  bool tx_status = FALSE;
  Tx_fifo[5] = channel; //  prepare the init channel

  do
  {
    tx_status = RF.send_packet(My_addr, Rx_addr, Tx_fifo, Pktlen, ack_reties);
    //sents package over air. ACK is received via GPIO polling
  }while(!tx_status);
  delay(100);
  Serial.print("next channel:");Serial.println(channel);

  RF.set_channel(channel); // set to next channel. prepare to send.
  RF.receive();                        //set to RECEIVE mode
}

void tx_init(uint8_t address)
{
                                                  // init CC1101 RF-module and get My_address from EEPROM
  RF.begin(My_addr);                              // inits RF module with main default settings

  RF.sidle();                                     // set to ILDE first
  RF.set_mode(0x01);                              // set modulation mode 1 = GFSK_1_2_kb; 2 = GFSK_38_4_kb; 3 = GFSK_100_kb; 4 = MSK_250_kb; 5 = MSK_500_kb; 6 =  OOK_4_8_kb
  RF.set_ISM(0x02);                               // set frequency 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
  RF.set_channel(0x01);                           // set channel
  RF.set_output_power_level(5);                  // set PA level in dbm
  RF.set_myaddr(address);                         // set my own address
  My_addr = address;

  RF.show_register_settings();                    // shows current CC1101 register values
  RF.receive();                                   // set to RECEIVE mode

  Serial.println(F("CC1101 TX Demo for MSP430")); // welcome message
}
