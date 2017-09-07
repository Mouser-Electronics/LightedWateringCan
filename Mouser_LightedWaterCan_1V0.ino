/*
     Debs Watering Project
    	Copyright (C) 2017  Mouser Electronics

   	 This program is free software: you can redistribute it and/or modify
  	  it under the terms of the GNU General Public License as published by
   	 the Free Software Foundation, either version 3 of the License, or
    	any later version.

    	This program is distributed in the hope that it will be useful,
    	but WITHOUT ANY WARRANTY; without even the implied warranty of
    	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    	GNU General Public License for more details.

    	You should have received a copy of the GNU General Public License
    	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// 0V1 - Get menu Sending
// 0v2 - Get menu responding
// 0v3 - Get commands running 
// 0v4 - Get BLE UART working
// 0v5 - bring IR Code into BLE code
// 0v6 - Fix New Line Commands
// 1v0 - Revise to standard, update colors and order, and remove unnessary code 

/*********************************************************************
 This is an example for Adafruit nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

//Grab Libraries relevant to IR 
#include <IRLibSendBase.h>
#include <IRLib_P01_NEC.h>
#include <IRLibCombo.h>
IRsend mySender; //create an instance of the sending object

#include "BluefruitConfig.h"

/* This example demonstrates how to use Bluefruit callback API :
 * - setConnectCallback(), setDisconnectCallback(), setBleUartRxCallback(),
 * setBleGattRxCallback() are used to install callback function for specific
 * event. 
 * - Furthermore, update() must be called inside loop() for callback to
 * be executed.
 * 
 * The sketch will add an custom service with 2 writable characteristics,
 * and install callback to execute when there is an update from central device
 * - one hold string
 * - one hold a 4-byte integer
  */

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE     Perform a factory reset when running this sketch
   
                            Enabling this will put your Bluefruit LE module
                            in a 'known good' state and clear any config
                            data set in previous sketches or projects, so
                            running this at least once is a good idea.
   
                            When deploying your project, however, you will
                            want to disable factory reset by setting this
                            value to 0.  If you are making changes to your
                            Bluefruit LE device via AT commands, and those
                            changes aren't persisting across resets, this
                            is the reason why.  Factory reset will erase
                            the non-volatile memory where config data is
                            stored, setting it back to factory default
                            values.
       
                            Some sketches that require you to bond to a
                            central device (HID mouse, keyboard, etc.)
                            won't work at all with this feature enabled
                            since the factory reset will clear all of the
                            bonding data stored on the chip, meaning the
                            central device won't be able to reconnect.
                            
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features    
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE        1
    #define MINIMUM_FIRMWARE_VERSION   "0.7.0"
/*=========================================================================*/


/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


//Define Global Variables
int32_t charid_string;
int32_t charid_number;
uint8_t protocol = 1; // This refers to the library for sending IR signals our code uses NEC or Protocol 1
uint32_t instruction; // The user selection
uint32_t code; // The code that will be sent based on user selection
uint16_t bits = 0; // Any additional bits used for sending IR Signals
bool OnConnect; // Used to open and send menu commands 
bool OnRecieved; // Used to issue IR commands


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void connected(void)
{
  Serial.println( F("Connected") );
  OnConnect = true; 
}

void disconnected(void)
{
  Serial.println( F("Disconnected") );
}

void BleUartRX(char data[], uint16_t len)
{
  Serial.print( F("[BLE UART RX]" ) );
  Serial.write(data, len);
  instruction = atoi(data); 
  OnRecieved = true;
  Serial.println();
}

