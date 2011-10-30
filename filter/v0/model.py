 # -*- coding: utf-8 -*-

import math

class Model:

    def __init__(self, f, s, h, x, y):
        self.frame = f    # frame
        self.subject = s  # model reference
        self.height = h   # height
        self.cx = x       # x coordinate
        self.cy = y       # y coordinate

    def __str__(self):
        return str(self.subject) +" "+ str(self.height) + \
        " "+ str(self.cx) +" "+ str(self.cy) +"\n"

    def pos(self):
        return (self.cx, self.cy)

    def dist2(self, obj):
        return (self.cx - obj.cx)*(self.cx - obj.cx) + (self.cy - obj.cy)*(self.cy - obj.cy)

    def dist(self, obj):
        return math.sqrt(self.dist2(obj))

    def nearest(self, objects, delta):
        dist = delta 
        n = None
        for obj in objects:
            temp = self.dist(obj)
            if dist > temp:
                dist = temp
                n = obj
        return n




