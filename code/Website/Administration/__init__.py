import os
import math
from datetime import datetime
from flask import current_app, Flask, redirect, url_for, render_template, request,flash,make_response, send_file, session
from werkzeug import secure_filename
# from Administration.forms import RoomForm
# import pandas as np
# import numpy as nump
import json


def create_app(config, debug=True, testing=False, config_overrides=None):
  app = Flask(__name__)
  # Bootstrap(app)
  
  app.config.from_object(config)
 
  # from Administration import routes

  # Add a default root route.
  @app.route("/")
  @app.route("/index")
  def index():
    if not session.get('logged_in'):
      return render_template('login.html')
    else:
      room_logs = {}
      Rooms = model.query_all("Rooms")
      # for room in Rooms:
      #   room_logs[room['Room_Number']] = (model.get_last_5logs(room['Room_Number']))
      #   print (room_logs[room['Room_Number']])
      return render_template('index.html', title='Home', rooms=Rooms, small_logs = room_logs)

  @app.route("/login", methods=["POST"])
  def login():
    if request.form['password'] == 'password' and request.form['username'] == 'admin':
        session['logged_in'] = True
    else:
        flash('wrong password!')
    return redirect(url_for('index'))

  @app.route("/logout")
  def logout():
    session['logged_in'] = False
    return redirect(url_for('index'))

  #page to view all users
  @app.route("/users")
  def users():
    rooms = {}
    Users = model.query_all("User")
    # for user in Users:
    #   # rooms[user['cruzID']] = ""
    #   rooms[user['cruzID']] = model.get_users_rooms(user['cruzID'])
    return render_template('users.html', title='Users',users=Users, Rooms = rooms)

  @app.route("/delete/user/<string:cruzID>",methods=["POST"])
  def delete_user(cruzID):
    if request.method == "POST":
      print(cruzID)
      model.delete_entity('User',cruzID.upper())
      return redirect(url_for('users'))
    return "ERROR"

  #page to view individual userlogs
  @app.route('/users/<string:cruzID>', methods=["GET", "POST"])
  def view_user_logs(cruzID):
    if request.method == 'GET':
      user_room_log = {}
      user = model.get_entity("User",cruzID)
      rooms = model.get_users_rooms(cruzID)
      for room in rooms:
        print(room)
        user_room_log[room] = model.get_room_logs(room,cruzID)
        print (user_room_log[room])
        for log in user_room_log[room]:
          log['Enter_Time'] = format_time(log['Enter_Time'])
          log['Exit_Time'] = format_time(log['Exit_Time'])

      return render_template('userlogs.html', title=cruzID, user = user, rooms = rooms, roomlogs = user_room_log)
    return "Error"

  #Page to display bar chart for a users given room activities
  @app.route('/users/<string:cruzID>/histogram/<string:room_number>')
  def user_histogram(cruzID,room_number):
    user = model.get_entity("User",cruzID)
    enter_histo = model.HistogramEnterTime(room_number,cruzID)
    exit_histo = model.HistogramExitTime(room_number,cruzID)

    time = get_time_array()
    print (time)
    print("ENTER")
    print (enter_histo) 
    print("EXIT")
    print (exit_histo)
    return render_template("user_histogram.html",room=room_number,user=user,time=time,enter=enter_histo,exit = exit_histo)

  #Page to display the bar chart for a given room
  @app.route('/roomlogs/<string:room_number>/histogram')
  def room_histogram(room_number):
    enter_histo = model.HistogramEnterTime(room_number)
    exit_histo = model.HistogramExitTime(room_number)
    capacity_histo = model.capacity_over_time(room_number)
    time = get_time_array()
    return render_template("room_histogram.html",room=room_number,time=time,enter=enter_histo,exit = exit_histo, capacity = capacity_histo)


  #page to view roomlogs
  @app.route("/roomlogs/<string:room_number>")
  def roomlogs(room_number):
    room = model.get_entity("Rooms",room_number)
    logs = model.get_room_logs(room_number)
    # Formating the time 
    for log in logs:
      log['Enter_Time'] = format_time(log['Enter_Time'])
      log['Exit_Time'] = format_time(log['Exit_Time'])
    return render_template('roomlogs.html', Logs=logs,  room=room)

  #adds Group to a given room
  @app.route("/addgroup/<string:room_number>", methods=["GET", "POST"])
  def addgroup(room_number):
    if request.method == 'POST':
      group_input = request.form['addGroup']
      if(group_input is not ""):
        model.update_room_group(room_number,group_input)
      return redirect(url_for('roomlogs',room_number=room_number))
    return redirect(url_for('index'))

  @app.route("/removegroup/<string:room_number>/<string:group_name>", methods=["GET", "POST"])
  def removegroup(room_number,group_name):
    if request.method == 'POST':
      model.remove_group(room_number,group_name)
    return redirect(url_for('roomlogs', room_number=room_number))


  #adds Group to a given room
  @app.route("/adduser/<string:room_number>", methods=["GET", "POST"])
  def adduser(room_number):
    if request.method == 'POST':
      cruzID_input = request.form['addUser']
      if(cruzID_input is not ""):
        model.update_room_users(room_number,cruzID_input)
      return redirect(url_for('roomlogs',room_number=room_number))
    return redirect(url_for('index'))

    #Removes user from group
  @app.route("/removeuser/<string:room_number>/<string:user_name>", methods=["GET", "POST"])
  def removeuser(room_number,user_name):
    if request.method == 'POST':
      model.remove_user(room_number,user_name)
    return redirect(url_for('roomlogs', room_number=room_number))
  
  #page deletes rooms
  @app.route("/deleteroom/<string:roomNumber>", methods=["GET", "POST"])
  def deleteroom(roomNumber):
    if request.method == 'POST':
      print ("Deleting room " + roomNumber)
      model.delete_entity("Rooms",roomNumber)
      return redirect(url_for('index'))
    return "ERROR"

  #page deletes rooms
  @app.route("/export/<string:room_number>", methods=["GET", "POST"])
  def export(room_number):
    if request.method == 'GET':
      model.roomlog_to_csv(room_number)
      path = "/" + room_number +".csv"
      print (path)
      return redirect(url_for('roomlogs', room_number=room_number))

  @app.route('/upload', methods = ['GET', 'POST'])
  def upload():
    if request.method == 'POST':
      submited_file = request.files['file']
      
      if submited_file:
        filename = secure_filename(submited_file.filename)
        retVal = checkfile(filename)
        if (retVal == 0):
          return "Error Wrong extention"
        elif ( retVal == 1):
          filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename)
          submited_file.save(filepath)
          model.CSVToCloudRoom(filepath)
        elif (retVal == 2):
          filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename)
          submited_file.save(filepath)
          model.CSVToCloudUser(filepath)
        else:
          return "Error uploading wrong"
        return redirect(url_for('index'))
      return "Error uploading wrong"


