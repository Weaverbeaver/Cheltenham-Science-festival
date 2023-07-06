# Flask for web servers
from flask import Flask, jsonify, request
import time
from datetime import datetime
import threading

# SQLite connector
import sqlite3
conn = sqlite3.connect('marblerun.db', check_same_thread=False)
cur = conn.cursor()
cur.execute('''CREATE TABLE IF NOT EXISTS Leaderboard (Timestamp, Name VARCHAR(30), Score INT)''')
cur.execute('''CREATE TABLE IF NOT EXISTS CurrentGame (StartTime timestamp DEFAULT current_timestamp, MarbleType INT, Game1Score INT, Game2Score INT, Game3Score INT, Game4Score INT, FinishTime timestamp)''')
cur.execute('''CREATE TABLE IF NOT EXISTS Targets (MarbleType INT, Game1Score INT, Game2Score INT, Game3Score INT, Game4Score INT)''')
cur.execute('''DELETE FROM Targets''')
cur.execute('''INSERT OR REPLACE INTO Targets (MarbleType,Game1Score,Game2Score,Game3Score,Game4Score) VALUES (0,100,8,100,200)''')
cur.execute('''INSERT OR REPLACE INTO Targets (MarbleType,Game1Score,Game2Score,Game3Score,Game4Score) VALUES (1,100,8,100,200)''')
cur.execute('''INSERT OR REPLACE INTO Targets (MarbleType,Game1Score,Game2Score,Game3Score,Game4Score) VALUES (2,100,8,100,200)''')
cur.execute('''INSERT OR REPLACE INTO Targets (MarbleType,Game1Score,Game2Score,Game3Score,Game4Score) VALUES (3,100,8,100,200)''')
cur.execute('''INSERT OR REPLACE INTO Targets (MarbleType,Game1Score,Game2Score,Game3Score,Game4Score) VALUES (4,100,8,100,200)''')
conn.commit()

# Library for PWM controllers
from adafruit_servokit import ServoKit
# Configure PWM controllers
# If upper or lower servos are found, twitch each to show they are working then move to start positions
# Otherwise, print servos not found to the terminal
try:
    lower = ServoKit(channels=16,address=127)
    lower.servo[0].angle = 45
    time.sleep(1)
    lower.servo[0].angle = 0
    lower.servo[1].angle = 70
    time.sleep(1)
    lower.servo[1].angle = 20
    time.sleep(1)
except:
    print("""\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n!!!!!Lower servos not found!!!!!\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n""")
try:
    upper = ServoKit(channels=16,address=124)
    upper.servo[0].angle = 180
    time.sleep(1)
    upper.servo[0].angle = 90
    upper.servo[1].angle = 80
    time.sleep(1)
    upper.servo[1].angle = 35
except:
    print("""\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n!!!!!Upper servos not found!!!!!\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n""")

# Library for Neopixel 
#sudo pip3 install adafruit-circuitpython-neopixel
import board
import neopixel
totalLEDs = 635
pixels = neopixel.NeoPixel(board.D21, totalLEDs, brightness=10, auto_write=False)

def ServoStartPos():
	# Set servos to correct location
	# Not needed anymore? If more than one marble is in the run this causes problems
	StartPosition1 = 90
	StartPosition2 = 35
	StartPosition3 = 0
	StartPosition4 = 20
	try:
		upper.servo[0].angle = StartPosition1
		upper.servo[1].angle = StartPosition2
	except:
		pass
	try:
		lower.servo[0].angle = StartPosition3
		lower.servo[1].angle = StartPosition4
	except:
		pass

# Open Gates one by one
def OpenGates(completedgates):
    openangles = [180,80,45,70]
    closeangles = [90,35,0,20]
    try:
        time.sleep(1)
        upper.servo[0].angle = openangles[0]
        time.sleep(2)
        upper.servo[0].angle = closeangles[0]
        time.sleep(1)
        upper.servo[1].angle = openangles[1]
        time.sleep(2)
        upper.servo[1].angle = closeangles[1]
    except:
        pass
    try:
        time.sleep(1)
        lower.servo[0].angle = openangles[2]
        time.sleep(2)
        lower.servo[0].angle = closeangles[2]
        time.sleep(1)
        lower.servo[1].angle = openangles[3]
        time.sleep(2)
        lower.servo[1].angle = closeangles[3]
    except:
        pass
        

def UpdateLEDs(totaliser):
#	LEDmax = round(totalLEDs/100*totaliser)
#	for x in range(0,LEDmax,2):
#		pixels[x] = (150,0,0)
#	for x in range(1,LEDmax,2):
#		pixels[x] = (100,50,0)
#	for x in range(LEDmax,totalLEDs):
#		pixels[x] = (0,0,0)
#	pixels.show()
#	return LEDmax
	return 0

def FlashLEDs():
#	for x in range(0,300):
#		for x in range(0,totalLEDs,2):
#			pixels[x] = (150,0,0)
#		for x in range(1,totalLEDs,2):
#			pixels[x] = (100,50,0)
#		pixels.show()
#		time.sleep(1)
#		for x in range(0,totalLEDs):
#			pixels[x] = (0,0,0)
#		time.sleep(1)
#		pixels.show()
	return totalLEDs

