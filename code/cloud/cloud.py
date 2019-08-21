from google.cloud import datastore
import os
from datetime import datetime, timezone, timedelta
import csv

# set the env variable through the script
os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "Lock-Management-System.json"
# os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = r'''C:\Users\Sam\Documents\UCSC\2018_Winter\CMPE123A\private_key.json'''

client = datastore.Client()

##############################################################################
##############################################################################
'''Room and User Entities'''
##############################################################################
##############################################################################
def AddNewRoom(index, room_number,users_list = [], class_list = []):
	# AddNewRoom() will add to the "Room" entity
	# first argument is the index in database
	#
	# second argument should be the room number
	# that has a lock on it i.e. BE301, BE340A, E2-599
	#
	# third argument is array of Group that
	# have acces to the room i.e. CMPE123A, CMPE121, CMPE167L
	
	entity = 'Rooms'
	ID = 'index{}'.format(room_number)
	new_key = client.key(entity, ID)

	# Prepares the new entity
	new_room_entity = datastore.Entity(key=new_key)
	new_room_entity.update({
    'Group': class_list ,
    'Room_Number': room_number,
    'Users': users_list,
    'Capacity': 0,
    'Last_Login_time': None
    })
	# new_room_entity = datastore.Entity(key=new_key)
	# new_room_entity['Classes'] = class_list
	# new_room_entity['Room_Number'] = room_number
	# new_room_entity['Users'] = Users
	# new_room_entity["Capacity"] = 0;

	client.put(new_room_entity)

def AddNewUser(index, name, cruzID, priority, class_list = []):
	# AddNewUser() will add to the "Student" entity
	# first argument is the index in database
	#
	# second arugment should be the first and last name
	# of the student i.e. "Samuel Wu"
	#
	# third argument is JUST the cruzID of the
	# student (without @ucsc.edu) i.e. sazwu
	#
	# fourth argument is the priority
	#
	# fifth argument is the list of classes the
	# student is in i.e. CMPE167, AMS147, CMPE110
	
	entity = 'User'
	ID = 'index{}'.format(cruzID)
	new_key = client.key(entity, ID)

	# Prepares the new entity
	new_user_entity = datastore.Entity(key=new_key)
	new_user_entity['Group'] = class_list
	new_user_entity['Name'] = name
	new_user_entity['cruzID'] = cruzID
	new_user_entity['Priority'] = priority
	new_user_entity['Last_Room'] = None

	# saves the entity 
	client.put(new_user_entity)
	
def ModifyRoomEntity(lab_index, room_number, class_list = []):
	# ModifyRoomEntity() modifies an existing "Room" entity
	# first argument is the index in database
	#
	# second argument should be the room number
	# that has a lock on it i.e. BE301, BE340A, E2-599
	#
	# third argument is array of Group that
	# have acces to the room i.e. CMPE123A, CMPE121, CMPE167L

	# Prepares the new entity
	ID = 'index{}'.format(lab_index)
	key = client.key('Rooms', ID)
	update_lab_entity = client.get(key)
	
	update_room_entity['Group'] = []
	update_room_entity['Group'] = class_list
	update_room_entity['Room_Number'] = room_number
	
	# saves the entity 
	client.put(update_room_entity)
	
def ModifyUserEntity(user_index, name, cruzID, priority, class_list = []):
	# ModifyUserEntity() modifies an existing "Lab" entity
	# first argument is the index in database
	#
	# second arugment should be the first and last name
	# of the student i.e. "Samuel Wu"
	#
	# third argument is JUST the cruzID of the
	# student (without @ucsc.edu) i.e. sazwu
	#
	# fourth argument is the priority
	#
	# fourth argument is the list of Group the
	# student is in i.e. CMPE167, AMS147, CMPE110
	
	# Prepares the new entity
	ID = 'index{}'.format(user_index)
	key = client.key('User', ID)
	update_user_entity = client.get(key)
	
	update_user_entity['Group'] = []
	update_user_entity['Group'] = class_list
	update_user_entity['Name'] = name
	update_user_entity['cruzID'] = cruzID
	update_user_entity['Priority'] = priority
	
	# saves the entity 
	client.put(update_user_entity)
	
