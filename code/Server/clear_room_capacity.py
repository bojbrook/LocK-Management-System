import time
import os
from  google.cloud import datastore
from datetime import datetime, timezone, timedelta

os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "Lock-Management-System.json"
client = datastore.Client()


def query_all(entity):
	query = client.query(kind=entity)
	return list(query.fetch())


def reset_room_capacity(room_number):
		"""Summary
		change_room_capacity() changes the capacity of the room to 0 in the datastore 
		Args:
				room_number (str): A string containing the room_number
				entering (boolean): A boolean on if the user is leaving or entering
		"""
		entity = 'Rooms'
		ID = 'index{}'.format(room_number)
		key = client.key(entity,ID)
		room_entity = client.get(key)
		room_entity['Capacity'] = 0
		room_entity['Last_Login_time'] = None
		client.put(room_entity)


current_time = datetime.now(timezone.utc).replace(microsecond=0)
Rooms = query_all("Rooms")
print ("Current " + str(current_time.time()))
for room in Rooms:
	last_login = (room['Last_Login_time'])
	if(last_login is not None):
		print(last_login.minute)
		print(current_time.minute)
		login_minute = last_login.minute
		current_minute = current_time.minute
		if(current_minute > login_minute + 5):
			print("Reseting Capacity in Room: " + room['Room_Number'])
			reset_room_capacity(room['Room_Number'])
		#checking for the bounds on the time
		elif((60 - abs(current_time.minute -  last_login.minute)) > 5):
			print("Reseting Capacity in Room: " + room['Room_Number'])
			reset_room_capacity(room['Room_Number'])
		else:
			print(last_login)
