# weather node
> Portable weather station node with SIM808
## Arduino
In this project we use the classic arduino uno.

![arduino uno](/assets/uno.jpg)
## GPRS Modem
To send data to our server we use the SIM808 GPRS modem [Duinopeak SIM808 GSM/GPRS/GPS/Bluetooth Shield for Arduino](https://www.gudreviews.com/boards-shields/612970688/duinopeak-sim-gsm-gprs-gps.html)

![SIM808 Shield](/assets/sim808.jpg)

The data will be sent periodicly to our server using POST requests, for this purpose we can use the available documentation [SIM808 IP Appication Note](/assets/sim800_series_ip_application_note_v1.00.jpg).

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