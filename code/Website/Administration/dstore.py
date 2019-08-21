from flask import current_app
from google.cloud import datastore
import csv

room_dict = {}
users_for_room_dict = {}
user_dict = {}

def init_app(app):
	pass

def get_client():
    """Summary
    gets the client for the data store
    Returns:
        datastore: client for the datastore
    """
    return datastore.Client(current_app.config['PROJECT_ID'])

################GETTING AND DELETING ENTITIES#####################
def query_all(entity):
	"""Summary
	Queries all the entities of a given kind
	Args:
	    entity (str): the kind of entitiy
	
	Returns:
	    list: a list of all of the entities in the kind
	"""
	ds = get_client()
	query = ds.query(kind=entity)
	return list(query.fetch())

def get_entity(kind,name):
	"""Summary
	gets a specific entitiy from the database
	Args:
	    kind (str): The kind of entity
	    name (str): The name of the enitity
	
	Returns:
	    entity: The enitity from the datastore
	"""
	ds = get_client()
	ID = 'index{}'.format(name)
	key = ds.key(kind,ID)
	return ds.get(key)


def delete_entity(kind,name):
	"""Summary
	Deletes an entity from the database
	Args:
	    kind (str): The kind of entity
	    name (str): the name of the enitity
	"""
	ds = get_client()
	ID = 'index{}'.format(name)
	key = ds.key(kind,ID)
	ds.delete(key)
	pass

##################################################################


def get_room_logs(room_number,cruzID = None):
	"""Summary
	Finds all the room logs for the room or just the user in the room
	Args:
	    room_number (str): The room_number for room
	    cruzID (None, str): cruzID for the User
	
	Returns:
	    list: a list of the room logs for the room or specific user
	"""
	ds = get_client()
	query = ds.query(kind=room_number)
	# query.order = ['Name']
	if(cruzID is not None):
		query.add_filter('cruzID', '=', cruzID.upper())
	return list(query.fetch())

###################UPDATING ROOM ENTITIES#########################
def update_room_group(room_number,group):
	"""Summary
	Updates the group for the specific room
	Args:
	    room_number (str): The room_number for room 
	    group (TYPE): The name of the group
	"""
	ds = get_client()
	room = get_entity("Rooms",room_number)
	Groups = list(room['Group'])
	Groups.append(group.upper())

	room['Group'] = Groups
	ds.put(room)

def update_room_users(room_number,cruzID):
	"""Summary
	Updates the users for the specific room
	Args:
	    room_number (str): The room_number for room 
	    cruzID (str): cruzID for the user to add to the room
	"""
	ds = get_client()
	room = get_entity("Rooms",room_number)
	users = list(room['Users'])
	users.append(cruzID.upper())

	room['Users'] = users
	ds.put(room)

def remove_group(room_number,group):
	"""Summary
	Removes a group access to a given room 
	Args:
	    room_number (str): The room_number for room 
	    group (str): group name 
	"""
	ds = get_client()
	room = get_entity("Rooms",room_number)
	groups = list(room['Group'])
	if group in groups:
		groups.remove(group)
	room['Group'] = groups
	ds.put(room)

def remove_user(room_number,user):
	"""Summary
	Removes a user access to a given room
	Args:
	    room_number (str): The room_number for room 
	    user (str): The cruzID for the given user
	"""
	ds = get_client()
	room = get_entity("Rooms",room_number)
	users = list(room['Users'])
	if user in users:
		users.remove(user)
	room['Users'] = users
	ds.put(room)
	
##################################################################

############METHODS FOR COMPARING USER GROUPS##############

def get_user_group(cruzID):
    """Summary
    get_user_Group() gets all of the Group that the given cruzID has access to.
    
    Args:
        cruzID (str): A string containing the users cruzID
    
    Returns:
        list: A list of all of the Group for that user
    """
    ds =get_client()
    Users = ds.query(kind='User')
    Users.projection = ['Group']
    Users.add_filter ('cruzID','=',cruzID)
    return list(Users.fetch()) 



