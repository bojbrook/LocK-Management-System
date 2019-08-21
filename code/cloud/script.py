# from google.cloud import datastore
import os
# from datetime import datetime, timezone, timedelta
import csv
import random
import cloud
import socket
import threading
import time
from random import randint

TCP_IP = '35.230.38.111'
TCP_BASE_PORT = 6001
BUFFER_SIZE = 1024

os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "Lock-Management-System.json"


def CreateStudent(file, size):
	#first argument is CSV file with name
	#second arguemnt is size of file
	#this function returns a list with first name, last name, and cruzID
	csv = open(file)
	randomFirstNameIndex = random.randint(1,size)
	randomLastNameIndex = random.randint(1,size)
	for i, line in enumerate(csv):
		if i == randomFirstNameIndex:
			index = line.find(',')
			firstName = line[:index]
		if i == randomLastNameIndex:
			index = line.find(',')
			lastName = line[index+1:-1]

	name = firstName + ' ' + lastName
	nameAndCruzID = name.split(" ")
	nameAndCruzID.append(firstName[0]+lastName)
	# print(nameAndCruzID)
	return nameAndCruzID
			
def UniqueCruzID(listOfStudents):
	#takes in a list of students
	#makes sure all cruzIDs are unique
	#cruzID is intially just first letter of first name + last
	#then it is first two letter of first name
	#then it is first two letters and last name and then incremneting number
	seen = set()
	index = list()
	for lst in listOfStudents:
		if lst[2] in seen:
			lst[2] = lst[0][0:2] + lst[1]
			index.append(lst)
		seen.add(lst[2])
	
	counter = 1
	while(len(index) != 0):
		seen.clear()
		index.clear()
		for lst in listOfStudents:
			if lst[2] in seen:
				lst[2] = lst[2] + str(counter)
			seen.add(lst[2])
		counter = counter + 1;
	return listOfStudents
		
def CreateListStudents(size):
	#first argument is size of list
	#creates a list of all students
	listOfStudents = list()
	i = 0
	while(i < size):
		oneStudent = CreateStudent("names.csv", 200)
		listOfStudents.append(oneStudent)
		i=i+1
	uniqueListOfStudents = UniqueCruzID(listOfStudents)
	return uniqueListOfStudents
	
def CreateRoom():
	if(random.randint(1,2)==2):
		room = "E2_"
	else:
		room = "BE"
	room = room + str(random.randint(1,599))
	return room
	
def CreateCourse():
	number = random.randint(1,4)
	if(number == 1):
		course = "CMPE"
	elif(number == 2):
		course = "EE"
	elif(number == 3):
		course = "CMPS"
	elif(number == 4):
		course = "AMS"

	course = course + str(random.randint(1,300))
	return course
	

def AssignCourseToRoom():
	roomList = list()

	roomList.append(CreateRoom())
	for j in range(0,3):
		roomList.append(CreateCourse())
	return roomList

def CreateListOfRooms(numberOfRooms):
	masterList = list()
	i = 0;
	while(i < numberOfRooms):
		masterList.append(AssignCourseToRoom())
		i = i+1
	return masterList

def CreateRoomsCSV(numberOfRooms):
	# creates some rooms and gives certain classes/groups access
	
	masterList = CreateListOfRooms(numberOfRooms)
	with open('rooms_script.csv', 'w') as file:
		file.write('\n')
		for j, listing in enumerate(masterList):
			file.write(masterList[j][0])
			file.write(',')
			for i, list in enumerate(masterList[j]):
				if(i == 0):
					pass
				else:
					file.write(masterList[j][i])
					file.write(' ')
					
			file.write(',')
			file.write('BELS')
			file.write('\n')
	return masterList
	
	
	
def CreateUsersCSV(numberOfStudents):
	# creates the users csv file with random
	# random firrst name and last name and unique cruzID
	# also enrolls them in anywhere from 2 to 5 classes
	# returns the list of students
	
	listOfStudents = CreateListStudents(numberOfStudents)
	with open('users_script.csv', 'w') as file:
		file.write('\n')
		for j, listing in enumerate(listOfStudents):
			file.write(listOfStudents[j][0])
			file.write(' ')
			file.write(listOfStudents[j][1])
			file.write(',')
			file.write(listOfStudents[j][2])
			file.write(',')
			file.write('2')
			file.write(',')
			numberOfClassesEnrolledIn = random.randint(2,5)
			for i in range(0,numberOfClassesEnrolledIn):
				file.write(CreateCourse())
				if (i == numberOfClassesEnrolledIn):
					pass
				else:
					file.write(' ')
			file.write('\n')
	return listOfStudents

class server_thread(threading.Thread):
	def __init__(self, Student,Room,Port):
		threading.Thread.__init__(self)
		self.Student = Student
		self.Room = Room
		self.Port = Port
	def run(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		MESSAGE = ("\login-" + listOfStudents[self.Student][2] + "-" + masterList[self.Room][0])

		byte_message = MESSAGE.encode('utf-8')
		# Logging in
		s.connect((TCP_IP,self.Port))
		print("Login: " + MESSAGE)
		s.send(byte_message)
		data = s.recv(BUFFER_SIZE)
		print (data)
		s.close()

		sleep_amount = randint(1, 10)
		time.sleep(sleep_amount)

		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		# Logging the user out
		s.connect((TCP_IP, self.Port))
		print("Logout: " + MESSAGE)
		s.send(byte_message)
		data = s.recv(BUFFER_SIZE)
		print (data)
		s.close()
		return

cloud.DeleteAllUserEntities()
cloud.DeleteAllRoomEntities()


# NUMBEROFROOMS = 10
# NUMBEROFSTUDENTS = 20
# masterList = CreateRoomsCSV(NUMBEROFROOMS)
# listOfStudents = CreateUsersCSV(NUMBEROFSTUDENTS)
# cloud.CSVToCloudRoom('rooms_script.csv')
# cloud.CSVToCloudUser('users_script.csv')


# # s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# i = 0
# j = 100
# print("Running Simulation for ", j, " logins\n")
# while(i < j):
# 	randomStudent = random.randint(0,NUMBEROFSTUDENTS-1)
# 	randomRoom = random.randint(0, NUMBEROFROOMS-1)
# 	port = TCP_BASE_PORT
# 	# print(listOfStudents[randomStudent][2], "logging into", masterList[randomRoom][0])
# 	print("starting thread: " + str(port))
# 	thread = server_thread(randomStudent,randomRoom,port)
# 	thread.start()
# 	sleep_amount = randint(0, 15)
# 	time.sleep(sleep_amount)


	# s.connect((TCP_IP, TCP_PORT))
	# MESSAGE = "\login " + listOfStudents[randomStudent][2] + " " + masterList[randomRoom][0]
	# print(MESSAGE +'\n')
	# s.send(MESSAGE)
	# data = s.recv(BUFFER_SIZE)
	# s.close()

	# cloud.LoginRoom(masterList[randomRoom][0], listOfStudents[randomStudent][2])
	# i = i + 1

	


