# weather node
> Portable weather station node with SIM808, STM32F103 (AKA Bluepill), SDS011, PM5003, AM3201
## Bluepill
In this project we use the STM32F103C6T8 Microcontroller, because it is cheap and powerful enough to manage up to 4 Serial port devices, and has enough memory for string processing, even capable of running FreeRTOS.
In order to develop we use the following tools.

- (VS Code)[https://code.visualstudio.com/]
- (Arduino for VS Code)[https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino]
- (Arduino)[https://www.arduino.cc/en/Main/OldSoftwareReleases#previous] 1.8.6 For compatibility issues
- (stm32duino)[https://www.stm32duino.com/] Library to be able to use the bluepill with Arduino IDE
- Package (Libreary)[http://dan.drown.org/stm32duino/package_STM32duino_index.json]

![bluepill](/assets/bluepill.jpg)


## GPRS Modem
To send data to our server we use the SIM808 GPRS modem Aliexpress link [here](https://www.aliexpress.com/i/32435607442.html)

![SIM808 Module](/assets/sim808.jpg)

The data will be sent periodicly to our server using POST requests, for this purpose we can use the available documentation [SIM808 IP Appication Note](/assets/sim800_series_ip_application_note_v1.00.jpg).


### HTTP Request and communication
Comands used to send an http request from this modem

| Command           | Purpose                             |
| ------- | ------- |
|AT+CMEE=2          | Enable verbose error messages       |
| ------- | ------- |
|AT+SAPBR=1,1       | Enable GPRS context                 |
|AT+SAPBR=2,1       | Get info from GPRS context          |
|AT+SAPBR=0,1       | Disable GPRS context                |
| ------- | ------- |
|AT+HTTPINIT        | Start HTTP service                  | 
|AT+HTTPPARA = "CID",1                      | Set CID     |
|AT+HTTPPARA = "CONTENT","application/json" | Set json    |
|AT+HTTPPARA = "URL","example.com/endpoint" | Set URL     |
| ------- | ------- |
|AT+HTTPDATA=10,1000| Send 10 bytes, timeout 1000 ms      |
|AT+HTTPACTION=1    | Execute post method              |
|AT+HTTPREAD        | Show server response                |
|AT+HTTPTERM        | Finish HTTP service                 |

### GPS related commands
This is a brief from the necessary commands to turn on and get GPS data (location and time), for more information you can see this [Application Note](SIM808_GPS_Application_Note_V1.00.pdf)

| Command           | Purpose                       |
| ------- | ------- |
|AT+CGPSPWR=1           | Turn GPS on              |
|AT+CGPSPWR=0           | Turn GPS off             |
|AT+CGPSSTATUS?        | Returns the GPS status   |
|AT+CGPSINF=0           | Get Basic GPS Info       |

After powering the GPS service, it is mandatory to read the GPS status in order to validate the return data, the gps status should be:
```
+CGPSSTATUS: Location 3D Fix
```

Data shown after requesting a basic GPS info
```
+CGPSINF: 0,1332.179300,7157.209700,3288.200000,20210125043921.000,0,8,0.166680,177.949997
```
If we split the returned data between commas, we have the following 

| Position | Value             | Meaning |
| -------  | ----------------- | ------- |
|       1  | 0                 | mode              |
|       2  | 1332.179300       | longitude         |
|       3  | 7157.209700       | latitude          |
|       4  | 3288.200000       | altitude          |
|       5  | 20210125043921.000| UTC time          |
|       6  | 0                 | time to first fix |
|       7  | 8                 | satellites in view| 
|       8  | 0.166680          | speed over ground |
|       9  | 177.949997        | course            |

### GLONASS related commands
Taken fron the [SIM800 Series Glonnas Application Note]('/assets/SIM800 Series_GNSS_Application Note V1.00.pdf')

| Command       | Purpose                      |
| ------------- | ---------------------------- |
| AT+CGNSPWR=1  | Turn on GNSS Power supply    |
| AT+CGNSPWR=0  | Turn GPS off                 |
| AT+CGNSINF    | GNSS Natigation information  |

The returned data from AT+CGNSINF looks like this:
```
+CGNSINF: 1,1,20210125060840.000,-13.536150,-71.953617,3338.500,0.44,222.1,1,,1.4,1.7,1.0,,12,6,,,46,,
```
| Index | Parameter                  | Unit              | Range      | Length |
| ----- | ---------                  | ----              | -----      | ------ |
|    1  | GPS run status             | --                | 0-1        | 1  |
|    2  | Fix status                 | --                | 0-1        | 1  |
|    3  | UTC date & Time            | yyyyMMddhhmmss.ss | mixed      | 18 |
|    4  | Latitude                   | ±dd.dddddd        | [-90,90]   | 10 |
|    5  | Longitude                  | ±ddd.dddddd       | [-180,180] | 10 |
|    6  | MSL Altitude               | meters            | --         | 8 |
|    7  | Speed Over Ground          | Km/hour           | [0,999.99] | 6 |
|    8  | Course Over Ground         | degrees           | [0,360.00] | 6 |
|    9  | Fix Mode                   | --                | 0,1,2[1]   | 1 |
|    10 | Reserved1                  | 0                 |            | 0 |
|    11 | HDOP                       | --                | [0,99.9]   | 4 |
|    12 | PDOP                       | --                | [0,99.9]   | 4 |
|    13 | VDOP                       | --                | [0,99.9]   | 4 |
|    14 | Reserved2                  | 0                 |            | 0 |
|    15 | GPS Satellites in View     | --                | [0,99]     | 2 |
|    16 | GNSS Satellites Used       | --                | [0,99]     | 2 |
|    17 | GLONASS Satellites in View | --                | [0,99]     | 2 |
|    18 | Reserved3                  | 0                 |            | 0 |
|    19 | C/N0 max                   | dBHz              | [0,55]     | 2 |
|    20 | HPA[2]                     | meters            | [0,9999.9] | 6 |  
|    21 | VPA[2]                     | meters            | [0,9999.9] | 6 |


## PM1 - nova PM sensor (SDS011)
Particle sensor, his datasheet can be found [here](/assets/pm1_ds.pdf)
Sample serial output at 9600bps, checkbit: none, stopbits: 1
```
aa c0 06 00 07 00 26 06 39 ab
```
Data frame fields:
| Position | Default | Description   |
| -------- | ------- | -----------   |
| 1        | 0xaa    | Frame start   |
| 2        | 0xc0    | Command       |
| 3        | Data1   | PM2.5 LowByte |
| 4        | Data2   | PM2.5 HighByte|
| 5        | Data3   | PM10 LowByte  |
| 6        | Data4   | PM10 HighByte |
| 7        | Data5   | ID byte 1 |
| 8        | Data6   | ID byte 2 |
| 9        | ---     | CheckSum (Data1+Data2+...) |
| 10       | 0xab    | Frame end |

As we are using PM2.5 we will read Data1 and Data2, then, the concentration should be:
```
  PM = HighByte*256 + LowByte
```
## PM2 - PM2.5 Sensor (PM5003)
Particle sensor, his datasheet can be found [here]('/assets/pm2.pdf')
Sample serial output at 9600bps, checkbit: none, stopbits: 1
```
42 4d 00 1c 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 97 00 01 42
```

Data frame fields:
| Position | Default | Description |
| -------- | ------- | ----------- |
| 1        | 0x42    | Start Char |
| 2        | 0x4d    | Start Char |
| 3,4      | 0x00 0x1c | Frame len 2*13+2 (data+crc)|
| 5,6      |   --    | Data1 PM1.0 (ug/cm3) (CF=1, standard particle)|
| 7,8      |   --    | Data2 PM2.5 (ug/cm3) (CF=1, standard particle)|
| 9,10     |   --    | Data3 PM10 (ug/cm3) (CF=1, standard particle)|
| 11,12    |   --    | Data4 PM1.0 (ug/cm3) (under atmosferic environment)|
| 13,14    |   --    | Data5 PM2.5 (ug/cm3) (under atmosferic environment)|
| 15,16    |   --    | Data6 (ug/cm3) (under atmosferic environment)|
| 17,18    |   --    | Data7 Number of particles with diameter beyond 0.3 um in 0.1 L of air|
| 19,20    |   --    | Data8 Number of particles with diameter beyond 0.5 um in 0.1 L of air|
| 21,22    |   --    | Data9 Number of particles with diameter beyond 1.0 um in 0.1 L of air|
| 23,24    |   --    | Data10 Number of particles with diameter beyond 2.5 um in 0.1 L of air|
| 25,26    |   --    | Data11 Number of particles with diameter beyond 5.0 um in 0.1 L of air|
| 27,28    |   --    | Data12 Number of particles with diameter beyond 10.0 um in 0.1 L of air|
| 29,30    |   --    | Data13 reserved |
| 31,32    |   --    | CRC High & low |

As we are focus on the standard particle (CF=1) we will read bytes 7 and 8 
## AM3201 - Temperature, humidity Sensor
This is a common sensor, it uses the One-wire protocol, its datasheet is attached [here](/assets/am2301.pdf)

![AM2301](/assets/am2301.jpeg)