def get_room_group(room_number):
  """Summary
      get_room_Group() gets all of the Group that have access to the room
  
  Args:
      room_number (str): A string containing the room number
  
  Returns:
      list: A list of all of the Group for that room
  """
  ds = get_client()
  room = get_entity("Rooms",room_number)
  return list(room['Group'])


def get_room_users(room_number):
	"""Summary
	Gets a list of all the users allowed into a room
	Args:
	    room_number (str): The room Number for a room
	
	Returns:
	    list: A list of users allowed into the room
	"""
	ds = get_client()
	room = get_entity("Rooms",room_number)
	return list(room['Users'])



def get_users_rooms(cruzID):
	"""Summary
	Gets the rooms available to the user. Based on the Groups they are in
	and the user groups they are in
	Args:
	    cruzID (str): The cruzID for the user
	
	Returns:
	    list: A list of all the rooms allowed for the user.
	"""
	rooms = []
	cruzID = cruzID.upper()
	group_list = get_user_group(cruzID)
	room_list = query_all("Rooms")


	for room in room_list:
		room_number = room['Room_Number']

		room_group = get_room_group(room_number)
		room_users = get_room_users(room_number)
		for group in group_list:
			if(group['Group'] in room_group):
				rooms.append(room_number)
				break
		# checking if user access to room
		if(cruzID in room_users and room_number not in rooms):
			rooms.append(room_number)
		
	return rooms


###########METHODS FOR EXPORTING ROOMS LOGS################
def roomlog_to_csv(Room_Number):
	"""Summary
	Exports the roomlog to a csvfile. The name of the csv file is the 
	same name as the room number
	Args:
	    Room_Number (TYPE): Description
	"""
	room_logs = query_all(Room_Number)
	filename = Room_Number + ".csv"
	with open(filename,"w",newline='') as csvfile:
		writer = csv.writer(csvfile, delimiter=' ')
		writer.writerow("cruzID,Enter-Time,Exit-Time")
		for log in room_logs:
			print (log)
			print ("\n")
			row_string = log['cruzID'] + "," + str(log["Enter_Time"]) + "," + str(log["Exit_Time"])
			# print (row_string)
			writer.writerow(row_string)			
	pass

##############################################################################


###########METHODS FOR UPLOADING NEW ROOMS################


def AddClassToRoom(room_number, Group = [], users = []):
	# DICTIONARY DATA STRUCTURE
	# adds a list of Group & users to a room
	# used when a list of Group can use a room/lab
	room_dict[room_number] = Group
	users_for_room_dict[room_number] = users

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


def AddNewRoom(index, room_number,users_list = [], class_list = []):
	# AddNewRoom() will add to the "Room" entity
	# first argument is the index in database
	#
	# second argument should be the room number
	# that has a lock on it i.e. BE301, BE340A, E2-599
	#
	# third argument is array of Group that
	# have acces to the room i.e. CMPE123A, CMPE121, CMPE167L
	ds = get_client()
	entity = 'Rooms'
	ID = 'index{}'.format(room_number)
	new_key = ds.key(entity, ID)

	# Prepares the new entity
	new_room_entity = datastore.Entity(key=new_key)
	new_room_entity.update({
    'Group': class_list ,
    'Room_Number': room_number,
    'Users': users_list,
    'Capacity': 0,
    'Last_Login_time': None
    })
	ds.put(new_room_entity)

def CSVToCloudRoom(file):
	# argument takes in .csv file to be parsed
	# directly adds from .csv file to the cloud
	room_dict.clear() 
	users_for_room_dict.clear()
	ParseCSVRoomEntity(file)	#parses file and stores in dictionary
	i = 0
	for key in room_dict.keys():
		AddNewRoom(i, key,users_for_room_dict[key],room_dict[key])
		i +=1

##############################################################################


