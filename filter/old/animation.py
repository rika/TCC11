 #!/usr/bin/python
 #-*- coding: utf-8 -*-
'''
Animation of tracking data
Author: Ricardo Juliano Mesquita Silva Oda <oda.ric@gmail.com>
Data: 24/09/11
Filename: simulation.py

To run:
python simulation.py [FILENAME] [STARTFRAME] [ENDFRAME]

The program will run an animation of the tracking data in the interval
of frames given as parameters.

Input file:
The input begins with two integers S T, the number of the starting
and ending frame of the experiment. After this there's T-S+1 blocks.
Each block starts with an integer N, and the next N following lines
have four numbers: S H Y X.
S is an integer representing the number of the subject;
H is an float for the height of the subject;
Y is the coordinate y of the subject;
X is the coodinate x of the subject.
'''
from Tkinter import *
import re, time, sys
from model import Model

WIDTH = 1007
HEIGHT = 400
OBJ_SIZE = 4

color = [
    "black",
    "red",
    "blue",
    "green",
    "yellow",
    "brown",
    "cyan",
    "grey"
    ]


# Simulation class that stores the objects in a canvas
class Simulation:
    def __init__ (self, canvas):
        self.canvas = canvas

    def clear (self):
        self.canvas.delete(ALL)

    def add (self, obj):
        x = obj.cx
        y = obj.cy
        ink = color[obj.subject%len(color)]
        self.canvas.create_rectangle(x-OBJ_SIZE, \
                y-OBJ_SIZE, x+OBJ_SIZE, y+OBJ_SIZE, fill=ink)
        item = self.canvas.create_text(x, y+3*OBJ_SIZE, text=str(obj.subject)+": ("+str(x)+","+str(y)+")")

# Stores objects of in a frame
class FrameData:
    def __init__(self, objects):
        self.objects = objects

    def get_objects(self):
        return self.objects

# Stores the information (frames and objects) from the input file
class Data:
    def __init__(self, filename):
        self.frames = []

        # Open file and get lines
        infile = open(sys.argv[1], "r")
        lines = infile.readlines()
        infile.close()

        # Get starting and ending frame numbers
        s = re.match('(\d+)\s(\d+)$', lines[0])
        lines = lines[1:]
        self.start = int(s.group(1))
        self.end   = int(s.group(2))

        for frame in range(self.start, self.end+1):
            # n is the number of objects in the frame
            s = re.search('(\d+)$', lines[0])
            lines = lines[1:]
            n = int(s.group(1))
       
            # Get objects of the frame
            objects = []
            for _i in range(n):
                s = re.search('(\d+)\s(\d+.?\d*)\s(-?\d+)\s(-?\d+)$', lines[0])
                lines = lines[1:]
                subject = int(s.group(1))
                height = float(s.group(2))
                cx = int(s.group(3))
                cy = int(s.group(4))
                objects.append(Model(frame, subject, height, cx, cy))
            self.frames.append(FrameData(objects))

    def get_objects(self, frame):
        return self.frames[frame-self.start].get_objects()



# Parameters
if len(sys.argv) != 4:
    print "USAGE: python " + sys.argv[0] + " [FILENAME] [START_FRAME] [END_FRAME]"
    exit(0)

start = int(sys.argv[2])
end   = int(sys.argv[3])

# Loading data from input file
data = Data(sys.argv[1])

if start < data.start or end > data.end or start > end:
    print "Invalid frame interval"
    print "This data file can be animated in the interval [", \
        data.start,",",data.end,"]"
    exit(-1)

# Setup
root = Tk()
root.title("Simulation")
root.resizable(0, 0)
root.geometry("+20+20")
frame = Frame(root, bd=5, relief=SUNKEN)
frame.pack()
canvas = Canvas(frame, width=WIDTH, height=HEIGHT)
canvas.pack(side = TOP)
frameNumber = StringVar()
label = Label(frame, textvariable=frameNumber)
label.pack(side = BOTTOM)
sim = Simulation(canvas)
root.update()

# Animation loop
try:
    for frame in range(start, end+1):
        frameNumber.set("Frame: "+str(frame))
        objects = data.get_objects(frame)
        sim.clear()
        for obj in objects:
            sim.add(obj)
        root.update() # redraw
        #time.sleep(1.0/20)
        print frame
        sys.stdin.readline()
except TclError:
    pass # to avoid errors when the window is closed