def QueryAllRoomsEntities():
	# prints all room entities
	# taken from gcloud_demo.py
	query = client.query(kind='Rooms')
	print('\nRunning Query of All Lab Entities\n*******************************************')
	for log_ in query.fetch():
		print('Room {:<6} is accessible by class {}'.format(log_['Room_Number'], ', '.join(log_['Group'])))

def QueryAllUserEntities():
	# prints all user entities
	# taken from gcloud_demo.py
	query = client.query(kind='User')
	print('\nRunning Query of All User Entities\n***************************************')
	for log_ in query.fetch():
		print('Name:{:<20} cruzID:{:<10} Priority:{} Group:{}'.format(log_['Name'], log_['cruzID'], log_['Priority'],', '.join(log_['Group'])))

	
def AddClassToRoomEntity(lab_index):
	# AddClassToRoomEntity() deletes an existing "Lab" entity
	# first argument is the index in database to delete

	# Prepares the new entity
	ID = 'index{}'.format(lab_index)
	key = client.key('Rooms', ID)
	client.delete(key)
	
def DeleteRoomEntity(room_number):
	# def DeleteRoomEntity() deletes an existing "Room" entity
	# first argument is the index in database to delete

	# Prepares the new entity
	ID = 'index{}'.format(room_number)
	key = client.key('Rooms', ID)
	print("Deleting Room:" + room_number)
	client.delete(key)

def DeleteUserEntity(cruzID):
	# def DeleteUserEntity() deletes an existing "User" entity
	# first argument is the index in database to delete
	
	# Prepares the new entity
	ID = 'index{}'.format(cruzID.upper())
	key = client.key('User',ID)
	print("Deleting " + cruzID)
	client.delete(key)	

def DeleteAllUserEntities():
	# Deletes all of Users in the datastore
	print('\n*******************Deleting all User Entities*******************\n')
	query = client.query(kind='User')
	users = list(query.fetch())
	for user in users:
		DeleteUserEntity(user['cruzID'])		
	print("\n*******************Done Deleting*******************\n")

def DeleteAllRoomEntities():
	# Deletes all of Rooms in the datastore
	print('\n*******************Deleting all Room Entities*******************\n')
	query = client.query(kind='Rooms')
	rooms = list(query.fetch())
	for room in rooms:
		DeleteRoomEntity(room['Room_Number'])		
	print("\n*******************Done Deleting*******************\n")


##############################################################################
##############################################################################
'''ROOM LOGS'''
##############################################################################
##############################################################################	
def LoginRoom(room_number, cruzID):
	#LoginRoom() is called when user enters room
	#
	#first argument is room he or she enters
	#second argument is the cruzID of the entering user

	#checks if there is already an exact entity
	#case is if user enter and exits room multiple time
	#################################################
	index = 0
	ID = 'index_{}{}'.format(cruzID, index)
	new_key = client.key(room_number, ID)
	check_ = client.get(new_key)
	if check_ != None:
		while check_['Exit_Time'] != None:
			index += 1
			ID = 'index_{}{}'.format(cruzID, index)
			new_key = client.key(room_number, ID)
			check_ = client.get(new_key)
			if check_ == None:
				break
	##################################################
	
	
	# Prepares the new entity
	new_room_entity = datastore.Entity(key = new_key)
	new_room_entity['cruzID'] = cruzID
	new_room_entity['Enter_Time'] = datetime.now(timezone.utc).replace(microsecond=0)
	new_room_entity['Exit_Time'] = None
	new_room_entity['24hr_Cumulative_Time'] = None
	client.put(new_room_entity)
	
