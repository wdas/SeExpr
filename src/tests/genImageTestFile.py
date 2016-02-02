#!/usr/bin/env python

import sys
import os
import re

###################################
def printUsage():
    print "Usage: genImageTestFile.py <dir> <outfile>"
    print "       traverse given dir for expr examples, write tests to outfile\n"
    print "       ex: genImageTestFile.py ./src/demos/imageSynth2/examples2 src/tests/testSeExprExamples.cpp"
    sys.exit()
###################################

### MAIN ###

if len(sys.argv) < 3:
    printUsage()

# get args
rootdir = sys.argv[1]
outfile = sys.argv[2]

# open outfile
f = open(outfile, 'w')
print >> f, "#include \"imageTests.h\"\n"

for dir_name, sub_dirs, se_files in os.walk(rootdir):
    for se_file in se_files:
        fullpath = os.path.join(dir_name, se_file)
        (filename,ext) = os.path.splitext(se_file)
        if(re.match('\.se', ext)):
            print >> f, "TEST(" + os.path.basename(dir_name) + ", " + filename + ")"
            print >> f, "{"
            print >> f, "    evalExpressionFile2(\"" + fullpath + "\");"
            print >> f, "}\n"

f.close()