# ******************************************
# METHODS FOR ANDROID ACCESS TO THE DATABASE
# ******************************************
  @app.route('/android/<string:cruzID>', methods = ['GET','POST'])
  def android_user_labs(cruzID):
    if request.method == 'GET':
      data =[]
      rooms = model.get_users_rooms(cruzID)
      for room in rooms:
        room_entity = model.get_entity("Rooms",room) 
        capacity = room_entity['Capacity']
        data.append({'Room': room, 'Capacity' : capacity})
        
      return json.dumps(data)
    return "ERROR"

  @app.route('/android/<string:cruzID>/<string:room_number>', methods = ['GET','POST'])
  def android_user_logs(cruzID,room_number):
    if request.method == 'GET':
      data =[]
      logs = model.get_room_logs(room_number,cruzID=cruzID)
      for log in logs:
        Enter_Time = log['Enter_Time']
        Exit_Time = log['Exit_Time']
        Enter_String = format_time(Enter_Time)
        Exit_String = format_time(Exit_Time)
        #calculating the time for each visit
        if(Exit_Time is not None):
          delta = (Exit_Time - Enter_Time)
          days, seconds = delta.days, delta.seconds
          hours = (days * 24 + seconds) / 3600
          minutes = math.ceil((seconds % 3600) / 60)
        else:
          hours = 0
          minutes = 0

        data.append({'Enter_Time': Enter_String, 'Exit_Time':Exit_String, 
        'Delta_hour':int(hours), 'Delta_minute':int(minutes)})
      return json.dumps(data)
      # return "Please Work"
      # data =[]
      # rooms = model.get_users_rooms(cruzID)
      # for room in rooms:
      #   room_entity = model.get_entity("Rooms",room) 
      #   capacity = room_entity['Capacity']
      #   data.append({'Room': room, 'Capacity' : capacity})
        
      # return json.dumps(data)
    return "ERROR"

    

  # Setup the data model.
  with app.app_context():
    model = get_model()
    model.init_app(app)

  return app


def format_time(time):
  if time is None:
    return ""
  minute = time.minute
  if(minute < 10):
    str_minute = "0"+str(minute)
  else:
    str_minute = str(minute)
  hour = time.hour
  if(hour < 7):
    hour = ((hour +17) % 25)
  else:
    hour = ((hour +18) % 25)
  if(hour > 12):
    hour = hour - 12
    str_minute = str_minute + "PM"
  else:
    str_minute = str_minute + "AM"
    
  hour = str(hour)
  day = str(time.day)
  month = str(time.month)
  year = str(time.year)
  date_string = month + "/" + day + "/" + year
  time_string = hour + ":" + str_minute
  # print (date_string)
  # print(time_string)
  return time_string + " " + date_string

# returns an array time values
def get_time_array():
  time = [None] * 24
  for index in range(0,24):
    if(index == 0):
      time[index] = "12:00AM"
    elif(index < 12 and index > 0):
      time[index] = str(index) + ":00AM"
    elif(index == 12):
      time[index] = "12:00PM"
    else:
      time[index] = str((index -12)) + ":00PM"
  return time


def checkfile(filename):
  name, ext = filename.split('.')
  print ("Name: " + name)
  print ("ext: " + ext)

  if(ext != "csv"):
    return 0
  elif(name.lower() == "rooms"):
    return 1
  elif(name.lower() == "users"):
    return 2
  else:
    return -1

def get_model():
  model_backend = current_app.config['DATA_BACKEND']
  # from . import dstore
  # model = dstore
  if model_backend == 'cloudsql':
      from . import model_cloudsql
      model = model_cloudsql
  elif model_backend == 'datastore':
      from . import dstore
      model = dstore
  elif model_backend == 'mongodb':
      from . import model_mongodb
      model = model_mongodb
  else:
      raise ValueError(
          "No appropriate databackend configured. "
          "Please specify datastore, cloudsql, or mongodb")

  return model