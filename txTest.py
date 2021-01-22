from time import sleep
import serial

ser = serial.Serial('COM3')
print('>>>> start')
ser.write(b'AT\r')
while True:
  data = ser.readline()
  print(data)
  sleep(0.1)