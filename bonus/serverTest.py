# Frame test with hier level programming in order to test if the server works
#  
import requests
url = 'http://sensor-network-lora.herokuapp.com/api/sensors'
url = 'http://us-central1-weather-node-ui.cloudfunctions.net/measurements'

measurements = {
  "temperature": 27.1,
  "humidity": 66.0,
  "latitude": -13.124,
  "longitude": -72.4321,
  "PM1": 4,
  "PM2": 31,
  "timestamp": 0
}

response = requests.post(url, json=measurements)
print(">> url:", url)
print(">>> tx:", measurements)
print(">>> rx:", response.json())