# Update leaderboard
def UpdateLeaderboard():
	cur.execute("SELECT StartTime from CurrentGame LIMIT 1")
	StartTime = datetime.strptime(cur.fetchone()[0],"%Y-%m-%d %H:%M:%S")
	cur.execute("SELECT Timestamp from Leaderboard ORDER BY Timestamp DESC LIMIT 1")
	lastfinish = cur.fetchone()
	conn.commit()
	ElapsedTime = int((datetime.now() - StartTime).total_seconds())
	if lastfinish:
		lastfinish = datetime.strptime(lastfinish[0],"%Y-%m-%d %H:%M:%S")
		if lastfinish < StartTime:
			cur.execute('''INSERT INTO Leaderboard (Timestamp,Score) VALUES (current_timestamp,%i)''' % (ElapsedTime))
			conn.commit()
			return ElapsedTime
	else:
		print("First fnisher")
		cur.execute('''INSERT INTO Leaderboard (Timestamp,Score) VALUES (current_timestamp,%i)''' % (ElapsedTime))
		conn.commit()
		return ElapsedTime
	return "Already complete"

# Update stats for LED totaliser
def UpdateTotaliser(targets,scores):
	game1 = scores[1]/targets[1]*100
	game2 = scores[2]/targets[2]*100
	game3 = scores[3]/targets[3]*100
	game4 = scores[4]/targets[4]*100
	if game1 > 100:
		game1 = 100
	if game2 > 100:
		game2 = 100
	if game3 > 100:
		game3 = 100
	if game4 > 100:
		game4 = 100
	totaliser = (game1 + game2 + game3 + game4)/4
	UpdateLEDs(totaliser)
	if totaliser == 100:
		UpdateLeaderboard()
		FlashLEDs()
	return {"total": totaliser,"game1": game1,"game2":game2,"game3":game3,"game4":game4}

app = Flask(__name__)

@app.route('/')
def index():
	return "Hello world"

@app.route('/nameteam')
def nameteam():
	teamname = request.args.get('name')
	cur.execute("SELECT Timestamp,Name,Score from Leaderboard ORDER BY Timestamp DESC LIMIT 1")
	lastgame = cur.fetchone()
	conn.commit()
	ReturnLastGame = {"Timestamp": lastgame[0], "PreviousName": lastgame[1], "CurrentName": teamname, "Score": lastgame[2]}
	cur.execute("UPDATE Leaderboard Set Name=:teamname WHERE Timestamp=:timestamp", {"timestamp": lastgame[0]})
	conn.commit()
	return jsonify(ReturnLastGame)

@app.route('/reset')
def gamereset():
	# Resets the game and selects a marble type
	
	# flush status of current game from database and set marble type
	cur.execute("DELETE FROM CurrentGame")
	cur.execute("SELECT MarbleType FROM Targets ORDER BY RANDOM() LIMIT 1")
	conn.commit()
	MarbleTypes = cur.fetchone()
	MarbleType = MarbleTypes[0]
	cur.execute('''INSERT INTO CurrentGame (MarbleType,Game1Score,Game2Score,Game3Score,Game4Score) VALUES (%i,0,0,0,0)''' % (MarbleType))
	conn.commit()
	
	ServoStartPos()
	
	# Update LEDs to zero
	UpdateLEDs(0);
	
	returntext = "New Game!  Marble type: " + str(MarbleType)
	return returntext

@app.route('/targets')
def targets():
	# Returns the target score for each game
	cur.execute("SELECT MarbleType from CurrentGame")
	currentgame = cur.fetchone()
	MarbleType = currentgame[0]
	cur.execute('''SELECT * from Targets WHERE MarbleType = %i''' % (MarbleType))
	conn.commit()
	targets = cur.fetchone()
	return jsonify(marbletype=targets[0],game1target=targets[1],game2target=targets[2],game3target=targets[3],game4target=targets[4])

@app.route('/updatescore')
def updatescore():
	# Retrieve game and score
	game = request.args.get('game')
	score = int(request.args.get('score'))
	
	# Update current score in database
	if game == "pacman":
		cur.execute("UPDATE CurrentGame Set Game1Score=:score", {"score": score})
		conn.commit()
	if game == "simon":
		cur.execute("UPDATE CurrentGame Set Game2Score=:score", {"score": score})
		conn.commit()
	if game == "hacking":
		cur.execute("UPDATE CurrentGame Set Game3Score=:score", {"score": score})
		conn.commit()
	if game == "coding":
		cur.execute("UPDATE CurrentGame Set Game4Score=:score", {"score": score})
		conn.commit()
	
	# Get current game scores
	cur.execute("SELECT MarbleType,Game1Score,Game2Score,Game3Score,Game4Score FROM CurrentGame LIMIT 1")
	conn.commit()
	currentscores = cur.fetchone()
	MarbleType = currentscores[0]
	
	# Retrieve target score
	cur.execute("SELECT MarbleType,Game1Score,Game2Score,Game3Score,Game4Score from Targets WHERE MarbleType=:marble", {"marble": MarbleType})
	conn.commit()
	targets = cur.fetchone()
	
	# Count completed games
	completedgames=0
	for x in range(1,5):
		if targets[x] <= currentscores[x]:
			completedgames = completedgames + 1
		
	# open gates for completed games
	if completedgames > 0:
		motor_thread1 = threading.Thread(target=OpenGates(1))
		motor_thread1.start()
		#OpenGates(completedgames)
		
	# Update totaliser
	totaliser = UpdateTotaliser(targets,currentscores)
	
	JSONreturn = {"MarbleType": MarbleType, "targets": targets, "currentscores": currentscores, "completedgames": completedgames, "totaliser": totaliser}
	return jsonify(JSONreturn)

if __name__ == '__main__':
	app.run(host='0.0.0.0', port=80, debug=False)
