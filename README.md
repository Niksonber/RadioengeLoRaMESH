# RadioengeLoRaMESH

Radioenge LoRaMESH Module handler for arduino Framework.

This Library provides simple interface to configure module (ID, NET, CLASS, BW, SF, CR...),  get info (RSSI, Trace route...), send and receive packges.

### Suported devices

ESP32 or Atmega (Arduino Uno, Nano...)

### Usage

#### Instantiation:

```cpp
 #include<LoRaMESH.h    
 
 LoRaMESH lora;
```

#### Initialization:

```cpp
if ( lora.begin(RX_PIN, TX_PIN != LoRaMESHNS::MESH_OK ){
    // initialization Fail
}
```

#### Set ID and NET

```cpp
uint16_t id, net;
uint32_t uniqueID;
lora.localRead(&id, &net, &uniqueId);

// set NET
if( lora.storeNet(NET) != LoRaMESHNS::MESH_OK ){
    // Fail   
}

if( lora.storeID(ID, NET, uniqueId) != LoRaMESHNS::MESH_OK ){
    // Fail
}
lora.localRead(&id, &net, &uniqueId);
```

#### Configuration

```cpp
 if( lora.configLoRa(ID, POWER, LoRaMESHNS::BW125KHZ, SF, LoRaMESHNS::CR45) != LoRaMESHNS::MESH_OK ){
     // COnfiguration fail
 }
```

Parameters:

1.  id - id of device

2. power

3. bw - band witdh e.g. BW125KHZ, BW250KHZ

4. sf - spreading factor [7-12] 

5. cr - coding rate, eg CR45 for 4/5



Alternativaly, using default values

```cpp
if( lora.configLoRa(ID) != LoRaMESHNS::MESH_OK ){
    // COnfiguration fail
}
```

#### Set Class

1. Class A - Low power mode (it doesn't receive mesage)

2. Class C - Normal mode (receive mesage)

to more info read the user manual

```cpp
if( lora.setLowPowerMode(ID, LoRaMESHNS::CLASS_A, LoRaMESHNS::WINDOW_10s) != LoRaMESHNS::MESH_OK ){
    // COnfiguration fail
}
```

Parameters:

1. id - Device's ID 

2. mode - Class mode of operation, e.g. classA or classC

3. window only applicable in class A, time interval before change class



Alternativaly, using default values

```cpp
if( lora.setLowPowerMode(ID) != LoRaMESHNS::MESH_OK ){
    // COnfiguration fail
}
```

### Receive

```cpp
uint16_t id;
uint8_t command, size, data[SIZE];
if( lora.receivePacket(&id, &command, data, &size, WAIT_TIME) == LoRaMESHNS::MESH_OK ){
     // received message
 }
```

#### Send

```cpp
uint16_t id;
uint8_t command, data[SIZE];
lora.prepareFrame(id, command, data, SIZE);
lora.sendPacket();
```

Once that frame is ready, to send same mesage just call 

```cpp
lora.prepareFrame(id, command, data, SIZE);
for (uint8_t i = 0; i < 3; i++){
    lora.sendPacket();
    delay(10000);   
}
```



#### Get router

Only gateway, that is, node with id == 0, can request router, RSSI or broadcast

```cpp
uint8_t id, command, size, maxAttempts = 10;
uint16_t data[MAX_SIZE/2];
if ( lora.getInfo(destineID, LoRaMESHNS::CMD_TRACEROUTE, (uint8_t *)data, &size) == LoRaMESHNS::MESH_OK ){
    while( maxAttempts--){
        if (lora.receivePacket(&id, &command, (uint8_t *)data, &size, WAIT_TIME) == LoRaMESHNS::MESH_OK ){
            // print router
            for(uint8_t i = 0; i < size/2; i++){
                Serial.println(data[i]);
            } 
            break;
        }
    }
}

```