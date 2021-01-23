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

 
## Steps followed to build this project.
 - Set up enviroment make the arduino blink
 - Echo the terminal to the simcom808 input and test the AT command