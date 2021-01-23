# Update sensor data on server with python

import requests
url = 'http://sensor-network-lora.herokuapp.com/api/sensors'

sensors = {
  "temperature": 18,
  "humidity": 19,
  "latitude": 21,
  "longitude": 22,
  "PM1": 23,
  "PM2": 24,
  "timestamp": 26
}

# Update sensors
data = { 
        'sensor': {
          'id': [
            sensors["temperature"],
            sensors["humidity"],
            sensors["latitude"],
            sensors["longitude"],
            sensors["PM1"],
            sensors["PM2"],
            sensors["timestamp"]
          ], 
          
          'value': [
            1,
            2,
            3,
            4,
            5,
            6,
            7
          ]
        }
      } #many sensors

response = requests.post(url, json=data)
print(">> url:", url)
print(">>> tx:", data)
print(">>> rx:", response.json())