###########METHODS FOR UPLOADING NEW ROOMS################
def AddClassToUser(user, info_list = []):
	# DICTIONARY DATA STRUCTURE
	# adds a list of Group to a room
	# used when a list of Group can use a room/lab
	user_dict[user] = info_list	


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
	ds = get_client()
	entity = 'User'
	ID = 'index{}'.format(cruzID.upper())
	new_key = ds.key(entity, ID)

	# Prepares the new entity
	new_user_entity = datastore.Entity(key=new_key)
	new_user_entity['Group'] = class_list
	new_user_entity['Name'] = name
	new_user_entity['cruzID'] = cruzID.upper()
	new_user_entity['Priority'] = priority
	new_user_entity['Last_Room'] = None

	# saves the entity 
	ds.put(new_user_entity)

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
	user_dict.clear()
	ParseCSVUserEntity(file)	#parses file and stores in dictionary
	i = 0
	for key in user_dict.keys():
		AddNewUser(i, key, user_dict[key][0], user_dict[key][1], user_dict[key][2])
		i +=1	
##########################################################



###########METHODS FOR Histogram################
def HistogramEnterTime(room_number,cruzID=None):
	# this function makes a histogram of 24 hour long intervals
	# of when the room is most often entered
	# it returns a list of size 24
	ds = get_client()
	hist_24_interval = [0] * 24
	query = ds.query(kind = room_number)
	if(cruzID is not None):
		query.add_filter('cruzID', '=', cruzID)
	logs = list(query.fetch())
	for log_ in logs:
		if log_['Enter_Time'] != None:
			hour = format_time((log_['Enter_Time'].hour))
			# if(hour < 7):
			# 	index = (hour +17) % 25
			# else:
			# 	index = (hour +18) % 25
			hist_24_interval[hour] += 1
	return hist_24_interval

def HistogramExitTime(room_number,cruzID=None):
	# this function makes a histogram of 24 hour long intervals
	# of when the room is most often exited
	# it returns a list of size 24
	ds = get_client()
	hist_24_interval = [0] * 24
	query = ds.query(kind = room_number)
	if(cruzID is not None):
		query.add_filter('cruzID', '=', cruzID)
	logs = list(query.fetch())
	for log_ in logs:
		if log_['Exit_Time'] != None:
			hour = format_time(log_['Exit_Time'].hour)
			# if(hour < 7):
			# 	index = (hour +17) % 25
			# else:
			# 	index = (hour +18) % 25
			hist_24_interval[hour] += 1
	return hist_24_interval

def capacity_over_time(room):
	hourly_capacity = [0] * 24
	room = query_all(room)
	for entry in room:
		if entry['Exit_Time'] != None:
			Enter_hour = format_time(entry['Enter_Time'].hour)
			Exit_hour = format_time(entry['Exit_Time'].hour)
			time_between = abs(Exit_hour - Enter_hour)
			counter = 0
			while counter <= time_between:
				print (counter)
				hourly_capacity[Enter_hour] = hourly_capacity[Enter_hour]+1
				counter = counter +1
				Enter_hour = Enter_hour +1	
	return hourly_capacity

def format_time(hour):
	"""Summary
		formats the hour time correctly for pacific coast time
	Args:
	    hour (int): Hour value of a date object
	
	Returns:
	    int: return the correct hour time
	"""
	if(hour < 7):
		hour = ((hour +17) % 25)
	else:
		hour = ((hour +18) % 25)
	return hour
##########################################################



###########METHODS RESETIGN ROOM CAPACITY AND LOGS################

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
    room_entity.update({
        'Capacity': 0
    })
    client.put(room_entity)

def clear_room_log(room_number):
    # ClearRoomLog() clears all entries in the entity "room_number"
    # can be used at end of day (or whenever user wants) to clear
    # the log of when the room was accessed
    cruzID_list = []
    query = client.query(kind = room_number)

    for log_ in query.fetch():
        cruzID_list.append(log_['cruzID'])  
    unique_cruzID_list = set(cruzID_list)
    
    for j in range(0, len(unique_cruzID_list)):
        cruzID = unique_cruzID_list.pop()   #can only pop from set
        for i in range(0, len(cruzID_list)):
            ID = 'index_{}{}'.format(cruzID, i)
            key = client.key(room_number, ID)
            client.delete(key)

###################################################################