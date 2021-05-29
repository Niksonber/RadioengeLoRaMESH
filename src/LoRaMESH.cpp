#include "LoRaMESH.h"

using namespace LoRaMESHNS;

LoRaMESH::LoRaMESH():_id(0), _net(-1), _uniqueId(-1), _begin(false), _hSerial(NULL){
    memset(_frame.buffer, 0, MAX_BUFFER_SIZE);
    memset(_rcvFrame.buffer, 0, MAX_BUFFER_SIZE);

}

void LoRaMESH::begin(uint8_t rxPin, uint8_t txPin, uint32_t baudRate, uint8_t uart){
    if(_begin) return;

    #if ESP32
        static HardwareSerial radioSerialCommands(uart);
        radioSerialCommands.begin(baudRate, SERIAL_8N1, rxPin, txPin);
    #else
        static SoftwareSerial radioSerialCommands(rxPin, txPin);
        radioSerialCommands.begin(baudRate);
    #endif
    _hSerial = &radioSerialCommands;

    /* Run local read */
    while(localRead(&_id, &_net, &_uniqueId)!=MESH_OK){
        delay(1000);
    }
    Serial.print("previus stored: id: ");
    Serial.print(_id);
    Serial.print(" net: ");
    Serial.print(_net);
    Serial.print(" uniqueID: ");
    Serial.println(_uniqueId);
    _begin = true;
}


mesh_status_t LoRaMESH::localRead(uint16_t *id, uint16_t *net, uint32_t *uniqueId){
    uint8_t size = 3, payload[31] = {0};

    // Asserts parameters 
    if(net == NULL) return MESH_ERROR;
    if(uniqueId == NULL) return MESH_ERROR;
    if(_hSerial == NULL) return MESH_ERROR;

    if (request(id, CMD_LOCALREAD, payload, &size) != MESH_OK)
        return MESH_ERROR;

    *net = *((uint16_t *) &payload[0]);
    *uniqueId = *((uint32_t *) &payload[2]);

    // update interanal variables
    _id = *id;
    _net = *net;
    _uniqueId = *uniqueId;
    
    return MESH_OK; 
}

mesh_status_t LoRaMESH::prepareFrame(uint16_t id, uint8_t command, uint8_t* payload, uint8_t size){
    // Asserts parameters
    if(payload == NULL) return MESH_ERROR;
    if(id > MAX_VALUE) return MESH_ERROR;

    _frame.size = size + 5;

    // Loads the target's ID 
    uint2buffer(&_frame.buffer[0], id);

    // Loads the command 
    _frame.buffer[2] = command;

    if((size >= 0) && (size < MAX_PAYLOAD_SIZE)){
        // Loads the payload on frame
        memcpy(&(_frame.buffer[3]), payload, size);

        // Calc CRC-16
        uint2buffer(&_frame.buffer[size+3], calcCRC((&_frame.buffer[0]), size+3));
    }
    else{
        // Invalid payload size
        // clear frame 
        memset(&_frame.buffer[0], 0, MAX_BUFFER_SIZE);
        return MESH_ERROR;
    }
    _frame.command = true;

    return MESH_OK;
}

mesh_status_t LoRaMESH::sendPacket(){
    // Asserts parameters
    if(_frame.size == 0) return MESH_ERROR;
    if((_hSerial == NULL) && (_frame.command)) return MESH_ERROR;

    Serial.print("Sending frame: ");
    print(_frame.buffer, _frame.size);

    if(_frame.command)
        _hSerial->write(_frame.buffer, _frame.size);
    
    else
        _hSerial->write(_frame.buffer, _frame.size);
    
    return MESH_OK;
}

mesh_status_t LoRaMESH::receivePacket(uint16_t* id, uint8_t* command, uint8_t* payload, uint8_t* size, uint32_t timeout){
    uint16_t waitNextByte = 500;
    uint8_t i = 0;
    uint16_t *rcvId, crc = 0;

    // Assert parameters 
    if(id == NULL) return MESH_ERROR;
    if(command == NULL) return MESH_ERROR;
    if(payload == NULL) return MESH_ERROR;
    if(size == NULL) return MESH_ERROR;
    if(_hSerial == NULL) return MESH_ERROR;

    #ifndef ESP32
        if(!_hSerial->isListening()) _hSerial->listen();
    #endif

    //Waits for reception
    while( ((timeout > 0 ) || (i > 0)) && (waitNextByte > 0) ){
        if(_hSerial->available() > 0){
            _rcvFrame.buffer[i++] = _hSerial->read();
            waitNextByte = 500;
        }
        if(i > 0) waitNextByte--;
        timeout--;
        delay(1);
    }

    // In case it didn't get any data
    if((timeout == 0) && (i == 0)) return MESH_ERROR;

    // Checks CRC16 
    crc = (uint16_t)_rcvFrame.buffer[i-2] | ((uint16_t)_rcvFrame.buffer[i-1] << 8);
    if(calcCRC(&_rcvFrame.buffer[0], i-2) != crc) return MESH_ERROR;

    // Copies ID, command, size, payload
    rcvId = (uint16_t *) &_rcvFrame.buffer[0];
    *id = *rcvId;
    *command = _rcvFrame.buffer[2];
    *size = i-5;
    _rcvFrame.size = i;
    memcpy(payload, &_rcvFrame.buffer[3], i-5);
    
    Serial.print("Recived frame: ");
    print(_rcvFrame.buffer, _rcvFrame.size);

    return MESH_OK;
}
    

