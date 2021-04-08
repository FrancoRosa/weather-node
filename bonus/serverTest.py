# Frame test with higher level programming in order to test if the server works
#  
from time import sleep
from random import random as rd
from pprint import pprint
from termcolor import colored
import requests
url = 'http://us-central1-weather-node-ui.cloudfunctions.net/measurements'

while True:
  measurements = {
    "temperature": "%2.1f"%(27.1 + rd()),
    "humidity": "%2.0f"%(66.0 + 10*rd()),
    "latitude": "%2.5f"%(-13.124 + 0.2 * rd()),
    "longitude": "%2.5f"%(-72.4321 + 0.2 * rd()),
    "PM1": "%2.0f"%(4 + 10*rd()),
    "PM2": "%2.0f"%(31 + 10*rd()),
    "timestamp": 0
  }

  response = requests.post(url, json=measurements)
  print(colored(">>> tx:",'red'))
  pprint(measurements)
  print(colored(">>> rx:",'green'))
  pprint(response.json())
  print(colored("_____________________",'grey'))

  sleep(5)
