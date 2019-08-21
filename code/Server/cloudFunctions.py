"""Summary
Holds all the functions for the accessing and interacting with the datastore
"""
import os
from google.cloud import datastore
from datetime import datetime, timezone, timedelta

os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "Lock-Management-System.json"

def get_client():
    """Summary
    gets the client for the google datastore
    Returns:
        Datastore: client for the google datastore
    """
    return datastore.Client()

def get_entity(kind,name):
    """Summary
    gets a specific entitiy from the database
    Args:
        kind (str): The kind of entity
        name (str): The name of the enitity
    
    Returns:
        entity: The enitity from the datastore
    """
    client = get_client()
    ID = 'index{}'.format(name)
    key = client.key(kind,ID)
    return client.get(key)

##############################################################################
##############################################################################
'''Functions for checking if user is allowed in room'''
##############################################################################
##############################################################################  


def is_users_from_room(room_number,cruzID):
    """Summary
        is_users_from_room() is called to check if the given cruzID
        has acces to the room based on their cruzID.
    Args:
        room_number (str): A string containg the room name
        cruzID (str): A string containing the cruzID of the user
    
    Returns:
        boolean: True if user has access to the given room
    """
    client = get_client()
    entity = 'Rooms'
    ID = 'index{}'.format(room_number)
    key = client.key(entity,ID)
    room_entity = client.get(key)
    if(room_entity is not None):
        user_list = list(room_entity["Users"])
        if(cruzID in user_list):
            return True
        return False
    else:
        return False
		

def get_user_Group(cruzID):
    """Summary
    get_user_Group() gets all of the Group that the given cruzID has access to.
    Args:
        cruzID (str): A string containing the users cruzID
    
    Returns:
        list: A list of all of the Group for that user
    """
    user = get_entity("User",cruzID)
    if(user is None):
        return None
    else:
        return list(user['Group'])

def get_room_Group(room_number):
    """Summary
        get_room_Group() gets all of the Group that have access to the room
    Args:
        room_number (str): A string containing the room number
    
    Returns:
        list: A list of all of the Group for that room
    """
    room = get_entity("Rooms",room_number)
    return list(room['Group'])

def is_allowed(cruzID,room_number):
    """Summary
        if the given user is allowed into the room based on CRUZID AND ROOM_NUMBER
    Args:
        room_number (str): A string containg the room name
        cruzID (str): A string containing the cruzID of the user
    
    Returns:
        boolean: True if user can access the given room
    """
    
    # checks if in the user group of room
    if(is_users_from_room(room_number,cruzID)):
    	return True

    rooms_groups = get_room_Group(room_number)
    users_groups = get_user_Group(cruzID)

    if(rooms_groups is None or users_groups is None):
        return False

    for room in rooms_groups:
        for user in users_groups:
            if(user == room):
                return True
    return False


##############################################################################
##############################################################################
'''Logging in and out of Rooms'''
##############################################################################
##############################################################################
def login_room(room_number, cruzID):
    """Summary
    checks if there is already an exact entity
    case is if user enter and exits room multiple time  
    Args:
        room_number (str): name of the room to access
        cruzID (str): cruzId of the user seeking access
    
    Returns:
        boolean: True if the user isnt' already logged into the room
    """
    client = get_client()
    index = 0
    cruzID = cruzID.upper()
    ID = 'index_{}{}'.format(cruzID, index)
    new_key = client.key(room_number, ID)
    check_ = client.get(new_key)
    if check_ != None:
        while check_['Exit_Time'] is not None:
            index += 1
            ID = 'index_{}{}'.format(cruzID, index)
            new_key = client.key(room_number, ID)
            check_ = client.get(new_key)
            if check_ == None:
                break
    ##################################################
    if(check_ is not None and check_['Enter_Time'] is not None):
        return False
    # Prepares the new entity
    new_room_entity = datastore.Entity(key = new_key)
    new_room_entity.update({
        '24hr_Cumulative_Time': None,
        'Delta_Time': None,
        'Enter_Time': datetime.now(timezone.utc).replace(microsecond=0),
        'Exit_Time': None,
        'cruzID': cruzID
    })
    client.put(new_room_entity)
    return True


def logout_room(room_number, cruzID):
    """Summary
    LogoutRoom() is called of LoginRoom() return false. This 
    funtion will log a user out of thier current session.
    
    Args:
        room_number (str): name of the room to access
        cruzID (str): cruzId of the user seeking access
    """
    client = get_client()
    index = 0
    cruzID = cruzID.upper()
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
    delta_time = leave_time - arrival_['Enter_Time']

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
        
    # new_room_entity['24hr_Cumulative_Time'] = str(total_time)   
    new_room_entity.update({
        '24hr_Cumulative_Time': str(total_time) ,
        'Delta_Time': str(delta_time) ,
        'Enter_Time': arrival_['Enter_Time'],
        'Exit_Time': leave_time,
        'cruzID': cruzID 
    })
    # stores in cloud
    client.put(new_room_entity)


##############################################################################
##############################################################################
'''Functions for updating room information'''
##############################################################################
##############################################################################

def set_login_time(room_number):
    """Summary
    Sets the last login time for given ROOM_NUMBER
    Args:
        room_number (str): the name of the given room
    """
    client = get_client()
    current_time = datetime.now(timezone.utc).replace(microsecond=0)
    room_entity = get_entity("Rooms", room_number)
    if room_entity is not None:
        room_entity.update({
            'Last_Login_time': current_time
        })
        client.put(room_entity)


def get_current_room(cruzID):
    """Summary
    Gets the last room of the given CRUZID. Return NULL if the users isn't
    currently logged into a room
    Args:
        cruzID (str): Users cruzID
    
    Returns:
        str: The current room number for the given user
    """
    client = get_client()
    entity = 'User'
    ID = 'index{}'.format(cruzID)
    key = client.key(entity,ID)
    user_entity = client.get(key)
    if(user_entity is None):
        return None
    return user_entity['Last_Room']

def set_current_room(cruzID,room_number):
    """Summary
    Sets the current room for the user
    Args:
        cruzID (str): cruzID for the user
        room_number (str): Name of the room
    """
    client = get_client()
    user_entity = get_entity("Users",cruzID)
    if user_entity is not None:
        user_entity.update({
            'Last_Room': room_number
        })
        client.put(user_entity)

def change_room_capacity(room_number,entering = False):
    """Summary
    change_room_capacity() changes the capacity of the room in the datastore 
    Args:
        room_number (str): A string containing the room_number
        entering (boolean): A boolean on if the user is leaving or entering
    """
    client = get_client()
    entity = 'Rooms'
    ID = 'index{}'.format(room_number)
    key = client.key(entity,ID)
    room_entity = client.get(key)
    room_capacity = room_entity["Capacity"]
    if(entering):
        room_capacity += 1
    elif(not entering and room_capacity > 0):
        room_capacity -= 1
    room_entity.update({
        'Capacity': room_capacity
    })
    client.put(room_entity)


