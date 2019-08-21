from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, BooleanField, SubmitField
from wtforms.validators import DataRequired


class RoomForm(FlaskForm):
	Groups = StringField('Group', validators=[DataRequired()])
	submit = SubmitField('Save')
