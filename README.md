# weather node
> Portable weather station node with SIM808
## Arduino
In this project we use the classic arduino uno.

![arduino uno](/assets/uno.jpg)
## GPRS Modem
To send data to our server we use the SIM808 GPRS modem [Duinopeak SIM808 GSM/GPRS/GPS/Bluetooth Shield for Arduino](https://www.gudreviews.com/boards-shields/612970688/duinopeak-sim-gsm-gprs-gps.html)

![SIM808 Shield](/assets/sim808.jpg)

The data will be sent periodicly to our server using POST requests, for this purpose we can use the available documentation [SIM808 IP Appication Note](/assets/sim800_series_ip_application_note_v1.00.jpg).

 
## Steps followed to build this project.
 - Set up enviroment make the arduino blink
 - Echo the terminal to the simcom808 input and test the AT command