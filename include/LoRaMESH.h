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
        classA, // Deep sleep mode
        classB, // Undocumented
        classC  // Normal operation mode
    };
 
    // Only applicable in class A mode
    enum{
        window5s, // Define the interval of  5s when the windown still open for recive before deep sleep mode
        window10s,// Define the interval of 10s when the windown still open for recive before deep sleep mode
        window15s // Define the interval of 15s when the windown still open for recive before deep sleep mode
    };

    enum{
        bw125khz, // bandwitdh of 125 khz
        bw250khz, // bandwitdh of 250 khz
        bw500khz  // bandwitdh of 500 khz
    };

    enum{
        cr45 = 1, // Coding rate of 4/5
        cr46,     // Coding rate of 4/6
        cr47,     // Coding rate of 4/7
        cr48      // Coding rate of 4/8
    };

    // Indicates whether the task ended successfully or failed 
    typedef enum{
        MESH_OK,
        MESH_ERROR
    } MeshStatus;

};



#endif