def LogoutRoom(room_number, cruzID):
	#LogoutRoom() is called when user leaves room
	#
	#first argument is room he or she leaves
	#second argument is the cruzID of the leaving user


	#checks if there is already an exact entity
	#case is if user enter and exits room multiple time
	#################################################
	index = 0
	ID = 'index_{}{}'.format(cruzID, index)
	new_key = client.key(room_number, ID)
	check_ = client.get(new_key)
	while check_['Exit_Time'] != None:
		index += 1
		ID = 'index_{}{}'.format(cruzID,index)
		new_key = client.key(room_number, ID)
		check_ = client.get(new_key)
	##################################################
	
	
	
	# Prepares the new entity
	new_room_entity = datastore.Entity(key = new_key)
	# values to be inputted
	arrival_ = client.get(new_key)
	leave_time = datetime.now(timezone.utc).replace(microsecond=0)
	total_time = leave_time - arrival_['Enter_Time']
	# new entity values
	new_room_entity['Enter_Time'] = arrival_['Enter_Time']
	new_room_entity['Exit_Time'] = leave_time
	new_room_entity['cruzID'] = cruzID
	
	# computes cumulative time spent in lab
	if index > 0:
		ID = 'index_{}{}'.format(cruzID, index-1)
		new_key = client.key(room_number, ID)
		previous_ = client.get(new_key)
		total_time_string = previous_['24hr_Cumulative_Time']
		# check for ValueError, not all format is like so
		# some format don't have day
		# may need to check for week eventually
		try:
			t = datetime.strptime(total_time_string,"%d day, %H:%M:%S")
			td2 = timedelta(days=t.day, hours=t.hour, minutes=t.minute, seconds=t.second)
		except ValueError:
			t = datetime.strptime(total_time_string,"%H:%M:%S")
			td2 = timedelta(hours=t.hour, minutes=t.minute, seconds=t.second)
		total_time += td2
		
	new_room_entity['24hr_Cumulative_Time'] = str(total_time)	
	# stores in cloud
	client.put(new_room_entity)

def ClearRoomLog(room_number):
	# ClearRoomLog() clears all entries in the entity "room_number"
	# can be used at end of day (or whenever user wants) to clear
	# the log of when the room was accessed
	cruzID_list = []
	query = client.query(kind = room_number)
	for log_ in query.fetch():
		cruzID_list.append(log_['cruzID'])	
	unique_cruzID_list = set(cruzID_list)
	
	for j in range(0, len(unique_cruzID_list)):
		cruzID = unique_cruzID_list.pop()	#can only pop from set
		for i in range(0, len(cruzID_list)):
			ID = 'index_{}{}'.format(cruzID, i)
			key = client.key(room_number, ID)
			client.delete(key)

def CountRoomPopulation(room_number):
	# CountRoomPopulation() counts how many people are in the room
	# based off of how many people have a "null" (None) exit time
	query = client.query(kind = room_number)
	counter = 0
	for log_ in query.fetch():
		if log_['Exit_Time'] == None:
			counter += 1
	return counter

def HistogramEnterTime(room_number):
	# this function makes a histogram of 24 hour long intervals
	# of when the room is most often entered
	# it returns a list of size 24
	hist_24_interval = [0] * 24
	query = client.query(kind = room_number)
	for log_ in query.fetch():
		if log_['Enter_Time'] != None:
			index = log_['Enter_Time'].hour
			hist_24_interval[index] += 1
	return hist_24_interval

def HistogramExitTime(room_number):
	# this function makes a histogram of 24 hour long intervals
	# of when the room is most often exited
	# it returns a list of size 24
	hist_24_interval = [0] * 24
	query = client.query(kind = room_number)
	for log_ in query.fetch():
		if log_['Exit_Time'] != None:
			index = log_['Exit_Time'].hour
			hist_24_interval[index] += 1
	return hist_24_interval
			
