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
from simulation import *

# Parameters
if (len(sys.argv) != 4 and len(sys.argv) != 5) or (len(sys.argv) == 5 and sys.argv[4] != "-a"):
    print "USAGE: python " + sys.argv[0] + " [FILENAME] [START_FRAME] [END_FRAME] [-a]"
    exit(0)

start = int(sys.argv[2])
end   = int(sys.argv[3])

animation = False
if len(sys.argv) == 5 and sys.argv[4] == "-a":
    animation = True

# Loading data from input file
data = Data(sys.argv[1])

if start < data.start or end > data.end or start > end:
    print "Invalid frame interval"
    print "This data file can be animated in the interval [", \
        data.start,",",data.end,"]"
    exit(-1)

if animation:
    # TK Setup
    root = Tk()
    root.title("Simulation")
    frame = Frame(root, bd=5, relief=SUNKEN)
    frame.pack()
    canvas = Canvas(frame, width=WIDTH, height=HEIGHT)
    canvas.pack(side = TOP)
    frameNumber = StringVar()
    label = Label(frame, textvariable=frameNumber)
    label.pack(side = BOTTOM)
    sim = Simulation(canvas)
    root.update()

#

'''
done = 0
while not done:
    frame = end
    while frame + WIN_SIZE > end:
        o = data.get_random_object()
#o = data.objects[3] # fix me 
        frame = o.frame

    p = []
    for i in range(WIN_SIZE):
        if o:
            p.append(o)
            o = o.nearest(data.get_objects(o.frame + 1), DELTA/2)
        else:
            break

    if len(p) == WIN_SIZE:
        direction, y_error = least_squares_line(p)
        if y_error < MAX_Y_ERROR:
            done = 1
'''
w = []
subject = 0
holes = 0

print "Tracking and filling holes"
while data.objects:
    start_obj = data.get_random_object()
    tracked_obj = start_obj
    
    # Filter loop
    done = 0
    try:

        '''
        a = direction[0]
        b = direction[1]
        x0 = p[0].cx
        x1 = p[WIN_SIZE-1].cx
        if x0 < x1:
            sim.canvas.create_line(x0-15, a*(x0-15)+b, x1+15, a*(x1+15)+b, fill="red")
        else:
            sim.canvas.create_line(x1-15, a*(x1-15)+b, x0+15, a*(x0+15)+b, fill="red")
        for obj in p:
            sim.canvas.create_rectangle(obj.cx, obj.cy, obj.cx, obj.cy)
        root.update()
        sys.stdin.readline()
    
        tracked_obj = p[WIN_SIZE-1]
        '''
        frame = tracked_obj.frame + 1
    
        fail = []
        change = 0
        while (frame >= start and frame <= end and done != 2):
            found = None
            dt = 0
            while (found == None):
                objects = data.get_objects(frame)
                found = tracked_obj.nearest(objects, DELTA)
                if found != None:
                    if (dt > 0):
                        for f in fail:
                            w.append(f)
                            holes += 1
                    fail = []
                else:
                    o = Model(frame, subject, tracked_obj.height, \
                            tracked_obj.cx, tracked_obj.cy)
                    fail.append(o)
    
                    if done == 0:
                        frame += 1
                    else:
                        frame -= 1
    
                    dt += 1
                    if frame == end or dt == DELTA_T:
    #sys.stdin.readline()
                        done += 1
                        if done == 1:
                            tracked_obj
                        change = 1
                        found = 1
            if not change:
                if animation:
                    # Animation
                    frameNumber.set("Frame: "+str(frame))
                    tracked_obj = found
                    sim.add_point(tracked_obj)
                    sim.render(objects)
                    root.update() # redraw
#time.sleep(1.0/20)
                if (done == 0):
                    frame += 1
                else:
                    frame -= 1
            if change and done == 1:
                tracked_obj = start_obj
                frame = tracked_obj.frame - 1
                change = 0

            if found != 1:
                data.remove_object(found)
                w.append(Model(found.frame, subject, found.height, \
                            found.cx, found.cy))

        data.remove_object(start_obj)
        w.append(Model(start_obj.frame, subject, start_obj.height, \
                    start_obj.cx, start_obj.cy))
        if animation:
            sim.clear()
        subject += 1

    except TclError:
        pass # to avoid errors when the window is closed


print " ", holes, "holes were filled"

print "Filtering ghosts"

obj_s = []
for s in range(subject):
    obj_s.append([])

for obj in w:
    obj_s[obj.subject].append(obj)

ghosts = 0
for sub in obj_s:
    if len(sub) < MIN_SUB:
        for o in sub:
            w.remove(o)
            ghosts += 1

print " ", ghosts, "ghosts were removed"

frames = []
for f in range(start, end+1):
    frames.append([])

for obj in w:
    frames[obj.frame-start].append(obj)

outfilename = "out.conf"
print "Writing in "+outfilename

outfile = open(outfilename, "w")
outfile.write(str(start) +" "+ str(end) +"\n")
for frame in frames:
    outfile.write(str(len(frame)) + "\n")
    for obj in frame:
        outfile.write(str(obj))

