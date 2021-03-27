#include "LoRaMESH.h"

 LoRaMESH lora;

void printNET(uint16_t id, uint16_t net, uint32_t uniqueId){
    Serial.println("Id: " + String(id) +" NET: "+ String(net) + " Unique ID: " + String(uniqueId));
}

void setup(){
    Serial.begin(9600);
    lora.begin(3,4);
   
    delay(10000);
    Serial.println("passou");

    uint16_t id, net;
    uint32_t uniqueId;

    lora.localRead(&id, &net, &uniqueId);
    printNET(id, net, uniqueId);

    if(lora.storeNet(121)!=LoRaMESHNS::MESH_OK)
        Serial.println("error");

    if(lora.storeID(49, 121, uniqueId)!=LoRaMESHNS::MESH_OK)
        Serial.println("error");
    lora.localRead(&id, &net, &uniqueId);
    printNET(id, net, uniqueId);
    
    if(lora.configLoRa(49)!=LoRaMESHNS::MESH_OK){
        Serial.println("error");
    }

    if(lora.setLowPowerMode(49)!=LoRaMESHNS::MESH_OK)
        Serial.println("error");
}

void loop(){
    Serial.print(".");
    delay(1000);
}