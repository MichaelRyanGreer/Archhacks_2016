Contributors:
	Jack Leshem
	Thomas Kelly
	Daniel Pollak
	Michael Greer

About:

	A wrist device that connects to a web server and is able to notify emergency contacts through Cisco Tropo
	Uses Particle Photon to connect to WiFi
	Webhooks send information to flask server on ec2 instance
	Flask server retrieves patient information from mysql database
	Server prompts Tropo app to text emergency contacts
	Programmed in the Particle IDE
	Used Adafruit Neopixel Library, adapted to Particle by the Particle Community
	Used Adafruit CAP1188 Library, adapted to Particle by user Sheva29	


Modes:

	Not worn: 	When capacitive sensor does not detect wrist
	Worn:		When capacitive sensor detects wrist
	Alert:		Either when red button is pressed or accelerometer detects quick motion (while in Worn mode)
			Returns to Worn when green button is pressed
	Emergency:	Automatically after 10 seconds or if red button is pressed (while in Alert mode)
			Sends data to flask server and progresses automatically to Wait
	Wait:		Buzzer and LEDs stay on to help rescuers find patient
			Returns to Worn when green button is pressed
	Time:		Displays the current time on the LED band and returns to Worn
