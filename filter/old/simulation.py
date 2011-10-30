 #!/usr/bin/python
 #-*- coding: utf-8 -*-
'''
Animation of tracking data
Author: Ricardo Juliano Mesquita Silva Oda <oda.ric@gmail.com>
Data: 
Filename: .py

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
import re, time, random, sys
from model import Model

WIDTH = 1007
HEIGHT = 400
OBJ_SIZE = 4
DELTA = 70
DELTA_T = 14
WIN_SIZE = 10
MAX_Y_ERROR = 4
MIN_SUB = 20

color = [
    "black",
    "red",
    "blue",
    "green",
    "yellow",
    "brown",
    "cyan",
    "grey",
    "purple",
    "orange"
    ]


# Simulation class that stores the objects in a canvas
class Simulation:
    def __init__ (self, canvas):
        self.canvas = canvas
        self.tracked_points = []
        self.fail_points = []

    def clear (self):
        self.tracked_points = []
        self.fail_points = []
        self.canvas.delete(ALL)

    def draw_obj (self, obj):
        x = obj.cx
        y = obj.cy
        ink = color[obj.subject%len(color)]
        self.canvas.create_rectangle(x-OBJ_SIZE, \
                y-OBJ_SIZE, x+OBJ_SIZE, y+OBJ_SIZE, fill=ink)
        item = self.canvas.create_text(x, y+3*OBJ_SIZE, text=str(obj.subject)+": ("+str(x)+","+str(y)+")")

    def add_point (self, obj):
        x = obj.cx
        y = obj.cy
        self.tracked_points.append((x,y))

    def render (self, objects):
        self.canvas.delete(ALL)
        l = len(self.tracked_points)
        if l > 0:
           x = self.tracked_points[l-1][0]
           y = self.tracked_points[l-1][1]
           self.canvas.create_oval(x-DELTA, \
                   y-DELTA, x+DELTA, y+DELTA, outline="red")
           for t in self.tracked_points:
               self.canvas.create_rectangle(t[0], t[1], t[0], t[1])
           for f in self.fail_points:
               self.canvas.create_rectangle(f[0], f[1], f[0], f[1], outline="red")

        for obj in objects:
            self.draw_obj(obj)

    def fail (self):
        self.fail_points.append(self.tracked_points.pop())

    def clear_track (self):
        self.tracked_points = []
        

# Stores the information (frames and objects) from the input file
class Data:
    def __init__(self, filename):

        self.frames = []
        self.objects = []

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
            objs = []
            for _i in range(n):
                s = re.search('^(\d+)\s(\d+\.?\d*)\s(\-?\d+)\s(\-?\d+)$', lines[0])
                lines = lines[1:]

                subject = int(s.group(1))
                height = float(s.group(2))
                cx = int(s.group(3))
                cy = int(s.group(4))
                o = Model(frame, subject, height, cx, cy)
                objs.append(o)
                self.objects.append(o)
            self.frames.append(objs)

    def remove_object(self, obj):
        self.objects.remove(obj)
        self.frames[obj.frame-self.start].remove(obj)

    def get_objects(self, frame):
        return self.frames[frame-self.start]

    def get_random_object(self):
        if self.objects:
            l = len(self.objects)-1
            return self.objects[random.randint(0, l)]
        else:
            return None


def least_squares_line(points):
    n = len(points)
    sx = 0.0
    sy = 0.0
    sxy = 0.0
    sx2 = 0.0
    for i in range(n):
        sx += points[i].cx
        sy += points[i].cy
        sxy += points[i].cx * points[i].cy
        sx2 += points[i].cx * points[i].cx
    c = n*sx2 - sx*sx
    if c == 0:
        c = 0.0001
    a = (n*sxy - sx*sy) / c
    b = (sy*sx2 - sx*sxy) / c

    y_error = 0.0
    for i in range(n):
        y_error += abs(points[i].cy - (a*points[i].cx + b))

    direction = (a, b)
    return direction, y_error


