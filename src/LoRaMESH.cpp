#include "LoRaMESH.h"

using namespace LoRaMESHNS;

LoRaMESH::LoRaMESH(uint8_t rxPin, uint8_t txPin, uint32_t baudRate){
  static SoftwareSerial radioSerialCommands(rxPin, txPin);
  radioSerialCommands.begin(baudRate);
  _hSerial = &radioSerialCommands;

  /* Run local read */
  localRead(&_id, &_net, &_uniqueId);
}



mesh_status_t LoRaMESH::localRead(uint16_t *id, uint16_t *net, uint32_t *uniqueId){
    uint8_t bufferPayload[31];
    uint8_t payloadSize, command, i = 0;

    // Asserts parameters 
    if(net == NULL) return MESH_ERROR;
    if(uniqueId == NULL) return MESH_ERROR;
    if(_hSerial == NULL) return MESH_ERROR;

    // Loads dummy bytes 
    for(i=0; i<3; i++)
        bufferPayload[i] = 0x00;

    if(prepareFrame(0, CMD_LOCALREAD, &bufferPayload[0], 3)!=MESH_OK)
        return MESH_ERROR;

    // Sends packet
    sendPacket();

    // Flush serial input buffer
    serialFlush();

    /* Waits for response */
    if(receivePacket(id, &command, &bufferPayload[0], &payloadSize, 5000) != MESH_OK)
        return MESH_ERROR;

    // Checks response command 
    if(command != CMD_LOCALREAD)
        return MESH_ERROR;

    *net = (uint16_t)bufferPayload[0] | ((uint16_t)(bufferPayload[1]) << 8);
    *uniqueId =  (uint32_t)bufferPayload[2] | 
                ((uint32_t)(bufferPayload[3]) << 8) |
                ((uint32_t)(bufferPayload[4]) << 16) |
                ((uint32_t)(bufferPayload[5]) << 24);

    // update interanal variables
    _id = *id;
    _net = *net;
    _uniqueId = *uniqueId;
    
    return MESH_OK; 
}

mesh_status_t LoRaMESH::prepareFrame(uint16_t id, uint8_t command, uint8_t* payload, uint8_t payloadSize){
    // Asserts parameters
    if(payload == NULL) return MESH_ERROR;
    if(id > 1023) return MESH_ERROR;

    uint16_t crc = 0;

    _frame.size = payloadSize + 5;

    // Loads the target's ID 
    _frame.buffer[0] = id&0xFF;
    _frame.buffer[1] = (id>>8)&0x03;

    // Loads the command 
    _frame.buffer[2] = command;

    if((payloadSize >= 0) && (payloadSize < MAX_PAYLOAD_SIZE)){
        // Loads the payload on frame
        memcpy(&(_frame.buffer[3]), payload, payloadSize);

        // Calc CRC-16
        crc = calcCRC((&_frame.buffer[0]), payloadSize+3);
        _frame.buffer[payloadSize+3] = crc&0xFF;
        _frame.buffer[payloadSize+4] = (crc>>8)&0xFF;    
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

    if(_frame.command)
        _hSerial->write(_frame.buffer, _frame.size);
    
    else
        _hSerial->write(_frame.buffer, _frame.size);
    
    return MESH_OK;
}

mesh_status_t LoRaMESH::receivePacket(uint16_t* id, uint8_t* command, uint8_t* payload, uint8_t* payloadSize, uint32_t timeout){
    uint16_t waitNextByte = 500;
    uint8_t i = 0;
    uint16_t crc = 0;

    // Assert parameters 
    if(id == NULL) return MESH_ERROR;
    if(command == NULL) return MESH_ERROR;
    if(payload == NULL) return MESH_ERROR;
    if(payloadSize == NULL) return MESH_ERROR;
    if(_hSerial == NULL) return MESH_ERROR;

    if(!_hSerial->isListening()) _hSerial->listen();

    //Waits for reception
    while( ((timeout > 0 ) || (i > 0)) && (waitNextByte > 0) ){
        if(_hSerial->available() > 0){
            _frame.buffer[i++] = _hSerial->read();
            waitNextByte = 500;
        }
        if(i > 0) waitNextByte--;
        timeout--;
        delay(1);
    }

    // In case it didn't get any data
    if((timeout == 0) && (i == 0)) return MESH_ERROR;

    // Checks CRC16 
    crc = (uint16_t)_frame.buffer[i-2] | ((uint16_t)_frame.buffer[i-1] << 8);
    if(calcCRC(&_frame.buffer[0], i-2) != crc) return MESH_ERROR;

    // Copies ID, command, payloadSize, payload
    *id = (uint16_t)_frame.buffer[0] | ((uint16_t)_frame.buffer[1] << 8);
    *command = _frame.buffer[2];
    *payloadSize = i-5;
    memcpy(payload, &_frame.buffer[3], i-5);

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