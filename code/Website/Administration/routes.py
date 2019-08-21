# from flask import current_app, Flask, redirect, url_for, render_template, request
# from main import app

# # Add a default root route.
# @app.route("/")
# @app.route("/index")
# def index():
#   Rooms = model.query_all("Rooms")
#   return render_template('index.html', title='Home', rooms=Rooms)

# #page to view all users
# @app.route("/users")
# def users():
#   rooms = {}
#   Users = model.query_all("User")
#   for user in Users:
#     rooms[user['cruzID']] = model.get_users_rooms(user['cruzID'])
#   return render_template('users.html', title='Users',users=Users, Rooms = rooms)		

# #page to view roomlogs
# @app.route("/roomlogs/<string:roomNumber>")
# def roomlogs(roomNumber):
#   logs = model.get_room_logs(roomNumber)
#   return render_template('roomlogs.html', Logs= logs,  room=roomNumber)

# #page to edit the rooms
# @app.route("/editroom/<string:roomNumber>", methods=["GET", "POST"])
# def editroom(roomNumber):
#   room = model.get_entity("Rooms",roomNumber)
#   return render_template('editroom.html', Room=room)   


# @app.route('/upload', methods = ['GET', 'POST'])
# def upload():
#   if request.method == 'POST':
#     submited_file = request.files['file']
#     if submited_file:
#       filename = secure_filename(submited_file.filename)
#       filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename)
#       submited_file.save(filepath)
#       model.CSVToCloudRoom(filepath)
#       return redirect(url_for('index'))
#     return "Error uploading wrong"