void decision_tree()
{
  Serial.print("Define Code here");
  //Conversion from user instuction to code needed for function
  switch(instruction){
    case 0:code = 0xFFE21D;break;
    case 1:code = 0xFFA25D;break;
    case 2:code = 0xFF629D;break;
    case 3:code = 0xFF22DD;break;
    case 4:code = 0xFF02FD;break;
    case 5:code = 0xFFC23D;break;
    case 6:code = 0xFFE01F;break;
    case 7:code = 0xFFA857;break;
    case 8:code = 0xFF906F;break;
    case 9:code = 0xFF6897;break;
    case 10:code = 0xFF9867;break;
    case 11:code = 0xFFB04F;break;
    case 12:code = 0xFF30CF;break;
    case 13:code = 0xFF18E7;break;
    case 14:code = 0xFF7A85;break;
    case 15:code = 0xFF10EF;break;
    case 16:code = 0xFF38C7;break;
    case 17:code = 0xFF5AA5;break;
    case 18:code = 0xFF42BD;break;
    case 19:code = 0xFF4AB5;break;
    case 20:code = 0xFF52AD;break;
    default:
      //If Nothing else do this
      code = 0xFFFFFF;
      break;
  }
}

void BleGattRX(int32_t chars_id, uint8_t data[], uint16_t len)
{
  Serial.print( F("[BLE GATT RX] (" ) );
  Serial.print(chars_id);
  Serial.print(") ");
  
  if (chars_id == charid_string)
  {  
    Serial.write(data, len);
    Serial.println();
  }else if (chars_id == charid_number)
  {
    int32_t val;
    memcpy(&val, data, len);
    Serial.println(val);
  }
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Callbacks Example"));
  Serial.println(F("-------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }
  
  if ( !ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    error( F("Callback requires at least 0.7.0") );
  }

  Serial.println( F("Adding Service 0x1234 with 2 chars 0x2345 & 0x6789") );
  ble.sendCommandCheckOK( F("AT+GATTADDSERVICE=uuid=0x1234") );
  ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0x2345,PROPERTIES=0x08,MIN_LEN=1,MAX_LEN=6,DATATYPE=string,DESCRIPTION=string,VALUE=abc"), &charid_string);
  ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0x6789,PROPERTIES=0x08,MIN_LEN=4,MAX_LEN=4,DATATYPE=INTEGER,DESCRIPTION=number,VALUE=0"), &charid_number);

  ble.reset();

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();
  
  /* Set callbacks */
  ble.setConnectCallback(connected);
  ble.setDisconnectCallback(disconnected);
  ble.setBleUartRxCallback(BleUartRX);
  
  /* Only one BLE GATT function should be set, it is possible to set it 
  multiple times for multiple Chars ID  */
  ble.setBleGattRxCallback(charid_string, BleGattRX);
  ble.setBleGattRxCallback(charid_number, BleGattRX);
}



/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{  
  ble.update(200);
  // Respond to settings made in the callback for connection
  if(OnConnect){
    delay(2000);
    ble.print("AT+BLEUARTTX=");ble.println(F("Enter the Number for a command \n\r"));delay(10);
    ble.print("AT+BLEUARTTX=");ble.println(F("0:OFF     1:ON       2:TIMER   \n\r"));delay(10);
    ble.print("AT+BLEUARTTX=");ble.println(F("3:Red     4:Green    5:Blue    \n\r"));delay(10);
    ble.print("AT+BLEUARTTX=");ble.println(F("6:LtGreen 7:Jade     8:Mint    \n\r"));delay(10);
    ble.print("AT+BLEUARTTX=");ble.println(F("9:Aqua    10:SkyBlue 11:Cyan   \n\r"));delay(10);
    ble.print("AT+BLEUARTTX=");ble.println(F("12:DkBlue 13:Lavendr 14:Violet \n\r"));delay(10);
    ble.print("AT+BLEUARTTX=");ble.println(F("15:White  16:FADE    17:JUMP   \n\r"));delay(10);
    ble.print("AT+BLEUARTTX=");ble.println(F("18:RESET  19:BrightD 20:BrightU\n\r"));delay(10);
    OnConnect = false;
  }
  //Apply changes that result from communications
  if(OnRecieved){
    ble.print("AT+BLEUARTTX=");ble.println(instruction);delay(10); // AT command, uart, allow uart to timeout and function
    decision_tree(); //Send off for code from user instruction
    ble.print("AT+BLEUARTTX=");ble.println(code,HEX);delay(10);
    mySender.send(protocol, code, bits);
    OnRecieved = false; //disable loop
    OnConnect = true; //send menu again
  }
}

