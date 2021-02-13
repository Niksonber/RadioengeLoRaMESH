/**
 * Author: Nikson Bernardes
 * Based on: Radioenge Equipamentos de Telecomunicações LoRaMESH.h LoRaMESH.cpp for Arduíno and User manual
 * Contains similar or equal passages those presents in LoRaMESH.h LoRaMESH.cpp by Radioenge Equipamentos de Telecomunicações
 * Updated: 13/02/2021
 * **/

#ifndef RADIOENGELORAMESH
#define RADIOENGELORAMESH

#include <Arduino.h>
#include <SoftwareSerial.h>

#define MAX_PAYLOAD_SIZE 232
#define MAX_BUFFER_SIZE 237

// Set of constants for Radioenge LoRaMESH Module
namespace LoRaMESHNS{

    // Commands codes
    enum{
        CMD_CLASSPOWER      = 0xC1, // Set energy economy mode 
        CMD_WRITECONFIG     = 0xCA, // Set ID and NET (Documented), but only ID is defined
        CMD_LORAPARAMETER   = 0xD6, // Get and Set LoRa parameters
        CMD_WRITEPASSWORD   = 0xCD, // Set passwod (Documented), but set NET
        CMD_LOCALREAD       = 0xE2, // Gets the ID, NET and unique ID from device (serial connected)
        CMD_REMOTEREAD      = 0xD4, // Gets the ID, NET and unique ID from device (LoRa connected)
        CMD_GPIOCONFIG      = 0xC2, // Configures a given GPIO pin to a desired mode, gets or sets its value
        CMD_DIAGNOSIS       = 0xE7, // Gets diagnosis information from the device 
        CMD_READNOISE       = 0xD8, // Reads the noise floor on the current channel 
        CMD_READRSSI        = 0xD5, // Reads the RSSI between the device and a neighbour 
        CMD_TRACEROUTE      = 0xD2, // Traces the hops from the device to the master 
        CMD_SENDTRANSP      = 0x28  // Sends a packet to the device's transparent serial port 
    };


    // A class type define the energy economy mode 
    enum{
        CLASS_A, // Deep sleep mode
        CLASS_B, // Undocumented
        CLASS_C  // Normal operation mode
    };
 
    // Only applicable in class A mode
    enum{
        WINDOW_5s, // Define the interval of  5s when the windown still open for recive before deep sleep mode
        WINDOW_10s,// Define the interval of 10s when the windown still open for recive before deep sleep mode
        WINDOW_15s // Define the interval of 15s when the windown still open for recive before deep sleep mode
    };

    enum{
        BW125KHZ, // bandwitdh of 125 khz
        BW250KHZ, // bandwitdh of 250 khz
        BW500KHZ  // bandwitdh of 500 khz
    };

    enum{
        CR45 = 1, // Coding rate of 4/5
        CR46,     // Coding rate of 4/6
        CR47,     // Coding rate of 4/7
        CR48      // Coding rate of 4/8
    };

    // GPIO type
    typedef enum{
        GPIO0,
        GPIO1,
        GPIO2,
        GPIO3,
        GPIO4,
        GPIO5,
        GPIO6,
        GPIO7
    } gpio_t;

    // GPIO mode
    enum{
        DIGITAL_IN,
        DIGITAL_OUT,
        ANALOG_IN = 3
    };

    enum{
        PULL_OFF,
        PULLUP,
        PULLDOWN
    };

    // Indicates whether the task ended successfully or failed 
    typedef enum{
        MESH_OK,
        MESH_ERROR
    } mesh_status_t;

    // Frame data struct
    typedef struct{
        uint8_t buffer[MAX_BUFFER_SIZE];
        uint8_t size;
        bool command;
    } frame_t;

};


// Radioenge LoRaMESH Module handler class
class LoRaMESH{
public:
    /// Constructor @param rxPin RX pin @param txPin TX pin  @param baudRate baudrate between 9600 and 57600    
    LoRaMESH(uint8_t rxPin, uint8_t txPin, uint32_t baudRate=9600);
    
    /// Prepare frame with header (ID(2B) + Command(1B)) and CRC as tail @return MESH_ERROR if some error occured else MESH_OK
    /// @param id Device's ID @param command Command byte e.g. CMD_CLASSPOWER @param payload: Pointer to payload array @param payloadsize: payload size
    LoRaMESHNS::mesh_status_t prepareFrame(uint16_t id, uint8_t command, uint8_t* payload, uint8_t payloadsize);
    
    
    /// Send frame via serial (needs to be previously prepared) @return MESH_ERROR if some error occured else MESH_OK
    LoRaMESHNS::mesh_status_t sendPacket();
    
 
    /// Receives a packet @return MESH_ERROR if some error occured else MESH_OK
    /// @param id[out] Device's ID @param command Command byte recived e.g. CMD_CLASSPOWER @param payload[out] Pointer to array where payload should be copied @param payloadsize[out] recived payload size @param timeout timeout in milliseonds
    LoRaMESHNS::mesh_status_t receivePacket(uint16_t* id, uint8_t* command, uint8_t* payload, uint8_t* payloadSize, uint32_t timeout);
    
    
    /// Gets the ID, NET and UNIQUE ID  @return MESH_ERROR if some error occured else MESH_OK
    /// @param id[out] Device's ID @param net[out] Configured NET @param uniqueId[out]:  Unique ID 
    LoRaMESHNS::mesh_status_t localRead(uint16_t* id, uint16_t* net, uint32_t* uniqueId);
    

    /// Calc CRC-16 from buffer @return CRC-16 
    /// @param  data: Pointer to the input buffer @param  length: Buffer size
    uint16_t   calcCRC(uint8_t* data, uint16_t length);

    /// Sets low power mode @return MESH_ERROR if some error occured else MESH_OK
    /// @param id Device's ID @param mode Class mode of operation, e.g. classA @param window only applicable in class A 
    LoRaMESHNS::mesh_status_t  setLowPowerMode(uint16_t id, uint8_t mode = LoRaMESHNS::CLASS_A, uint8_t window=LoRaMESHNS::WINDOW_10s);

    /// Define ID device @return MESH_ERROR if some error occured else MESH_OK
    /// @param id id of device @param net network, but isnt necessary @param uniqueID call LocalRead to get it 
    LoRaMESHNS::mesh_status_t storeID(uint16_t id, uint16_t net, uint32_t uniqueID);
    
    /// Define NET device @return MESH_ERROR if some error occured else MESH_OK
    /// @param net network 
    LoRaMESHNS::mesh_status_t storeNet(uint16_t net);

protected:
    uint16_t _id, _net;
    uint32_t _uniqueId;
    LoRaMESHNS::frame_t _frame;
    SoftwareSerial * _hSerial;

    void serialFlush();
};


#endif