'''
def AverageTimeInRoom(room_number):
	# this function calculates the average time spent in room
	# between login and logouts
'''
	
	
	
##############################################################################
##############################################################################
'''CSV FILE TO CLOUD'''
##############################################################################
##############################################################################
def AddClassToRoom(room_number, Group = [], users = []):
	# DICTIONARY DATA STRUCTURE
	# adds a list of Group & users to a room
	# used when a list of Group can use a room/lab
	room_dict[room_number] = Group
	users_for_room_dict[room_number] = users

def DeleteRoom(room_number):
	'''
	This function actually might not be needed
	'''
	# deletes room from dictionary and associated Group
	del room_dict[room_number]


def ParseCSVRoomEntity(file):
	#first argument is the .csv to be read
	#.csv file must have first column as the room number
	#.csv file must have the list of Group that can 
	#access the room in the second column separated by
	#spaces
	with open(file, newline='') as file:
		reader = csv.reader(file)
		# skips the first row in the .csv file
		iter_row = iter(reader)
		next(iter_row)
		for row in iter_row:
			# this creates a list of the Group that are separated by a space
			# print (row[0])
			stringDel = row[1].split(" ")
			userDel = row[2].split(" ")
			AddClassToRoom(row[0], stringDel,userDel)

def CSVToCloudRoom(file):
	# argument takes in .csv file to be parsed
	# directly adds from .csv file to the cloud
	ParseCSVRoomEntity(file)	#parses file and stores in dictionary
	i = 0
	for key in room_dict.keys():
		AddNewRoom(i, key,users_for_room_dict[key], room_dict[key])
		i +=1
	

######
def AddClassToUser(user, info_list = []):
	# DICTIONARY DATA STRUCTURE
	# adds a list of Group to a room
	# used when a list of Group can use a room/lab
	user_dict[user] = info_list	
	
	
def ParseCSVUserEntity(file):
	#first argument is the .csv to be read
	#.csv file must have first column as the room number
	#.csv file must have the list of Group that can 
	#access the room in the second column separated by
	#spaces
	with open(file, newline='') as file:
		reader = csv.reader(file)
		# skips the first row in the .csv file
		iter_row = iter(reader)
		next(iter_row)
		for row in iter_row:
			user_list = []
			Group = row[3]
			stringDel = Group.split(" ")
			# creates a list that is then stored into the dictionary
			user_list.append(row[1])
			user_list.append(row[2])
			user_list.append(stringDel)
			AddClassToUser(row[0], user_list)


def CSVToCloudUser(file):
	# argument takes in .csv file to be parsed
	# directly adds from .csv file to the cloud
	ParseCSVUserEntity(file)	#parses file and stores in dictionary
	i = 0
	for key in user_dict.keys():
		AddNewUser(i, key, user_dict[key][0], user_dict[key][1], user_dict[key][2])
		i +=1	
	
#testing
##########################
# class_list0 = ["CMPE123A", "CMPE129B"]
# class_list1 = ["AMS147", "CMPS101"]
# room_list = ["BE340"]
# room_number0 = "BE340"
# room_number1 = "E2-399"
# name0 = "Samuel Wu"
# name1 = "Bowen Brooks"
# name3 = "John Smith"
# cruzID0 = "sazwu"
# cruzID1 = "bojbrook"
# cruzID2 = "hello"
# cruzID3 = "jsmith"
admin_priority = 0
faculty_priority = 1
grad_priority = 2
undergrad_priority = 3

# dictionaries for room entities
# interesting data structur in Python
room_dict = {}
users_for_room_dict = {}
user_dict = {}

# QueryAllUserEntities()
DeleteAllUserEntities()
DeleteAllRoomEntities()

# CSVToCloudRoom("rooms.csv")
# CSVToCloudUser("users.csv")





# print(HistogramEnterTime(room_number0))
# print(HistogramExitTime(room_number0))


