#ifndef RADIOENGELORAMESH
#define RADIOENGELORAMESH

// Set of constants for Radioenge LoRaMESH Module
namespace LoRAMESHNS{

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
    } MeshStatus;

};



#endif