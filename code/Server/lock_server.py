"""Summary

Attributes:
    ALLOWED_IN (bytes): Description
    BUFFER_SIZE (int): Description
    LOGIN_COMMAND (str): Description
    NOT_ALLOWED (bytes): Description
    READ_COMMAND (str): Description
    s (TYPE): Description
    TCP_IP (str): Description
    TCP_PORT (int): Description
"""
import socket, ssl
import threading
import cloudFunctions as cf

TCP_IP = "0.0.0.0"
TCP_PORT = 6001
BUFFER_SIZE = 1024  
LOGIN_COMMAND = "\\login"
READ_COMMAND = "\\READ_ERROR"
RESET_COMMAND = "\\reset"
ALLOWED_IN = b"230 Allowed"
NOT_ALLOWED = b"430 Denied"
COMMAND_NOT_FOUND = b"404 Command not found"



##############################################################################
##############################################################################
'''CODE FOR THE SERVER'''
##############################################################################
############################################################################## 

class server_thread(threading.Thread):
    def __init__(self, conn):
        threading.Thread.__init__(self)
        self.conn = conn
    def run(self):
        try:
            data = self.conn.recv(BUFFER_SIZE).decode("utf-8")
            print (data)
        except UnicodeDecodeError:
            print ("it was not a ascii-encoded unicode string")
            self.conn.send(NOT_ALLOWED)  # echo
            self.conn.close()
            return
        if LOGIN_COMMAND in data:
            message_type,cruzId,room_number = data.split("-")
            # Need to make sure all the cruzID in database are the same case
            cruzId = cruzId.upper()
            if(cruzId is not None and cf.is_allowed(cruzId,room_number)):
                self.conn.send(ALLOWED_IN)
                self.conn.close()
                last_room = cf.get_current_room(cruzId)
                #Checking if logged into different room
                if(last_room is not None and last_room != room_number):
                    print("Logging out of last room " + last_room)
                    cf.logout_room(last_room,cruzId)
                    cf.change_room_capacity(last_room)
                #loging into right room
                if(cf.login_room(room_number,cruzId)):
                    print ("Logging in " + cruzId)
                    cf.set_current_room(cruzId,room_number)
                    cf.set_login_time(room_number)
                    cf.change_room_capacity(room_number,entering=True)
                else:
                    print ("Logging out " + cruzId)
                    cf.logout_room(room_number,cruzId)
                    cf.set_current_room(cruzId,None)
                    cf.change_room_capacity(room_number)
            else:
                print (cruzId + ' not allowed in')
                self.conn.send(NOT_ALLOWED)  # echo
                self.conn.close()
        else:
            self.conn.send(COMMAND_NOT_FOUND)
            self.conn.close()
            print ("Data not recognized")
            pass
        

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(5)

while 1:
    conn, addr = s.accept()
    print ('Connection address:', addr)
    
    thread = server_thread(conn)
    thread.start()
        
