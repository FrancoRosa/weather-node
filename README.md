# weather node
> Portable weather station node with SIM808
## Arduino
In this project we use the classic arduino uno.

![arduino uno](/assets/uno.jpg)
## GPRS Modem
To send data to our server we use the SIM808 GPRS modem [Duinopeak SIM808 GSM/GPRS/GPS/Bluetooth Shield for Arduino](https://www.gudreviews.com/boards-shields/612970688/duinopeak-sim-gsm-gprs-gps.html)

![SIM808 Shield](/assets/sim808.jpg)

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

## Steps followed to build this project.
 - Set up enviroment make the arduino blink
 - Echo the terminal to the simcom808 input and test the AT command