uint16_t LoRaMESH::calcCRC(uint8_t* data_in, uint16_t length){
    uint16_t i;
    uint8_t bitbang, j;
    uint16_t crc_calc;

    crc_calc = 0xC181;
    for(i=0; i<length; i++){
        crc_calc ^= (((uint16_t)data_in[i]) & 0x00FF);

        for(j=0; j<8; j++){
            bitbang = crc_calc;
            crc_calc >>= 1;
            
            if(bitbang & 1) crc_calc ^= 0xA001;
            
        }
    }
    return (crc_calc&0xFFFF);
}

void LoRaMESH::serialFlush(){
    while(_hSerial->available() > 0){
        _hSerial->read();
    }
}

mesh_status_t LoRaMESH::setLowPowerMode(uint16_t id, uint8_t mode, uint8_t window){ 
    uint8_t size = 4, payload[31] = {0};

    // Fill buffer - 0x00(1B) - mode(1B) - window(1B) - 0x00(1B) 
    payload[1] = (uint8_t)mode;
    payload[2] = (uint8_t)window;

    if(request(&id, CMD_CLASSPOWER, payload, &size)!=MESH_OK)
        return MESH_ERROR;

    return MESH_OK;
}

mesh_status_t LoRaMESH::storeID(uint16_t id){
    return storeID(id, 0, _uniqueId);
}

mesh_status_t LoRaMESH::storeID(uint16_t id, uint16_t net, uint32_t uniqueID){
    uint8_t size = 11, payload[31] = {0};

    // Assertion 
    if(id > MAX_VALUE || net > MAX_VALUE)
        return MESH_ERROR;

    // Fill buffer - net(2B) - uid(4B) - 0x00(5B) - CRC(2B) 
    uint2buffer(&payload[0], net);
    uint2buffer(&payload[2], uniqueID);
        
    if(request(&id, CMD_WRITECONFIG, payload, &size)!=MESH_OK)
        return MESH_ERROR;

    _id = id;
    return MESH_OK;    
}

mesh_status_t LoRaMESH::storeNet(uint16_t net){
    uint16_t id = 0;
    uint8_t size = 5, payload[31] = {0};

    // Assertion 
    if(id > MAX_VALUE || net>MAX_VALUE) return MESH_ERROR;
    
    //Fill payload - net(2B) - uid(4B) - 0x00(5B) - CRC(2B) 
    payload[0] = 0x04;
    uint2buffer(&payload[1], net);

    if(request(&id, CMD_WRITEPASSWORD, payload, &size)!=MESH_OK)
        return MESH_ERROR;

    _net = net;
    return MESH_OK;    
}

mesh_status_t LoRaMESH::configLoRa(uint16_t id, uint8_t power, uint8_t bw, uint8_t sf, uint8_t cr){
    uint8_t size = 5, payload[5];

    // Assertion
    if(id > MAX_VALUE || bw > 2 || sf <7 || sf > 12 || cr < 1 || cr > 4)
        return MESH_ERROR;

    // Fill payload - write(1B) - power(1B) - bw(1B) - sf(1B) - cr(1B)
    payload[0] = 0x01;
    payload[1] = power;
    payload[2] = bw;
    payload[3] = sf;
    payload[4] = cr;

    if(request(&id, CMD_LORAPARAMETER, payload, &size)!=MESH_OK)
        return MESH_ERROR;

    // Checks if the parameters is correct
    if( payload[1]!=power ||
        payload[2]!=bw ||
        payload[3]!=sf ||
        payload[4]!=cr)
        return MESH_ERROR;

    return MESH_OK;
}

mesh_status_t LoRaMESH::getInfo(uint16_t id, uint8_t command, uint8_t *data, uint8_t *size){
    (*size) = 5;
    memset(data, 0, *size);
    if(command != CMD_READRSSI  && command != CMD_TRACEROUTE &&
       command != CMD_READNOISE && command != CMD_DIAGNOSIS)
        return MESH_ERROR;
    return request(&id, command, data, size);
}

mesh_status_t LoRaMESH::request(uint16_t *id, uint8_t command, uint8_t *payload, uint8_t *size){
    uint8_t rcvCommand;

    if (_hSerial==NULL) return MESH_ERROR;

    if (*id > MAX_VALUE)
        return MESH_ERROR;

    if (prepareFrame(*id, command, payload, (*size))!=MESH_OK)
        return MESH_ERROR;
    
    sendPacket();
    serialFlush();

    if (receivePacket(id, &rcvCommand, payload, size, 5000)!=MESH_OK)
        return MESH_ERROR;

    // Checks if it is a response to the command
    if (rcvCommand != command)
        return MESH_ERROR;
    
    return MESH_OK;   
}

void LoRaMESH::print(uint8_t * buffer, size_t size){
    for(uint8_t i = 0; i < size; i++){
        Serial.print(buffer[i]);
        Serial.print(" ");
    }
    Serial.println();
}