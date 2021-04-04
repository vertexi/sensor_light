/*-----------------------------------------------------------------------------
  '                     RX_DEMO
  '                     -------
  '  - install CC11101_MSP430.h and CC11101_MSP430.cpp as a library (create subdirectory "CC1101"
  '    in your existing sketchbook/libraries directory and copy CC1101_MSP430.* into the CC1101
  '    directory)
  '
  '-----------------------------------------------------------------------------*/

#include "math.h"
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
int8_t pos_vector[6];
uint8_t pos_vector_status;

uint8_t ledPin1 = 5;    // LED1 connected to digital pin 5
uint8_t ledPin2 = 6;    // LED1 connected to digital pin 6

void rx_init(uint8_t address);
void fill_pos_vector(uint8_t sender, uint8_t channel, int8_t rssi);
uint8_t positioning();
void light_control(uint8_t pos);

__inline__ double __attribute__((const)) divide( double y, double x ) {
                                    // calculates y/x
    union {
        double dbl;
        unsigned long long ull;
    } u;
    u.dbl = x;                      // x = x
    u.ull = ( 0xbfcdd6a18f6a6f52ULL - u.ull ) >> (unsigned char)1;
                                    // pow( x, -0.5 )
    u.dbl *= u.dbl;                 // pow( pow(x,-0.5), 2 ) = pow( x, -1 ) = 1.0/x
    return u.dbl * y;               // (1.0/x) * y = y/x
}

//---------------------------------[SETUP]-----------------------------------
void setup()
{
  // init serial Port for debugging
  Serial.begin(9600);
  Serial.println();
  
  pinMode(ledPin1, OUTPUT);   // sets the pin as output 
  pinMode(ledPin2, OUTPUT);   // sets the pin as output 
  digitalWrite(ledPin1, LOW);        // sets the digital pin 13 off
  digitalWrite(ledPin2, LOW);        // sets the digital pin 13 off

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
    Serial.print("rssi:"); Serial.println((int8_t)Rx_fifo[6]);
    Serial.print("lqi:"); Serial.println(Rx_fifo[7]);

    cc1101_packet_available = FALSE;
    fill_pos_vector(sender, Rx_fifo[5], (int8_t)Rx_fifo[6]);
    if (pos_vector_status == 0x3f)
    {
      light_control(positioning());
      for (int i = 0; i < 6; i++)
      {
        pos_vector[i] = 0;
      }
      pos_vector_status = 0x00;
    }
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

float means[6] = {-51.6904, -49.2619, 7.7699, 8.7001, 10.0467, 7.9498};
float divider[6] = {6.4862, 7.007, 7.7699, 8.7001, 10.0467, 7.9498};
float paras[7] = {-0.4427, 1.3809, 0.2777, 0.5639, 0.8723, 0.6008, 0.80337};

uint8_t positioning()
{
  //standardlize
  for (int i = 0; i < 6; i++)
  {
    pos_vector[i] -= means[i];
    pos_vector[i] *= divide(1, divider[i]);
  }

  //linear transform
  float linear = 0;
  for (int i = 0; i < 6; i++)
  {
    linear += (pos_vector[i]*paras[i]);
  }
  linear += paras[6];

  //logstic
  double result = 0;
  divide(1,(1+powf(2.71828, -linear)));
 
  if (result < 0.5)
  {
    return 1;
  }else
  {
    return 2;
  }
}

void light_control(uint8_t pos)
{
  digitalWrite(ledPin1, LOW);        // sets the digital pin 13 off
  digitalWrite(ledPin2, LOW);        // sets the digital pin 13 off
  if (pos == 1)
  {
    digitalWrite(ledPin1, HIGH);        // sets the digital pin 13 off
  } else
  {
    digitalWrite(ledPin2, HIGH);        // sets the digital pin 13 off
  }
}

void fill_pos_vector(uint8_t sender, uint8_t channel, int8_t rssi)
{
  int index = 0;
  if (sender == 0x02)
  {
    index = 0;
  } else if (sender == 0x03)
  {
    index = 2;
  } else if (sender == 0x04)
  {
    index = 4;
  }

  if (channel == 0x01)
  {
    index++;
  }
  pos_vector[index] = rssi;

  uint8_t status = 0x00;
  switch (index)
  {
    case 0:
      status = 1;
      break;
    case 1:
      status = 2;
      break;
    case 2:
      status = 4;
      break;
    case 3:
      status = 8;
      break;
    case 4:
      status = 16;
      break;
    case 5:
      status = 32;
      break;
  }
  pos_vector_status |= status;
}

void rx_init(uint8_t address)
{
  // init CC1101 RF-module and get My_address from EEPROM
  RF.begin(My_addr);                   //inits RF module with main default settings

  RF.sidle();                          //set to ILDE first

  RF.set_mode(0x01);                   //set modulation mode 1 = GFSK_1_2_kb; 2 = GFSK_38_4_kb; 3 = GFSK_100_kb; 4 = MSK_250_kb; 5 = MSK_500_kb; 6 = OOK_4_8_kb
  RF.set_ISM(0x02);                    //set ISM Band 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
  RF.set_channel(0x50);                //set channel
  RF.set_output_power_level(10);        //set PA level in dbm
  RF.set_myaddr(address);                 //set my own address
  My_addr = address;

  RF.spi_write_register(IOCFG2, 0x06);

  RF.show_register_settings();         //shows current CC1101 register values
  RF.receive();                        //set to RECEIVE mode

  Serial.println(F("CC1101 RX Demo for MSP430"));   //welcome message
}
