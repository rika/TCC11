 #!/usr/bin/python
 # -*- coding: utf-8 -*-

import sys, re
from model import Model

# files: ????-gpoints.conf

class FrameData:
    def __init__(self, frame):
        #self.frame = frame
        self.objects = []

        filename = str(frame) + "-gpoints.conf"
        infile = open(filename, "r")
        
        temp = infile.readlines()
        lines = []
        for line in temp:
            if not re.match(r'^\s*\n?$', line) and not re.match(r'^\[\d+\]$', line):
                lines.append(line)

        while len(lines) > 0:
            lines[0] = re.sub("\D", "", lines[0])
            lines[1] = re.sub(r'[\sh=]', "", lines[1])
            lines[2] = re.sub(r'\D', "", lines[2])
            lines[3] = re.sub(r'\D', "", lines[3])
            subject = int(lines[0])
            height = float(lines[1])
            cx = int(lines[2])
            cy = int(lines[3])
            self.objects.append(Model(frame, subject, height, cx, cy))
            lines = lines[4:]

        infile.close()

    def __str__(self):
        result = str(len(self.objects)) + "\n"
        for obj in self.objects:
            result += str(obj)
        return result



# SCRIPT
if len(sys.argv) != 3:
    print "USAGE: python " + sys.argv[0] + " [START_FRAME] [END_FRAME]"
    exit(0)

start = int(sys.argv[1])
end   = int(sys.argv[2])

outfile =  open("gpoints.conf", "w")
outfile.write(str(start) +" "+ str(end) +"\n")

#data = []
for frame in range(start, end+1):
    #data.append(FrameData(frame))
    outfile.write(str(FrameData(frame)))
