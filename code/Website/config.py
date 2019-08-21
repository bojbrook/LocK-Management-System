import os


# set the env variable through the script
os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "Lock-Management-System.json"

# The secret key is used by Flask to encrypt session cookies.
SECRET_KEY = 'secret'

DATA_BACKEND = 'datastore'

PROJECT_ID = 'lock-management-system'

UPLOAD_FOLDER = 'Administration/static/uploads'

